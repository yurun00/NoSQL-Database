#pragma once
#include "Common.h"
#include "Globals.h"

class MetaSSTable {
	friend class MetaSSTableManager;
	// column family Id
	std::string title;
	// row number
	unsigned int rowNum;
public:
	MetaSSTable() : title(""), rowNum(0) {}
	MetaSSTable(std::string ttl) : title(ttl), rowNum(0) {}
	bool setTitle(std::string ttl);
	unsigned int getRowNum();
	unsigned int getPageId(unsigned int rowId);
	unsigned int getRowStart(unsigned int rowId);
	unsigned int getRowEnd(unsigned int rowId);
};

class MetaSSTableManager {
	friend class Controller;
	std::map<std::string, MetaSSTable> cfToMeta;
public:
	unsigned int getRowNum(std::string colFamId);
	bool addRow(std::string colFamId);
	bool createTable(std::string colFamId);
};