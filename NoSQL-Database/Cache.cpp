#include "Cache.h"

bool Cache::updateEntry(std::string colFamId, std::string colId, unsigned int rowId, std::string val) {
	std::string mKey = colFamId + "-" + colId + "-" + std::to_string(rowId);

	// add value
	if (data.find(mKey) == data.end()) {
		if (data.size() >= maxSize) {
			data.erase(q.front());
			q.pop();
		}
		data[mKey] = val;
		q.push(data.find(mKey));
	}
	// update value
	else {
		data[mKey] = val;
	}
	//std::cout << "cache size: " << data.size() << std::endl;
	return true;
}

bool Cache::exist(std::string colFamId, std::string colId, unsigned int rowId) {
	std::string mKey = colFamId + "-" + colId + "-" + std::to_string(rowId);
	
	if (data.find(mKey) == data.end())
		return false;

	return true;
}

bool Cache::deleteEntry(std::string colFamId, std::string colId, unsigned int rowId) {
	std::string mKey = colFamId + "-" + colId + "-" + std::to_string(rowId);
	if (data.find(mKey) == data.end())
		return false;

	data[mKey] = TOMBSTONE;
	return true;
}


std::string Cache::readEntry(std::string colFamId, std::string colId, unsigned int rowId) {
	std::string mKey = colFamId + "-" + colId + "-" + std::to_string(rowId);

	if (data.find(mKey) == data.end())
		return "";

	else return data[mKey];
}

bool CacheManager::exist(std::string colFamId, std::string colId, unsigned int rowId, std::string v) {
	if (vToCcs.find(v) == vToCcs.end())
		return false;

	return vToCcs[v].exist(colFamId, colId, rowId);
}

bool CacheManager::exist(std::string colFamId, std::string colId, unsigned int rowId) {
	for (auto mit = vToCcs.begin(); mit != vToCcs.end(); mit++) {
		if (mit->second.exist(colFamId, colId, rowId))
			return true;
	}
	return false;
}

std::string CacheManager::readEntry(std::string colFamId, std::string colId, unsigned int rowId, std::string v) {
	std::string mKey = colFamId + "-" + colId + "-" + std::to_string(rowId);

	assert(vToCcs.find(v) != vToCcs.end());
	if (vToCcs[v].exist(colFamId, colId, rowId))
		return vToCcs[v].readEntry(colFamId, colId, rowId);

	return "";
}

