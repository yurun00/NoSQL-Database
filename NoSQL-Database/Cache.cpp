#include "Cache.h"

bool Cache::updateEntry(std::string colFamId, std::string colId, unsigned int rowId, std::string val) {
	if (data.size() >= maxSize) {
		data.erase(q.front());
		q.pop();
	}
	

	std::string mKey = colFamId + "-" + colId + "-" + std::to_string(rowId);
	// add value
	if (data.find(mKey) == data.end()) {
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

std::string Cache::readEntry(std::string colFamId, std::string colId, unsigned int rowId) {
	std::string mKey = colFamId + "-" + colId + "-" + std::to_string(rowId);

	if (data.find(mKey) == data.end())
		return "";

	else return data[mKey];
}