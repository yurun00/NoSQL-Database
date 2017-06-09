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
		if (val == TOMBSTONE)
			return false;
		sst.addEntry(colId, rowId, val);
		return true;
	}
	else {
		std::string bfKey = colFamId + "-" + colId + "-" + std::to_string(rowId);
		for (int mn = mt.version - 1; mn >= 0; mn--) {
			// if the bloom filter indicates that the key is in the memtable file
			std::string mKey = "m" + std::to_string(mn);
			if (bfm.vToBfs[mKey]
				.possiblyContains((const uint8_t*)bfKey.c_str(), bfKey.size())) {
				// if the value is in cache of the file
				if (ccm.exist(colFamId, colId, rowId, mKey)) {
					val = ccm.readEntry(colFamId, colId, rowId, mKey);
					if (val == TOMBSTONE)
						return false;
					sst.addEntry(colId, rowId, val);
					return true;
				}
				// else read from the file
				std::string fn = "data/" + mKey + "-memTable.data";
				if (readColRowFrom(colFamId, colId, rowId, fn, sst))
					return true;
			}
			bfNg++;
		}
		for (int sn = dataPartNumber - 1; sn >= 0; sn--) {
			// if the bloom filter indicates that the key is in the sstable file
			std::string mKey = "s" + std::to_string(sn);
			if (bfm.vToBfs[mKey]
				.possiblyContains((const uint8_t*)bfKey.c_str(), bfKey.size())) {
				// if the value is in cache of the file
				if (ccm.exist(colFamId, colId, rowId, mKey)) {
					val = ccm.readEntry(colFamId, colId, rowId, mKey);
					if (val == TOMBSTONE)
						return false;
					sst.addEntry(colId, rowId, val);
					return true;
				}
				std::string fn = "data/" + mKey + "-SSTable.data";
				if (readColRowFrom(colFamId, colId, rowId, fn, sst))
					return true;
			}
			bfNg++;
		}
	}
	return false;
}

// read entry from the file return true if find, false if not
bool Controller::readColRowFrom(const std::string colFamId, const std::string colId, const unsigned int rowId, std::string fn, SSTable& sst) {
	std::string val;
	std::vector<SSTable> ssts;
	// read the sstables in the file and try to find the value
	readSSTables(fn, ssts);
	for (auto svit = ssts.begin(); svit != ssts.end(); svit++) {
		// if the value exist in this sstable
		if (svit->title == colFamId && svit->index.find(colId) != svit->index.end()
			&& svit->data.size() > svit->index[colId]
			&& svit->data[svit->index[colId]].find(rowId) != svit->data[svit->index[colId]].end()) {
			val = svit->data[svit->index[colId]][rowId];
			// return true and do not add to sstable, so the caller will return
			if (val == TOMBSTONE)
				return true;
			sst.addEntry(colId, rowId, val);
			// add the page where the value is in to the cache of the file
			std::string mKey = fn.substr(fn.find("/")+1, fn.find("-")-fn.find("/")-1);
			unsigned int start = (rowId / PAGE_SIZE) * PAGE_SIZE;
			unsigned int end = std::min((rowId / PAGE_SIZE + 1) * PAGE_SIZE - 1, svit->data[svit->index[colId]].size() - 1);
			for (unsigned int i = start; i <= end; i++) {
				ccm.vToCcs[mKey].updateEntry(colFamId, colId, i, svit->data[svit->index[colId]][i]);
			}
			return true;
		}
	}
	// the value is not in these sstables, continue to next file, check the bloom filter first
	// bloom filter false positive number
	bfFp++;
	return false;
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
	bfm.vToBfs["s" + std::to_string(version)] = BloomFilter(BLOOM_FILTER_FALSE_POSITIVE_RATE, size);
	for (auto vit = ssts.begin(); vit != ssts.end(); vit++) {
		for (auto mit1 = vit->index.begin(); mit1 != vit->index.end(); mit1++) {
			for (auto mit = vit->data[mit1->second].begin(); mit != vit->data[mit1->second].end(); mit++) {
				std::string colFamId = vit->title, colId = mit1->first, val = mit->second;
				unsigned int rowId = mit->first;
				std::string bfKey = colFamId + "-" + colId + "-" + std::to_string(rowId);
				bfm.vToBfs["s" + std::to_string(version)].add((const uint8_t*)bfKey.c_str(), bfKey.size());
			}
		}
	}
	return true;
}

bool Controller::deleteEntry(std::string cfId, std::string colId, unsigned int rowId) {
	mt.deleteEntry(cfId, colId, rowId);
	// if current size greater than or equal to the maximum size, dump the memtable to the disk
	if (mt.curSize >= mt.maxSize) {
		flushAndCompaction();
	}
	return true;
}

bool Controller::updateEntry(std::string cfId, std::string colId, unsigned int rowId, std::string val) {
	mt.update(cfId, colId, rowId, val);
	// if current size greater than or equal to the maximum size, dump the memtable to the disk
	if (mt.curSize >= mt.maxSize) {
		flushAndCompaction();
	}
	return true;
}

