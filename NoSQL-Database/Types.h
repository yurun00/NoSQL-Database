#pragma once
#include "Common.h"
typedef std::map<std::string, std::map<std::string, std::map<unsigned int, std::string>>> locToVal;
typedef std::map<std::string, unsigned int> colToRows;
typedef std::map<unsigned int, std::string> rowToVal;
typedef std::map<std::string, std::map<std::string, unsigned int>> colToValToIdx;
typedef std::map<std::string, std::map<unsigned int, std::string>> colToIdxToVal;
