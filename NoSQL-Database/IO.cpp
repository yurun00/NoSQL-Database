#include "IO.h"

bool writeSSTables(const std::string fn, const std::vector<SSTable>& ssts) {
	if (ssts.size() == 0)
		return false;
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

bool writeSSTablesD(std::string fn, std::vector<SSTable>& ssts) {
	remove(fn.c_str());
	for (auto vit = ssts.begin(); vit != ssts.end(); vit++) {
		for (auto mit = vit->index.begin(); mit != vit->index.end(); mit++) {
			unsigned int cnt = 0;
			for (auto mit1 = vit->data[mit->second].begin(); mit1 != vit->data[mit->second].end(); mit1++) {
				if (vit->oDict[mit->first].find(mit1->second) == vit->oDict[mit->first].end()) {
					vit->oDict[mit->first][mit1->second] = cnt;
					cnt++;
				}
				mit1->second = std::to_string(vit->oDict[mit->first][mit1->second]);
			}
		}
	}

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

		for (auto mit = it->oDict.begin(); mit != it->oDict.end(); mit++) {
			os << "%" << "\n";
			os << mit->first << "\n";
			for (auto mit1 = mit->second.begin(); mit1 != mit->second.end(); mit1++) {
				os << mit1->first << "," << mit1->second << "\n";
			}
			os << "%" << "\n";
		}

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

bool readSSTablesD(std::string fn, std::vector<SSTable>& ssts) {
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
		else if (line == "%") {
			std::string colId, val, code;
			std::getline(is, colId);
			while (std::getline(is, line)) {
				if (line == "%") break;
				std::istringstream ss(line);
				std::getline(ss, val, ',');
				std::getline(ss, code, ',');
				sst.iDict[colId][atoi(code.c_str())] = val;
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
			for (auto mit = sst.index.begin(); mit != sst.index.end(); mit++) {
				for (auto mit1 = sst.data[mit->second].begin(); mit1 != sst.data[mit->second].end(); mit1++) {
					mit1->second = sst.iDict[mit->first][atoi((mit1->second).c_str())];
				}
			}
			ssts.push_back(sst);
			sst = SSTable();
		}
	}
	is.close();
	return true;
}

bool writeSSTablesB(const std::string fn, const std::vector<SSTable>& ssts);
bool readSSTablesB(const std::string fn, std::vector<SSTable>& ssts);