bool Controller::addRow(std::string colFamId, std::vector<std::string> colIds, std::vector<std::string> vals) {
	unsigned int rn = msm.getRowNum(colFamId);
	assert(colIds.size() == vals.size());
	for (unsigned int i = 0; i < colIds.size(); i++) {
		mt.update(colFamId, colIds[i], rn, vals[i]);
		// if current size greater than or equal to the maximum size, dump the memtable to the disk
		if (mt.curSize >= mt.maxSize) {
			flushAndCompaction();
		}
	}
	msm.addRow(colFamId);
	return true;
}

bool Controller::flushAndCompaction() {
	unsigned int mtVersion = mt.version;
	// create bloom filter for the memtable file
	addMemTableBF();
	flushMt();
	// if current version greater than or equal to the maximum version, compact the memtable files on disk
	if (mtVersion >= mt.maxVersion) {
		// compact memtable files on the disk, and update sstable files
		compaction();
	}
	return true;
}

bool Controller::compaction() {
	compactMemOrSST("m");
	compactMemOrSST("s");
	// update caches
	ccm.vToCcs = std::map<std::string, Cache>{};
	for (unsigned int i = 0; i < dataPartNumber; i++) {
		ccm.vToCcs["s" + std::to_string(i)] = Cache{};
	}

	return true;
}

bool Controller::compactMemOrSST(std::string mOrS) {
	std::vector<SSTable> ssts1, ssts2, ssts3;
	int v = (mOrS == "m" ? MAX_MEMTABLE_VERSION : dataPartNumber - 1);
	assert(v >= 0);
	std::string fn = "data/" + mOrS + std::to_string(v) + (mOrS == "m" ? "-memTable.data" : "-SSTable.data");

	// delete memtable files and bloom filters after loading into the memory
	readSSTables(fn, ssts1);
	remove(fn.c_str());
	bfm.vToBfs.erase(mOrS + std::to_string(v));

	while (v > 0) {
		fn = "data/" + mOrS + std::to_string(--v) + (mOrS == "m" ? "-memTable.data" : "-SSTable.data");
		readSSTables(fn, ssts2);
		remove(fn.c_str());
		bfm.vToBfs.erase(mOrS + std::to_string(v));
		// compact sstables in ssts1 and ssts2 to generate ssts3
		// ssts1 is new, ssts2 is old, when there are conflicts, chose values in ssts1
		ssts3 = SSTable::mergeSSTableVecs(ssts1, ssts2);
		ssts1 = ssts3;
		ssts2 = std::vector<SSTable>{};
	}
	if (mOrS == "m") {
		fn = "data/s" + std::to_string(dataPartNumber) + "-SSTable.data";
		writeSSTables(fn, ssts1);
		// manage bloom filters in the memory
		addSSTablesBF(ssts1, dataPartNumber);
		dataPartNumber++;
		mt.version = 0;
	}
	else {
		assert(mOrS == "s");
		unsigned int curDataPart = 0;
		int remInFile = MAX_SSTABLE_SIZE;
		std::vector<SSTable> ssts;
		for (auto vit = ssts1.begin(); vit != ssts1.end(); vit++) {
			vit->removeTombstone();
		}
		for (auto vit = ssts1.begin(); vit != ssts1.end(); vit++) {
			for (auto mit = vit->index.begin(); mit != vit->index.end(); mit++) {
				for (auto mit1 = vit->data[mit->second].begin(); mit1 != vit->data[mit->second].end(); mit1++) {
					if (remInFile == 0) {
						remInFile = MAX_SSTABLE_SIZE;
						fn = "data/s" + std::to_string(curDataPart) + "-SSTable.data";
						writeSSTables(fn, ssts);
						// manage bloom filters in the memory
						addSSTablesBF(ssts, curDataPart);
						curDataPart++;
						ssts = std::vector<SSTable>{};
					}
					remInFile--;
					std::string colFamId = vit->title, colId = mit->first, val = mit1->second;
					unsigned int rowId = mit1->first;
					if (ssts.size() == 0 || ssts.back().title != colFamId)
						ssts.push_back(SSTable{ colFamId });
					if (ssts.back().index.find(colId) == ssts.back().index.end()) {
						ssts.back().index[colId] = ssts.back().data.size();
						ssts.back().data.push_back(rowToVal{});
						ssts.back().data.back()[rowId] = val;
					}
					else {
						assert(ssts.back().index[colId] == ssts.back().data.size() - 1);
						ssts.back().data.back()[rowId] = val;
					}
				}
			}
		}
		if (remInFile != MAX_SSTABLE_SIZE) {
			fn = "data/s" + std::to_string(curDataPart) + "-SSTable.data";
			writeSSTables(fn, ssts);
			// manage bloom filters in the memory
			addSSTablesBF(ssts, curDataPart);
			curDataPart++;
		}
		dataPartNumber = curDataPart;
	}
	return true;
}

bool Controller::flushMt() {
	// convert the memtable to sstables and dump to disk as a json file
	std::string fn = "data/m" + std::to_string(mt.version) + "-memTable.data";

	std::vector<SSTable> ssts = mt.toSSTables();

	writeSSTables(fn, ssts);

	mt.data = locToVal();
	mt.curSize = 0;
	mt.version++;

	ccm.vToCcs["m" + std::to_string(mt.version)] = Cache{};
	return true;
}




