#include "SSTable.h"

SSTable::SSTable(std::string ttl, std::map<std::string, unsigned int> idx,
	std::vector<std::map<unsigned int, std::string>> dat) : title(ttl), index(idx), data(dat) {

}

void SSTable::print() {
	std::cout << "table title: " << title << "\n";
	std::cout << "column index: " << "\n";
	for (std::map<std::string, unsigned int>::const_iterator mit = index.begin(); mit != index.end(); mit++) {
		std::cout << mit->first << "," << std::to_string(mit->second) << "\n";
	}
	std::cout << "row data:" << "\n";
	for (std::vector<std::map<unsigned int, std::string>>::const_iterator vit = data.begin(); vit != data.end(); vit++) {
		for (std::map<unsigned int, std::string>::const_iterator mit = vit->begin(); mit != vit->end(); mit++) {
			std::cout << std::to_string(mit->first) << "," << mit->second << "\n";
		}
	}
	return ;
}

void SSTable::printAsTable() {
	std::cout << "table title: " << title << "\n";
	std::cout << "columns: \t";
	for (auto mit = index.begin(); mit != index.end(); mit++) {
		std::cout << mit->first << "\t";
	}
	std::cout << std::endl;
	unsigned int maxRowId = 0;
	for (auto mit = index.begin(); mit != index.end(); mit++) {
		unsigned int t = data[mit->second].rbegin()->first;
		if (t > maxRowId) maxRowId = t;
	}
	for (unsigned int i = 0; i <= maxRowId; i++) {
		std::cout << "id: " << i << "\t\t";
		for (auto mit = index.begin(); mit != index.end(); mit++) {
			if (data[mit->second].find(i) != data[mit->second].end()) {
				std::cout << data[mit->second][i] << "\t";
			}
			else {
				std::cout << "\t";
			}
		}
		std::cout << std::endl;
	}
	return ;
}

bool SSTable::setTitle(const std::string ttl) {
	title = ttl;
	return true;
}

bool SSTable::setIndex(const std::map<std::string, unsigned int> idx) {
	index = idx;
	return true;
}

bool SSTable::setData(const std::vector<std::map<unsigned int, std::string>> dat) {
	data = dat;
	return true;
}

bool SSTable::addEntry(std::string colId, unsigned int rowId, std::string val) {
	// column exists
	if(index.find(colId) != index.end())
		data[index[colId]][rowId] = val;
	// column does not exist
	else {
		unsigned int offset = data.size();
		data.push_back(rowToVal());
		data[offset][rowId] = val;

		index[colId] = offset;
	}
	return true;
}

std::vector<SSTable> SSTable::mergeSSTableVecs(std::vector<SSTable>& ssts1, std::vector<SSTable>& ssts2) {
	std::map<std::string, SSTable> cfToSST;
	std::vector<SSTable> ssts;
	for (auto vit1 = ssts1.begin(); vit1 != ssts1.end(); vit1++) {
		cfToSST[vit1->title] = *vit1;
	}
	for (auto vit2 = ssts2.begin(); vit2 != ssts2.end(); vit2++) {
		if (cfToSST.find(vit2->title) == cfToSST.end()) {
			cfToSST[vit2->title] = *vit2;
		}
		else {
			SSTable sst = cfToSST[vit2->title];
			cfToSST[vit2->title].mergeSSTables(sst, *vit2);
		}
	}
	for (auto mit = cfToSST.begin(); mit != cfToSST.end(); mit++) {
		ssts.push_back(mit->second);
	}
	return ssts;
}

// remove tombstone entries
SSTable& SSTable::mergeSSTables(const SSTable& sst1, const SSTable& sst2) {
	if (sst1.title != sst2.title) {
		std::cout << "Different tables cannot be merged." << std::endl;
		return *this;
	}
	*this = SSTable();
	title = sst1.title;
	auto mit1 = sst1.index.begin(), mit2 = sst2.index.begin();
	unsigned int cnt = 0;
	while (mit1 != sst1.index.end() || mit2 != sst2.index.end()) {
		if (mit1 == sst1.index.end() || (mit2 != sst2.index.end() && mit1->first > mit2->first)) {
			index[mit2->first] = cnt;
			data.push_back(sst2.data[mit2->second]);
			cnt++;
			mit2++;
		}
		else if (mit2 == sst2.index.end() || mit1->first < mit2->first) {
			index[mit1->first] = cnt;
			data.push_back(sst1.data[mit1->second]);
			cnt++;
			mit1++;
		}
		else {
			rowToVal rtv = mergeData(sst1.data[mit1->second], sst2.data[mit2->second]);
			index[mit1->first] = cnt;
			data.push_back(rtv);
			cnt++;
			mit1++;
			mit2++;
		}
		// remove tombstone entries
		auto mit = data.back().begin();
		while (mit != data.back().end())
		{
			if (mit->second == TOMBSTONE)
				data.back().erase(mit++);
			else
				mit++;
		}
	}
	return *this;
}

rowToVal SSTable::mergeData(const rowToVal& dat1, const rowToVal& dat2) {
	rowToVal dat;
	for (rowToVal::const_iterator vit1 = dat1.begin(); vit1 != dat1.end(); vit1++) {
		dat[vit1->first] = vit1->second;
	}
	for (rowToVal::const_iterator vit2 = dat2.begin(); vit2 != dat2.end(); vit2++) {
		if(dat.find(vit2->first) == dat.end())
			dat[vit2->first] = vit2->second;
	}
	return dat;
}