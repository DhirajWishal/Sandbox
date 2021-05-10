#pragma once

#include <vector>
#include <xxhash.h>
#include <algorithm>

using UI64 = uint64_t;

template<class Key, class Value, class Compare = std::less<UI64>>
class BinaryHashMap : private Compare {

	struct Container {
		Container() = default;
		Container(const Key& key, const Value& value, const UI64& hash)
			: mKey(key), mValue(value), mHash(hash) {}

		Key mKey = Key();
		Value mValue = Value();
		UI64 mHash = 0;
	};

	class KeyCompare {
	public:
		KeyCompare() = default;

		constexpr bool operator()(const Container& left, const Container& right) const
		{
			return mCompare(left.mHash, right.mHash);
		}

		constexpr bool operator()(const Container& left, const UI64& hash) const
		{
			return mCompare(left.mHash, hash);
		}

	private:
		const Compare& mCompare = {};
	};

public:
	BinaryHashMap() {}
	~BinaryHashMap() {}

	void Insert(const Key& key, const Value& data)
	{
		Get(key) = data;
	}

	Value& Get(const Key& key)
	{
		UI64 hash = XXH64(&key, sizeof(Key), 0);
		auto pContainer = FindContainer(hash, mEntries.size());

		if (pContainer && pContainer->mHash == hash)
			return pContainer->mValue;

		mEntries.insert(mEntries.end(), Container(key, Value(), hash));
		std::sort(mEntries.begin(), mEntries.end(), KeyCompare());

		return Get(key);
	}

	Container* FindContainer(UI64 hash, UI64 size)
	{
		Container* pBegin = mEntries.data();
		Container* pData = nullptr;
		UI64 counter = 0;

		while (size)
		{
			counter = size / 2;
			pData = pBegin + counter;

			if (Compare::operator()(pData->mHash, hash))
			{
				pBegin = ++pData;
				size -= counter + 1;
			}
			else
				size = counter;
		}

		return pBegin;
	}

private:
	std::vector<Container> mEntries;
};