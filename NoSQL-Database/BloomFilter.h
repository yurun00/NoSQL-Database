#pragma once
#include "Common.h"
#include <cstdint>

/*	false positive error rate:	p
	size of the bloom filter:	m
	number of hash functions:	k
	number of elements inserted:n

	m = -(n*ln(p)) / (ln(2))^2
	k = m / n * ln(2)

	double hashing:
	hash_i(x,m) = (hash_a(x) + i * hash_b(x)) mod m
 */
class BloomFilter {
	std::vector<bool> m_bits;
	std::uint8_t m_numHashes;
public:
	BloomFilter() : m_bits(), m_numHashes(0) {}
	BloomFilter(float p, std::uint64_t n);
	void add(const std::uint8_t *data, std::size_t len);
	bool possiblyContains(const std::uint8_t *data, std::size_t len) const;
};

class BloomFilterManager {
	friend class Controller;
	// map from version number to sstable file
	std::map<std::string, BloomFilter> vToBfs;
};

