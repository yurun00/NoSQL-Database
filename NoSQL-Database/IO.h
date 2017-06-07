#pragma once
#include "Common.h"
#include "SSTable.h"
#include <fstream>
#include <sstream>

bool writeSSTables(const std::string fileName, const std::vector<SSTable>& ssts);
bool readSSTables(const std::string fileName, std::vector<SSTable>& ssts);