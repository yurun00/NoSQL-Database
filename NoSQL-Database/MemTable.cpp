#include "MemTable.h"

std::vector<SSTable> MemTable::toSSTables() {
	// column family id: SSTable
	std::map<std::string, SSTable> cfToSST;
	for (auto mit = data.begin(); mit != data.end(); mit++) {
		for (auto mit1 = mit->second.begin(); mit1 != mit->second.end(); mit1++) {
			for (auto mit2 = mit1->second.begin(); mit2 != mit1->second.end(); mit2++) {
				std::string colFamId = mit->first, colId = mit1->first, val = mit2->second;
				unsigned int rowId = mit2->first;
				cfToSST[colFamId].addEntry(colId, rowId, val);
			}
		}
	}
	std::vector<SSTable> ssts;
	for (auto mit = cfToSST.begin(); mit != cfToSST.end(); mit++) {
		mit->second.setTitle(mit->first);
		ssts.push_back(mit->second);
	}
	return ssts;
}

bool MemTable::update(std::string colFamId, std::string colId, unsigned int rowId, std::string val) {
	// update location is not in memtable, add an entry and increase the size
	if(!exist(colFamId, colId, rowId))
		curSize++;
	else ;
	data[colFamId][colId][rowId] = val;

	return true;
}

bool MemTable::deleteEntry(std::string colFamId, std::string colId, unsigned int rowId) {
	// update location is not in memtable, add an entry and increase the size
	if (!exist(colFamId, colId, rowId))
		curSize++;
	else ;
	data[colFamId][colId][rowId] = TOMBSTONE;

	return true;
}

std::string MemTable::read(std::string colFamId, std::string colId, unsigned int rowId) {
	if (exist(colFamId, colId, rowId)) {
		return data[colFamId][colId][rowId];
	}
	else
		return "";
}

bool MemTable::exist(std::string colFamId, std::string colId, unsigned int rowId) {
	if (data.find(colFamId) == data.end() || data[colFamId].find(colId) == data[colFamId].end()
		|| data[colFamId][colId].find(rowId) == data[colFamId][colId].end())
		return false;
	return true;
}
