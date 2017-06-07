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
	// if the location is not in memtable	
	else {
		std::string bfKey = colFamId + "-" + colId + "-" + std::to_string(rowId);
		for (auto vit = bfm.vToBfs.begin(); vit != bfm.vToBfs.end(); vit++) {
			// if the bloom filter indicates that the key is in the file
			if (vit->second.possiblyContains((const uint8_t*)bfKey.c_str(), bfKey.size())) {
				std::vector<SSTable> ssts;
				// read the sstables in the file and try to find the value
				std::string fn = "data/" + std::to_string(vit->first) + "-memTable.data";
				readSSTables(fn, ssts);
				for (auto svit = ssts.begin(); svit != ssts.end(); svit++) {
					if (svit->title == colFamId && svit->index.find(colId) != svit->index.end()
						&& svit->data.size() > svit->index[colId]
						&& svit->data[svit->index[colId]].find(rowId) != svit->data[svit->index[colId]].end()) {
						val = svit->data[svit->index[colId]][rowId];
						sst.addEntry(colId, rowId, val);
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

bool Controller::addRow(std::string colFamId, std::vector<std::string> colIds, std::vector<std::string> vals) {
	unsigned int rn = msm.getRowNum(colFamId);
	assert(colIds.size() == vals.size());
	for (unsigned int i = 0; i < colIds.size(); i++) {
		mt.update(colFamId, colIds[i], rn, vals[i]);
		// if current size greater than the maximum size, dump the memtable to the disk
		if (mt.curSize >= mt.maxSize) {
			// create bloom filter for the memtable file
			bfm.vToBfs[mt.version] = BloomFilter(BLOOM_FILTER_FALSE_POSITIVE_RATE, mt.curSize);
			for (auto mit = mt.data.begin(); mit != mt.data.end(); mit++) {
				for (auto mit1 = mit->second.begin(); mit1 != mit->second.end(); mit1++) {
					for (auto mit2 = mit1->second.begin(); mit2 != mit1->second.end(); mit2++) {
						std::string colFamId = mit->first, colId = mit1->first, val = mit2->second;
						unsigned int rowId = mit2->first;
						std::string bfKey = colFamId + "-" + colId + "-" + std::to_string(rowId);
						bfm.vToBfs[mt.version].add((const uint8_t*)bfKey.c_str(), bfKey.size());
					}
				}
			}
			dumpMt();
		}
	}
	msm.addRow(colFamId);
	return true;
}

bool Controller::dumpMt() {
	// convert the memtable to sstables and dump to disk as a json file
	std::string fn = "data/" + std::to_string(mt.version) + "-memTable.data";

	std::vector<SSTable> ssts = mt.toSSTables();

	writeSSTables(fn, ssts);

	mt.data = locToVal();
	mt.curSize = 0;
	mt.version++;
	return true;
}




