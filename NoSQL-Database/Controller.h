#pragma once
#include "Common.h"
#include "SSTable.h"
#include "MetaSSTable.h"
#include "MemTable.h"
#include "BloomFilter.h"
#include "Cache.h"

class Controller{
	friend class Query;
	MetaSSTableManager msm;
	MemTable mt;
	BloomFilterManager bfm;
	Cache cc;
public:
	Controller() : msm(), mt(), bfm() {}
	bool readCol(const std::string colFamId, const std::string colId, SSTable& sst);
	bool readColRow(const std::string colFamId, const std::string colId, const unsigned int rowId, SSTable& sst);
	bool addRow(std::string colFamId, std::vector<std::string> colIds, std::vector<std::string> vals);
	bool addSSTablesBF(const std::vector<SSTable>& ssts, unsigned int version);
	bool addMemTableBF();
	bool dumpMt();
};