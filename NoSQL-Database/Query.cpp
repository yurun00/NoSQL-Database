#include "Query.h"

SSTable& Query::getResult() {
	return result;
}

Query& Query::select(std::vector<std::string> colIds) {
	result = SSTable();
	mColIds = colIds;
	return *this;
}

SSTable& Query::from(std::string cfId) {
	mCfId = cfId;
	if (mColIds.size() == 0) return result;

	for (auto vit = mColIds.begin(); vit != mColIds.end(); vit++) {
		cfId += "-" + *vit;
		ctl.readCol(mCfId, *vit, result);
	}
	result.setTitle(cfId);
	return result;
}

Query& Query::insert(std::vector<std::string> colIds, std::vector<std::string> vals) {
	assert(colIds.size() == vals.size());
	result = SSTable();
	mColIds = colIds;
	mVals = vals;
	return *this;
}

SSTable& Query::into(std::string cfId) {
	mCfId = cfId;
	assert(mColIds.size() == mVals.size());
	ctl.addRow(mCfId, mColIds, mVals);
	return result;
}

Query& Query::createTable(std::string cfId) {
	ctl.msm.createTable(cfId);
	return *this;
}

//SSTable& Query::where(std::string cond) {
//
//}
//
//SSTable& Query::join(const SSTable& sst1, const SSTable& sst2) {
//
//}
//
//SSTable& Query::on(std::string cond) {
//
//}