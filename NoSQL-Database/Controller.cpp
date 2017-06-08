#include "Controller.h"

bool Controller::readCol(const std::string colFamId, const std::string colId, SSTable& sst) {
	unsigned int rowNum = msm.getRowNum(colFamId);
	for (unsigned int i = 0; i < rowNum; i++) {
		readColRow(colFamId, colId, i, sst);
	}
	return true;
}

bool Controller::readColRow(const std::string colFamId, const std::string colId, const unsigned int rowId, SSTable& sst) {
	std::string val;
	// if the location is in memtable
	if (mt.exist(colFamId, colId, rowId)) {
		val = mt.read(colFamId, colId, rowId);
		sst.addEntry(colId, rowId, val);
	}
	// if the location is not in memtable, but in the cache 
	else if (cc.exist(colFamId, colId, rowId)){
		val = cc.readEntry(colFamId, colId, rowId);
		sst.addEntry(colId, rowId, val);
	}
	else {
		std::string bfKey = colFamId + "-" + colId + "-" + std::to_string(rowId);
		for (auto vit = bfm.vToBfs.begin(); vit != bfm.vToBfs.end(); vit++) {
			// if the bloom filter indicates that the key is in the file
			if (vit->second.possiblyContains((const uint8_t*)bfKey.c_str(), bfKey.size())) {
				std::vector<SSTable> ssts;
				// read the sstables in the file and try to find the value
				std::string fn = "data/" + vit->first + "-memTable.data";
				readSSTables(fn, ssts);
				for (auto svit = ssts.begin(); svit != ssts.end(); svit++) {
					// if the value exist in this sstable
					if (svit->title == colFamId && svit->index.find(colId) != svit->index.end()
						&& svit->data.size() > svit->index[colId]
						&& svit->data[svit->index[colId]].find(rowId) != svit->data[svit->index[colId]].end()) {
						val = svit->data[svit->index[colId]][rowId];
						sst.addEntry(colId, rowId, val);
						// add the page where the value is in to the cache
						unsigned int start = (rowId / PAGE_SIZE) * PAGE_SIZE;
						unsigned int end = std::min((rowId / PAGE_SIZE + 1) * PAGE_SIZE - 1, svit->data[svit->index[colId]].size() - 1);
						for (unsigned int i = start; i <= end; i++) {
							cc.updateEntry(colFamId, colId, rowId, svit->data[svit->index[colId]][i]);
						}
						return true;
					}
				}
				// the value is not in these sstables, continue to next file, check the bloom filter first
				// bloom filter false positive number
				bfFp++;
			}
			bfNg++;
		}
	}
	return true;
}

// when memtable is dumped to the disk as a file, create a bloom filter for that file with current memtable version
bool Controller::addMemTableBF() {
	// initialize bloom filter with false positive error rate and number of elements inserted
	bfm.vToBfs["m" + std::to_string(mt.version)] = BloomFilter(BLOOM_FILTER_FALSE_POSITIVE_RATE, mt.curSize);
	// add each element to the bloom filter
	for (auto mit = mt.data.begin(); mit != mt.data.end(); mit++) {
		for (auto mit1 = mit->second.begin(); mit1 != mit->second.end(); mit1++) {
			for (auto mit2 = mit1->second.begin(); mit2 != mit1->second.end(); mit2++) {
				std::string colFamId = mit->first, colId = mit1->first, val = mit2->second;
				unsigned int rowId = mit2->first;
				std::string bfKey = colFamId + "-" + colId + "-" + std::to_string(rowId);
				bfm.vToBfs["m" + std::to_string(mt.version)].add((const uint8_t*)bfKey.c_str(), bfKey.size());
			}
		}
	}
	return true;
}

bool Controller::addSSTablesBF(const std::vector<SSTable>& ssts, unsigned int version) {
	unsigned int size = 0;
	for (auto vit = ssts.begin(); vit != ssts.end(); vit++) {
		for (auto vit1 = vit->data.begin(); vit1 != vit->data.end(); vit1++) {
			size += vit1->size();
		}
	}
	bfm.vToBfs["m" + std::to_string(version)] = BloomFilter(BLOOM_FILTER_FALSE_POSITIVE_RATE, size);
	for (auto vit = ssts.begin(); vit != ssts.end(); vit++) {
		for (auto vit1 = vit->index.begin(); vit1 != vit->index.end(); vit1++) {
			for (auto mit = vit->data[vit1->second].begin(); mit != vit->data[vit1->second].end(); mit++) {
				std::string colFamId = vit->title, colId = vit1->first, val = mit->second;
				unsigned int rowId = mit->first;
				std::string bfKey = colFamId + "-" + colId + "-" + std::to_string(rowId);
				bfm.vToBfs["m" + std::to_string(version)].add((const uint8_t*)bfKey.c_str(), bfKey.size());
			}
		}
	}
	return true;
}


bool Controller::addRow(std::string colFamId, std::vector<std::string> colIds, std::vector<std::string> vals) {
	unsigned int rn = msm.getRowNum(colFamId), mtVersion = mt.version;
	assert(colIds.size() == vals.size());
	for (unsigned int i = 0; i < colIds.size(); i++) {
		mt.update(colFamId, colIds[i], rn, vals[i]);
		cc.updateEntry(colFamId, colIds[i], rn, vals[i]);
		// if current size greater than or equal to the maximum size, dump the memtable to the disk
		if (mt.curSize >= mt.maxSize) {
			// create bloom filter for the memtable file
			addMemTableBF();
			dumpMt();
			// if current version greater than or equal to the maximum version, compact the memtable files on disk
			if (mtVersion >= mt.maxVersion) {
				// compact files on the disk
				std::vector<SSTable> ssts1, ssts2, ssts3;
				unsigned int v = mt.maxVersion;
				std::string fn = "data/m" + std::to_string(v) + "-memTable.data";
				readSSTables(fn, ssts1);
				// delete file and bloom filter after loading into the memory
				remove(fn.c_str());
				bfm.vToBfs.erase("m" + std::to_string(v));

				while (v > 0) {
					fn = "data/m" + std::to_string(--v) + "-memTable.data";
					readSSTables(fn, ssts2);
					remove(fn.c_str());
					bfm.vToBfs.erase("m" + std::to_string(v));
					// compact sstables in ssts1 and ssts2 to generate ssts3
					// ssts1 is new, ssts2 is old, when there are conflicts, chose values in ssts1
					ssts3 = SSTable::mergeSSTableVecs(ssts1, ssts2);

					ssts1 = ssts3;
					ssts2 = std::vector<SSTable>{};
				}
				fn = "data/s" + std::to_string(dataPartNumber) + "-SSTable.data";
				writeSSTables(fn, ssts1);
				
				// manage bloom filters in the memory
				addSSTablesBF(ssts1, dataPartNumber++);

				mt.version = 0;
			}
		}
	}
	msm.addRow(colFamId);
	return true;
}

bool Controller::dumpMt() {
	// convert the memtable to sstables and dump to disk as a json file
	std::string fn = "data/m" + std::to_string(mt.version) + "-memTable.data";

	std::vector<SSTable> ssts = mt.toSSTables();

	writeSSTables(fn, ssts);

	mt.data = locToVal();
	mt.curSize = 0;
	mt.version++;
	return true;
}




