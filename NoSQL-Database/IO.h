#pragma once
#include "Common.h"
#include "SSTable.h"
#include "Globals.h"
#include <fstream>
#include <sstream>

bool writeSSTables(const std::string fileName, const std::vector<SSTable>& ssts);
bool readSSTables(const std::string fileName, std::vector<SSTable>& ssts);
bool writeSSTablesU(const std::string fileName, const std::vector<SSTable>& ssts);
bool readSSTablesU(const std::string fileName, std::vector<SSTable>& ssts);
bool writeSSTablesB(const std::string fileName, const std::vector<SSTable>& ssts);
bool readSSTablesB(const std::string fileName, std::vector<SSTable>& ssts);
bool writeSSTablesD(const std::string fileName, const std::vector<SSTable>& ssts);
bool readSSTablesD(const std::string fileName, std::vector<SSTable>& ssts);