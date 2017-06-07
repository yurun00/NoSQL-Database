#pragma once
#include "Common.h"
#include "Types.h"

class SSTable{
	friend class Controller;
	friend class MemTable;
	friend bool writeSSTables(const std::string fileName, const std::vector<SSTable>& ssts);
	friend bool readSSTables(const std::string fileName, std::vector<SSTable>& ssts);
	// column family name
	std::string title;
	// column id: <row id, value> offset
	colToRows index;
	// vector element is data in a columns, in the form <row id: value>
	std::vector<rowToVal> data;
	// row id, value
	//std::vector<std::pair<std::string, unsigned int>> compressedData;
	// 'b': block compression; 'd': dictionary compression; 'u': uncompressed
	//char compress;
public:
	SSTable(): title(), index(), data() {
	}
	SSTable(std::string ttl, colToRows idx,
		std::vector<rowToVal> dat);
	void print();
	bool setTitle(const std::string ttl);
	bool setIndex(const colToRows idx);
	bool setData(const std::vector<rowToVal> dat);
	// sst1 is the later version, sst2 is the previous version, so if there is a conflict, 
	// we should always take data in sst1.
	bool addEntry(std::string colId, unsigned int rowId, std::string val);
	static std::vector<SSTable> mergeSSTableVecs(std::vector<SSTable>& sst1, std::vector<SSTable>& sst2);
	SSTable& mergeSSTables(const SSTable& sst1, const SSTable& sst2);
	rowToVal mergeData(const rowToVal& dat1, const rowToVal& dat2);
	// remove values of '!tombstone'
	bool removeTombstone();
};