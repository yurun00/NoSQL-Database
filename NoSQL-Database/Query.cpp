#include "Query.h"

SSTable& Query::getResult() {
	return result;
}

Query& Query::select(std::vector<std::string> colIds) {
	result = SSTable();
	mColIds = colIds;
	return *this;
}

Query& Query::from(std::string cfId) {
	mCfId = cfId;
	if (mColIds.size() == 0) return *this;

	for (auto vit = mColIds.begin(); vit != mColIds.end(); vit++) {
		cfId += "-" + *vit;
		ctl.readCol(mCfId, *vit, result);
	}
	result.setTitle(cfId);
	return *this;
}

SSTable& Query::where(std::string colId, std::string val) {
	mWhereColId = colId;
	mWhereVal = val;
	// add the column in the condition to result
	ctl.readCol(mCfId, mWhereColId, result);
	// extract invalid row ids in that column
	std::set<unsigned int> invalidRowIds;
	auto rtv = result.data[result.index[mWhereColId]];
	for (auto rit = rtv.begin(); rit != rtv.end(); rit++) {
		if (rit->second != val) {
			invalidRowIds.insert(rit->first);
		}
	}
	// remove invalid row ids and values from other columns
	for (auto mit = result.index.begin(); mit != result.index.end(); mit++) {
		for (auto sit = invalidRowIds.begin(); sit != invalidRowIds.end(); sit++) {
			if (result.data[mit->second].find(*sit) != result.data[mit->second].end()) {
				result.data[mit->second].erase(*sit);
			}
		}
	}
	// remove the column in the condition
	if (std::find(mColIds.begin(), mColIds.end(), colId) == mColIds.end()) {
		result.data.erase(result.data.begin() + result.index[colId]);
		result.index.erase(colId);
	}
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

//
//SSTable& Query::join(const SSTable& sst1, const SSTable& sst2) {
//
//}
//
//SSTable& Query::on(std::string cond) {
//
//}