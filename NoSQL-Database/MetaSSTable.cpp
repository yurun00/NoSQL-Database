#include "MetaSSTable.h"

bool MetaSSTable::setTitle(std::string ttl) {
	title = ttl;
	return true;
}

unsigned int MetaSSTable::getRowNum() {
	return rowNum;
}

unsigned int MetaSSTable::getPageId(unsigned int rowId) {
	return rowId / PAGE_SIZE;
}

unsigned int MetaSSTable::getRowStart(unsigned int rowId) {
	return getPageId(rowId) * PAGE_SIZE;
}

unsigned int MetaSSTable::getRowEnd(unsigned int rowId) {
	return std::min((getPageId(rowId) + 1) * PAGE_SIZE - 1, getRowNum() - 1);
}

unsigned int MetaSSTableManager::getRowNum(std::string colFamId) {
	return cfToMeta[colFamId].getRowNum();
}

bool MetaSSTableManager::addRow(std::string colFamId) {
	if (cfToMeta.find(colFamId) == cfToMeta.end()) {
		cfToMeta[colFamId] = MetaSSTable(colFamId);
	}
	cfToMeta[colFamId].rowNum++;
	return true;
}

bool MetaSSTableManager::createTable(std::string colFamId) {
	if (cfToMeta.find(colFamId) == cfToMeta.end()) {
		cfToMeta[colFamId] = MetaSSTable(colFamId);
		return true;
	}
	else return false;
}


