#include "BloomFilter.h"
#include "MurmurHash3.h"
#include <array>

std::array<std::uint64_t, 2> hash(const std::uint8_t *data, std::size_t len) {
	std::array<std::uint64_t, 2> hashValue;
	MurmurHash3_x64_128(data, len, 0, hashValue.data());
	return hashValue;
}

inline std::uint64_t nthHash(std::uint8_t n,
	std::uint64_t hashA,
	std::uint64_t hashB,
	std::uint64_t filterSize) {
	return (hashA + n * hashB) % filterSize;
}

BloomFilter::BloomFilter(float p, std::uint64_t n) {
	m_bits = std::vector<bool>(-(n * log(p)) / (log(2) * log(2)), false);
	m_numHashes = m_bits.size() / n * log(2);
}

void BloomFilter::add(const std::uint8_t *data, std::size_t len) {
	auto hashValues = hash(data, len);

	for (int n = 0; n < m_numHashes; n++) {
		m_bits[nthHash(n, hashValues[0], hashValues[1], m_bits.size())] = true;
	}
}

bool BloomFilter::possiblyContains(const uint8_t *data, std::size_t len) const {
	auto hashValues = hash(data, len);

	for (int n = 0; n < m_numHashes; n++) {
		if (!m_bits[nthHash(n, hashValues[0], hashValues[1], m_bits.size())]) {
			return false;
		}
	}

	return true;
}