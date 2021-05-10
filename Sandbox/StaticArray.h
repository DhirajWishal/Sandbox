#pragma once
#include <initializer_list>
#include <memory>

/**
 * Static array object.
 * This object can be used to store data in a static array.
 */
template<class Type, size_t ArrayCount>
class StaticArray {
public:
	/**
	 * Default consructor.
	 */
	StaticArray() {}

	/**
	 * Construct the static array using an initializer list.
	 */
	StaticArray(std::initializer_list<Type> list)
	{
		// Check if the list size is compatible.
		if ((list.size() > ArrayCount) || !list.size())
			return;

		// Copy the data from the list to the static array.
		std::memcpy(sArray, list.begin(), list.size() * typeSize());
	}

	/**
	 * Cronstruct the class using another array (copy).
	 * 
	 * @param other: The other static array to copy data from.
	 */
	StaticArray(const StaticArray<Type, ArrayCount>& other)
	{
		std::memcpy(sArray, other.sArray, ArrayCount * typeSize());
	}

	/**
	 * Cronstruct the class using another array (move).
	 *
	 * @param other: The other static array to copy data from.
	 */
	StaticArray(StaticArray<Type, ArrayCount>&& other)
	{
		std::memmove(sArray, other.sArray, ArrayCount * typeSize());
		std::memset(other.sArray, Type(), ArrayCount * typeSize());
	}

	/**
	 * Default destructor.
	 */
	~StaticArray() {}

	/**
	 * Get the size of the array.
	 */
	constexpr size_t size() const { return ArrayCount; }

	/**
	 * Get the size of the Type in bytes.
	 */
	constexpr size_t typeSize() const { return sizeof(Type); }

	/**
	 * Access a given element using the index.
	 *
	 * @param index: The index of the element.
	 */
	Type& at(long long index)
	{
		// Process the index.
		if (index < 0)
			index = static_cast<long long>(ArrayCount) + index;

		return sArray[index];
	}

	/**
	 * Access a given element using the index.
	 *
	 * @param index: The index of the element.
	 */
	const Type at(long long index) const
	{
		// Process the index.
		if (index < 0)
			index = static_cast<long long>(ArrayCount) + index;

		return sArray[index];
	}

public:
	/**
	 * Index operator.
	 *
	 * @param index: The index to be accessed.
	 */
	Type& operator[](long long index)
	{
		return at(index);
	}

	/**
	 * Index operator.
	 *
	 * @param index: The index to be accessed.
	 */
	const Type operator[](long long index) const
	{
		return at(index);
	}

	/**
	 * Is equal to operator.
	 *
	 * @param other: The other static array.
	 */
	bool operator==(const StaticArray<Type, ArrayCount>& other)
	{
		// Iterate through the array and check if the elements are equal.
		for (size_t itr = 0; itr < ArrayCount; itr++)
			if (sArray[itr] != other[itr])
				return false;

		return true;
	}

	/**
	 * Assignment operator (copy).
	 * 
	 * @param other: The other static array.
	 */
	StaticArray<Type, ArrayCount>& operator=(const StaticArray<Type, ArrayCount>& other)
	{
		std::memcpy(sArray, other.sArray, ArrayCount * typeSize());
	}

	/**
	 * Assignment operator (move).
	 *
	 * @param other: The other static array.
	 */
	StaticArray<Type, ArrayCount>& operator=(StaticArray<Type, ArrayCount>&& other)
	{
		std::memmove(sArray, other.sArray, ArrayCount * typeSize());
		std::memset(other.sArray, Type(), ArrayCount * typeSize());
	}

private:
	Type sArray[ArrayCount] = { Type() };	// Static array store.
};
