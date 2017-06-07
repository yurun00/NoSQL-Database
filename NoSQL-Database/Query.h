#pragma once
#include "Common.h"
#include "MemTable.h"
#include "SSTable.h"
#include "Controller.h"

class Query{
	Controller ctl;

	SSTable result;
	std::vector<std::string> mColIds;
	std::vector<std::string> mVals;
	std::string mCfId;
	std::string mWhereCond;
	SSTable* mSST1;
	SSTable* mSST2;
public:
	Query() : ctl(), result(), mColIds(), mVals(), mCfId(), mWhereCond()
		, mSST1(), mSST2() {}
	Query(Controller controller) : ctl(controller) {}
	SSTable& getResult();
	Query& select(std::vector<std::string> colIds);
	SSTable& from(std::string cfId);
	SSTable& where(std::string cond);
	SSTable& join(const SSTable& sst1, const SSTable& sst2);
	SSTable& on(std::string cond);

	Query& insert(std::vector<std::string> colIds, std::vector<std::string> vals);
	SSTable& into(std::string cfId);

	Query& createTable(std::string cfId);
};
