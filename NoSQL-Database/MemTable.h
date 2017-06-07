#pragma once
#include "Common.h"
#include "SSTable.h"
#include "IO.h"
#include "Globals.h"

class MemTable {
	friend class Controller;
	// column family id, column id, row id: value
	locToVal data;
	unsigned int curSize, maxSize;
	unsigned int version;
public:
	MemTable() : data(), curSize(0), maxSize(MAX_MEMTABLE_SIZE), version(0) {}
	MemTable(unsigned int cs, unsigned int ms, unsigned int vs) : data(), curSize(cs), maxSize(ms), version(vs) {}
	std::vector<SSTable> toSSTables();
	// update or add
	bool update(std::string colFamId, std::string colId, unsigned int rowId, std::string val);
	bool remove(std::string colFamId, std::string colId, unsigned int rowId);
	std::string read(std::string colFamId, std::string colId, unsigned int rowId);
	bool exist(std::string colFamId, std::string colId, unsigned int rowId);
};