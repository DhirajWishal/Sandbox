#pragma once

#include <vector>
#include <xxhash.h>
#include <algorithm>

template<class Key, class Value, class HashCompare = std::less<size_t>>
class HashMap : private HashCompare {
	struct Entry {
		Entry(Key key = Key(), Value value = Value(), size_t hash = 0)
			: key(key), value(value), hash(hash) {}

		Key key = Key();
		Value value = Value();
		size_t hash = 0;
	};

	typedef std::vector<Entry> EntryVector;

	class FirstLess
	{
	public:
		FirstLess(const HashCompare& comp) : mCompare(comp) {}

		bool operator()(const Entry& left, const Entry& right) const
		{
			return mCompare(left.hash, right.hash);
		}

	private:
		const HashCompare& mCompare;
	};

public:
	HashMap() = default;
	~HashMap() {}

	void Insert(const Key& key, const Value& value)
	{
		uint64_t hash = XXH64(&key, sizeof(Key), 0);
		uint32_t block = hash >> 32;	// We take the first 32 bits (0x|00000000|00000000) as the block index.
		uint32_t maxBlock = -1;
	}

	Value Get(const Key& key)
	{
		return Value();
	}

private:

};
