#pragma once
#include "Common.h"
extern const unsigned int PAGE_SIZE;
extern const unsigned int MAX_MEMTABLE_SIZE;
extern const unsigned int MAX_MEMTABLE_VERSION;
extern const unsigned int MAX_CACHE_SIZE;
extern const unsigned int MAX_SSTABLE_SIZE;
extern const float BLOOM_FILTER_FALSE_POSITIVE_RATE;
extern const std::string TOMBSTONE;
extern unsigned int bfFp;
extern unsigned int bfNg;
extern std::string compressMethod;
extern unsigned int dataPartNumber;
