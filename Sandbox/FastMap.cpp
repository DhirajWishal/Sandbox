#include "FastMap.h"

#include <immintrin.h>

#define CHUNK_SIZE  4
#define CHUNK_ALIGN	__declspec(align(32))

long long FindIndex(std::vector<uint64_t>& hashVector, const uint64_t& hash)
{
	uint64_t count = hashVector.size();
	uint64_t mod = count % CHUNK_SIZE;

	if (mod)
		hashVector.insert(hashVector.end(), CHUNK_SIZE - mod, 0), count += CHUNK_SIZE - mod;

	auto dataPtr = hashVector.begin()._Ptr;
	int counter = count / CHUNK_SIZE;

	CHUNK_ALIGN unsigned __int64 hashBlock[CHUNK_SIZE] = { hash, hash, hash, hash };
	//CHUNK_ALIGN unsigned __int64 hashBlock[CHUNK_SIZE];
	//std::memset(hashBlock, hash, CHUNK_SIZE * sizeof(uint64_t));

	size_t pos = 0;
	while (counter--)
	{
		CHUNK_ALIGN unsigned __int64 block[CHUNK_SIZE] = { *(dataPtr + pos + 0), *(dataPtr + pos + 1), *(dataPtr + pos + 2), *(dataPtr + pos + 3) };
		__m256i cmp = _mm256_cmpeq_epi64(*reinterpret_cast<__m256i*>(hashBlock), *reinterpret_cast<__m256i*>(block));

		if (cmp.m256i_u64[0])
			return pos + 0;

		if (cmp.m256i_u64[1])
			return pos + 1;

		if (cmp.m256i_u64[2])
			return pos + 2;

		if (cmp.m256i_u64[3])
			return pos + 3;

		pos += CHUNK_SIZE;
	}

	return -1;
}
