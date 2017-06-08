#include "Globals.h"

const unsigned int PAGE_SIZE = 4;
const unsigned int MAX_MEMTABLE_SIZE = 16;
const unsigned int MAX_MEMTABLE_VERSION = 3;
const float BLOOM_FILTER_FALSE_POSITIVE_RATE = 0.1;
unsigned int bfFp = 0;;
unsigned int bfNg = 0;
std::string compressMethod = "u";
unsigned int dataPartNumber = 0;