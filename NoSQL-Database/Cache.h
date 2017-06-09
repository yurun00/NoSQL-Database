#pragma once
#include "Common.h"
#include "Globals.h"

class Cache{
	// FIFO
	std::queue<std::map<std::string, std::string>::iterator> q;
	// column family id, column id, row id: value
	std::map<std::string, std::string> data;
	unsigned int maxSize;
public:
	Cache() :q(), data(), maxSize(MAX_CACHE_SIZE) {}
	// when write item, update the entry in the cache
	// when read item, add the page into the cache
	bool updateEntry(std::string colFamId, std::string colId, unsigned int rowId, std::string val);
	bool exist(std::string colFamId, std::string colId, unsigned int rowId);
	bool deleteEntry(std::string colFamId, std::string colId, unsigned int rowId);
	std::string readEntry(std::string colFamId, std::string colId, unsigned int rowId);
};

class CacheManager {
	friend class Controller;
	// map from version number to sstable file
	std::map<std::string, Cache> vToCcs;
public:
	bool exist(std::string colFamId, std::string colId, unsigned int rowId, std::string v);
	bool exist(std::string colFamId, std::string colId, unsigned int rowId);
	std::string readEntry(std::string colFamId, std::string colId, unsigned int rowId, std::string v);
};

