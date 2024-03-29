#include "Globals.h"

const unsigned int PAGE_SIZE = 512;
const unsigned int MAX_MEMTABLE_SIZE = 2000;
const unsigned int MAX_MEMTABLE_VERSION = 3;
const unsigned int MAX_SSTABLE_SIZE = 2000;
const unsigned int MAX_CACHE_SIZE = 2000;
const float BLOOM_FILTER_FALSE_POSITIVE_RATE = 0.1;
const std::string TOMBSTONE = "!tombstone";
unsigned int bfFp = 0;
unsigned int bfNg = 0;
std::string compressMethod = "u";
unsigned int dataPartNumber = 0;