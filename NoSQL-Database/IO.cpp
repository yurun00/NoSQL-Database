#include "IO.h"

bool writeSSTables(const std::string fn, const std::vector<SSTable>& ssts) {
	if (compressMethod == "u")
		return writeSSTablesU(fn, ssts);
	else
		return true;
}

bool readSSTables(const std::string fn, std::vector<SSTable>& ssts) {
	if (compressMethod == "u")
		return readSSTablesU(fn, ssts);
	else
		return true;
}

bool writeSSTablesU(const std::string fn, const std::vector<SSTable>& ssts) {
	remove(fn.c_str());

	std::ofstream os;
	os.open(fn.c_str(), std::ios::out | std::ios::app);
	for (auto it = ssts.begin(); it != ssts.end(); it++) {
		os << "!" << "\n";
		os << it->title << "\n";
		os << "@" << "\n";
		for (auto mit = it->index.begin(); mit != it->index.end(); mit++) {
			os << mit->first << "," << std::to_string(mit->second) << "\n";
		}
		os << "@" << "\n";
		for (auto vit = it->data.begin(); vit != it->data.end(); vit++) {
			os << "#" << "\n";
			for (auto mit = vit->begin(); mit != vit->end(); mit++) {
				os << mit->first << "," << mit->second << "\n";
			}
			os << "#" << "\n";
		}
		os << "$" << "\n";
	}
	os.close();
	return true;
}

bool writeSSTablesD(const std::string fn, const std::vector<SSTable>& ssts) {
	return true;
}

bool readSSTablesU(const std::string fn, std::vector<SSTable>& ssts) {
	std::ifstream is;
	is.open(fn.c_str(), std::ios::in);
	std::string line;
	SSTable sst;
	while (std::getline(is, line)) {
		if (line == "!") {
			std::getline(is, line);
			sst.title = line;
		}
		else if (line == "@") {
			std::string colId, rowOffset;
			while (std::getline(is, line)) {
				if (line == "@") break;
				std::istringstream ss(line);
				std::getline(ss, colId, ',');
				std::getline(ss, rowOffset, ',');
				sst.index[colId] = atoi(rowOffset.c_str());
			}
		}
		else if (line == "#") {
			std::map<unsigned int, std::string> mp{};
			std::string rowId, val;
			while (std::getline(is, line)) {
				if (line == "#") break;
				std::istringstream ss(line);
				std::getline(ss, rowId, ',');
				std::getline(ss, val, ',');
				mp[atoi(rowId.c_str())] = val;
			}
			sst.data.push_back(mp);
		}
		else if (line == "$") {
			ssts.push_back(sst);
			sst = SSTable();
		}
	}
	is.close();
	return true;
}

bool readSSTablesD(const std::string fn, std::vector<SSTable>& ssts) {
	return true;
}

bool writeSSTablesB(const std::string fn, const std::vector<SSTable>& ssts);
bool readSSTablesB(const std::string fn, std::vector<SSTable>& ssts);

