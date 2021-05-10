#pragma once
#include <memory>
#include <type_traits>
#include <initializer_list>

/**
 * Array allocator for the dynamic array.
 *
 * @tparam Type: The type of the alocation.
 * @tparam Alignment: The alignment of the allocation.
 */
template<class Type, size_t Alignment>
class ArrayAllocator {
public:
	/**
	 * Default constructor.
	 */
	ArrayAllocator() {}

	/**
	 * Default destructor.
	 */
	~ArrayAllocator() {}

	/**
	 * Create a new memory block.
	 * This also aligns the block to the corrent alignment
	 *
	 * @param byteSize: The size of the block in bytes.
	 */
	static constexpr Type* CreateNewBlock(size_t byteSize)
	{
		return static_cast<Type*>(operator new[](byteSize, std::align_val_t{ Alignment }));
	}

	/**
	 * Destroy an allocated block of memory.
	 * This automatically calls the destructor of all the stored elements.
	 *
	 * @param pBlock: The block to be deallocated.
	 */
	static constexpr void DestroyBlock(Type* pBlock)
	{
		operator delete[](static_cast<void*>(pBlock), std::align_val_t{ Alignment });
	}
};

/**
 * A simple dynamic array.
 *
 * @tparam Type: The type of the data stored in the array.
 * @tparam SizeBias: The number of slots to be allocated at a single instance.
 * @tparam Allocator: A custom allocator based on the ArrayAllocator class to allocate memory.
 */
template<class Type, size_t SizeBias = 1, class Allocator = ArrayAllocator<Type, sizeof(Type)>>
class Array {
	// Check if the input template arguments are valid.
	static_assert(!std::is_same<Type, Allocator>::value, "Invalid template arguments! Template parameters are Array<Type, SizeBias, Allocator>.");

public:
	/**
	 * Default constructor.
	 */
	Array() : pBegin(nullptr), pNext(nullptr), pEnd(nullptr) {}

	/**
	 * Construct the array using another array (copy).
	 *
	 * @param other: The other array.
	 */
	Array(const Array<Type, SizeBias, Allocator>& other)
		: pBegin(nullptr), pNext(nullptr), pEnd(nullptr)
	{
		// Check if the other array has data in it.
		if (!other.size())
			return;

		// Create a new memory block.
		pBegin = Allocator::CreateNewBlock(typeSize() * other.size());

		// Copy the other array's memory.
		copyBlock(pBegin, other.pBegin, other.pNext);

		// Initialize the pointers.
		pNext = pBegin + other.size();
		pEnd = pBegin + other.size();
	}

	/**
	 * Construct the array using another array (move).
	 *
	 * @param other: The other array.
	 */
	Array(Array<Type, SizeBias, Allocator>&& other)
		: pBegin(nullptr), pNext(nullptr), pEnd(nullptr)
	{
		// Check if the other array has data in it.
		if (!other.size())
			return;

		// Copy contents of the other array.
		pBegin = other.pBegin;
		pNext = other.pNext;
		pEnd = other.pEnd;

		// Reset the contents of the other array.
		other.pBegin = nullptr;
		other.pNext = nullptr;
		other.pEnd = nullptr;
	}

	/**
	 * Construct the array by initializing the size of it.
	 * This automatically sets the element values to its default.
	 *
	 * @param size: The number of elements to be stored.
	 * @param value: The value to initialize the elements with. Default is Type().
	 */
	Array(size_t size, const Type& value = Type())
		: pBegin(nullptr), pNext(nullptr), pEnd(nullptr)
	{
		// Check if the size is valid.
		if (!size)
			return;

		// Create a new memory block.
		pBegin = Allocator::CreateNewBlock(typeSize() * size);

		// Initialize the block with values.
		initializeBlock(pBegin, value, size);

		// Initialize the pointers.
		pNext = pBegin + size;
		pEnd = pBegin + size;
	}

	/**
	 * Construct the array using an initializer list.
	 *
	 * @param list: The initializer list.
	 */
	Array(std::initializer_list<Type> list)
		: pBegin(nullptr), pNext(nullptr), pEnd(nullptr)
	{
		// Check if the initializer list contains data.
		if (!list.size())
			return;

		// Create a new memory block.
		pBegin = Allocator::CreateNewBlock(typeSize() * list.size());

		// Copy the initializer list.
		copyBlock(pBegin, const_cast<Type*>(list.begin()), const_cast<Type*>(list.end()));

		// Initialize the pointers.
		pNext = pBegin + list.size();
		pEnd = pBegin + list.size();
	}

	/**
	 * Default constructor.
	 */
	~Array() { clear(); }

public:
	/**
	 * Iteratror of the Dynamic Array.
	 */
	class Iterator {
	public:
		/**
		 * Default constructor.
		 *
		 * @param pBegin: The begin pointer of the memory block.
		 */
		Iterator(const Type* pBegin = nullptr) : pBegin(const_cast<Type*>(pBegin)) {}

		/**
		 * Default destructor.
		 */
		~Iterator() {}

		/**
		 * Assignment operator.
		 *
		 * @param pointer: The pointer to be set.
		 */
		Iterator& operator=(const Type* pointer)
		{
			this->pBegin = const_cast<Type*>(pointer);
			return *this;
		}

		/**
		 * Increment operator.
		 * This increments the pointer by one.
		 */
		Type* operator++(int)
		{
			pBegin++;
			return pBegin;
		}

		/**
		 * Return a the poitner after adding an offset.
		 * This does not modify the internal pointer.
		 *
		 * @param offset: The offset to be added.
		 */
		Type* operator+(size_t offset) const
		{
			return pBegin + offset;
		}

		/**
		 * Increment the pointer by a given value.
		 *
		 * @param offset: The valur to be added.
		 */
		Type* operator+=(size_t offset)
		{
			pBegin += offset;
			return pBegin;
		}

		/**
		 * Decrement operator.
		 * This decrements the pointer by one.
		 */
		Type* operator--(int)
		{
			pBegin--;
			return pBegin;
		}

		/**
		 * Return a the poitner after subtracting an offset.
		 * This does not modify the internal pointer.
		 *
		 * @param offset: The offset to be subtracted.
		 */
		Type* operator-(size_t offset) const
		{
			return pBegin - offset;
		}

		/**
		 * Decrement the pointer by a given value.
		 *
		 * @param offset: The valur to be subtracted.
		 */
		Type* operator-=(size_t offset)
		{
			pBegin -= offset;
			return pBegin;
		}

		/**
		 * Is not equal operator.
		 *
		 * @param pointer: The pointer to be checked with.
		 */
		bool operator!=(const Type* pointer) const
		{
			return pBegin != pointer;
		}

		/**
		 * Is equal operator.
		 *
		 * @param pointer: The pointer to be checked with.
		 */
		bool operator==(const Type* pointer) const
		{
			return pBegin == pointer;
		}

		/**
		 * Is less than operator.
		 *
		 * @param pointer: The pointer to be checked with.
		 */
		bool operator<(const Type* pointer) const
		{
			return pBegin < pointer;
		}

		/**
		 * Is less than or equal operator.
		 *
		 * @param pointer: The pointer to be checked with.
		 */
		bool operator<=(const Type* pointer) const
		{
			return pBegin <= pointer;
		}

		/**
		 * Is grater than operator.
		 *
		 * @param pointer: The pointer to be checked with.
		 */
		bool operator>(const Type* pointer) const
		{
			return pBegin > pointer;
		}

		/**
		 * Is grater than or equal operator.
		 *
		 * @param pointer: The pointer to be checked with.
		 */
		bool operator>=(const Type* pointer) const
		{
			return pBegin >= pointer;
		}

		/**
		 * Get the value pointed by the iterator.
		 */
		const Type operator*() const
		{
			return *this->pBegin;
		}

	private:
		Type* pBegin = nullptr;		// The array memory pointer.
	};

	/**
	 * Constatnt iteratror of the Dynamic Array.
	 */
	class ConstIterator {
	public:
		/**
		 * Default constructor.
		 *
		 * @param pBegin: The begin pointer of the memory block.
		 */
		ConstIterator(const Type* pBegin = nullptr) : pBegin(const_cast<Type*>(pBegin)) {}

		/**
		 * Default destructor.
		 */
		~ConstIterator() {}

		/**
		 * Assignment operator.
		 *
		 * @param pointer: The pointer to be set.
		 */
		ConstIterator& operator=(const Type* pointer)
		{
			this->pBegin = const_cast<Type*>(pointer);
			return *this;
		}

		/**
		 * Increment operator.
		 * This increments the pointer by one.
		 */
		Type* operator++(int)
		{
			pBegin++;
			return pBegin;
		}

		/**
		 * Increment the pointer by a given value.
		 *
		 * @param offset: The valur to be added.
		 */
		Type* operator+=(size_t offset)
		{
			pBegin += offset;
			return pBegin;
		}

		/**
		 * Decrement operator.
		 * This decrements the pointer by one.
		 */
		Type* operator--(int)
		{
			pBegin--;
			return pBegin;
		}

		/**
		 * Return a the poitner after subtracting an offset.
		 * This does not modify the internal pointer.
		 *
		 * @param offset: The offset to be subtracted.
		 */
		Type* operator-(size_t offset) const
		{
			return pBegin - offset;
		}

		/**
		 * Decrement the pointer by a given value.
		 *
		 * @param offset: The valur to be subtracted.
		 */
		Type* operator-=(size_t offset)
		{
			pBegin -= offset;
			return pBegin;
		}

		/**
		 * Is not equal operator.
		 *
		 * @param pointer: The pointer to be checked with.
		 */
		bool operator!=(const Type* pointer) const
		{
			return pBegin != pointer;
		}

		/**
		 * Is equal operator.
		 *
		 * @param pointer: The pointer to be checked with.
		 */
		bool operator==(const Type* pointer) const
		{
			return pBegin == pointer;
		}

		/**
		 * Is less than operator.
		 *
		 * @param pointer: The pointer to be checked with.
		 */
		bool operator<(const Type* pointer) const
		{
			return pBegin < pointer;
		}

		/**
		 * Is less than or equal operator.
		 *
		 * @param pointer: The pointer to be checked with.
		 */
		bool operator<=(const Type* pointer) const
		{
			return pBegin <= pointer;
		}

		/**
		 * Is grater than operator.
		 *
		 * @param pointer: The pointer to be checked with.
		 */
		bool operator>(const Type* pointer) const
		{
			return pBegin > pointer;
		}

		/**
		 * Is grater than or equal operator.
		 *
		 * @param pointer: The pointer to be checked with.
		 */
		bool operator>=(const Type* pointer) const
		{
			return pBegin >= pointer;
		}

		/**
		 * Get the value pointed by the iterator.
		 */
		const Type operator*() const
		{
			return *this->pBegin;
		}

	private:
		Type* pBegin = nullptr;		// The array memory pointer.
	};

	/**
	 * Reverse iteratror of the Dynamic Array.
	 * This iterates through the array in reverse.
	 */
	class ReverseIterator {
	public:
		/**
		 * Default constructor.
		 *
		 * @param pBegin: The end pointer of the memory block.
		 */
		ReverseIterator(const Type* pEnd = nullptr) : pEnd(const_cast<Type*>(pEnd - 1)) {}

		/**
		 * Default destructor.
		 */
		~ReverseIterator() {}

		/**
		 * Assignment operator.
		 *
		 * @param pointer: The pointer to be set.
		 */
		ReverseIterator& operator=(const Type* pointer)
		{
			this->pEnd = const_cast<Type*>(pointer - 1);
			return *this;
		}

		/**
		 * Increment operator.
		 * This increments the pointer by one.
		 */
		Type* operator++(int)
		{
			pEnd--;
			return pEnd;
		}

		/**
		 * Increment the pointer by a given value.
		 *
		 * @param offset: The valur to be added.
		 */
		Type* operator+=(size_t offset)
		{
			pEnd += offset;
			return pEnd;
		}

		/**
		 * Return a the poitner after subtracting an offset.
		 * This does not modify the internal pointer.
		 *
		 * @param offset: The offset to be subtracted.
		 */
		Type* operator-(size_t offset) const
		{
			return pEnd - offset;
		}

		/**
		 * Decrement the pointer by a given value.
		 *
		 * @param offset: The valur to be subtracted.
		 */
		Type* operator-=(size_t offset)
		{
			pEnd -= offset;
			return pEnd;
		}

		/**
		 * Is not equal operator.
		 *
		 * @param pointer: The pointer to be checked with.
		 */
		bool operator!=(const Type* pointer) const
		{
			return (pEnd + 1) != pointer;
		}

		/**
		 * Is equal operator.
		 *
		 * @param pointer: The pointer to be checked with.
		 */
		bool operator==(const Type* pointer) const
		{
			return (pEnd + 1) == pointer;
		}

		/**
		 * Is less than operator.
		 *
		 * @param pointer: The pointer to be checked with.
		 */
		bool operator<(const Type* pointer) const
		{
			return (pEnd + 1) < pointer;
		}

		/**
		 * Is less than or equal operator.
		 *
		 * @param pointer: The pointer to be checked with.
		 */
		bool operator<=(const Type* pointer) const
		{
			return (pEnd + 1) <= pointer;
		}

		/**
		 * Is grater than operator.
		 *
		 * @param pointer: The pointer to be checked with.
		 */
		bool operator>(const Type* pointer) const
		{
			return (pEnd + 1) > pointer;
		}

		/**
		 * Is grater than or equal operator.
		 *
		 * @param pointer: The pointer to be checked with.
		 */
		bool operator>=(const Type* pointer) const
		{
			return (pEnd + 1) >= pointer;
		}

		/**
		 * Get the value pointed by the iterator.
		 */
		const Type operator*() const
		{
			return *this->pEnd;
		}

	private:
		Type* pEnd = nullptr;		// The array memory pointer.
	};

public:
	/**
	 * Extend the array by a certain size.
	 *
	 * @param size: The size to be extended (element count). Default is SizeBias.
	 */
	void extend(size_t size = SizeBias)
	{
		// Create a new block to fit the required size.
		size_t newCapacity = getNewCapacity(size);

		// Allocate a new block.
		Type* pBlock = Allocator::CreateNewBlock(typeSize() * newCapacity);

		// Move existing data to the new block.
		moveBlock(pBlock, pBegin, pNext);

		// Clear the existing data.
		clear();

		// Assign pointers.
		pBegin = pBlock;
		pNext = pBegin + (newCapacity - size);
		pEnd = pBegin + newCapacity;
	}

	/**
	 * Add an elements to the end of the array (copy).
	 *
	 * @param value: The value to be added.
	 */
	void pushBack(const Type& value)
	{
		// Check if the allocated block is full. 
		if (pNext == pEnd)
			extend();

		// Assign data to the array.
		*pNext = value;
		pNext++;
	}

	/**
	 * Add an elements to the end of the array (move).
	 *
	 * @param value: The value to be added.
	 */
	void pushBack(Type&& value)
	{
		pushBack(static_cast<const Type&>(value));
	}

	/**
	 * Pop the last element after returning the value of it.
	 */
	Type popBack()
	{
		return *(pNext--);
	}

	/**
	 * Push an element to the front of the array (copy).
	 *
	 * @param value: The value to be added.
	 */
	void pushFront(const Type& value)
	{
		// Check if the allocated block is full. 
		if (pNext == pEnd)
			extend();

		// Move the array one block to the back to override the first element.
		moveBlock(pBegin + 1, pBegin, pNext);

		// Assign the value and increment the pointer.
		pBegin[0] = value;
		pNext++;
	}

	/**
	 * Push an element to the front of the array (move).
	 *
	 * @param value: The value to be added.
	 */
	void pushFront(Type&& value)
	{
		pushFront(value);
	}

	/**
	 * Pop the first element after returning the value of it.
	 */
	Type popFront()
	{
		// Get the first value before overriding it.
		Type retValue = pBegin[0];

		// Move the whole array one block back.
		moveBlock(pBegin, pBegin + 1, pNext);

		// Decrement the pointer. 
		pNext--;

		return retValue;
	}

	/**
	 * Clear all the data in the array.
	 * This resets all the data in this to its default.
	 */
	void clear()
	{
		// Terminate the block if already allocated.
		if (pBegin)
			Allocator::DestroyBlock(pBegin);

		// Reset pointers.
		pBegin = nullptr;
		pNext = nullptr;
		pEnd = nullptr;
	}

	/**
	 * Get an element at a given index.
	 * This method accepts negative indexes.
	 *
	 * @param index: The index to be accessed.
	 */
	Type& at(long long index)
	{
		// Process the index.
		if (index < 0)
			index = size() + index;

		// Return the element.
		return pBegin[index];
	}

	/**
	 * Get an element at a given index.
	 * This method accepts negative indexes.
	 *
	 * @param index: The index to be accessed.
	 */
	const Type at(long long index) const
	{
		// Process the index.
		if (index < 0)
			index = size() + index;

		// Return the element
		return pBegin[index];
	}

	/**
	 * Remove an element from the array using its index.
	 *
	 * @param index: The index of the element to be removed.
	 */
	void remove(long long index)
	{
		// Process the index.
		if (index < 0)
			index = size() + index;

		// Call the destructor of the element which is being removed.
		at(index).~Type();

		// The removal is done by copying the existing elements of memory proceeding the required element, and 
		// moving it back by one block. This way we wont be needing to allocate a new block and copy the data in
		// to seperate instructions.
		moveBlock(pBegin + index, pBegin + index + 1, pNext);

		// Decrement the pointer to point to the closest free block.
		pNext--;
	}

	/**
	 * Resize the array to a variable length.
	 * This method will clear the previously stored data.
	 *
	 * @param size: The number of elements to be stored.
	 * @param value: The value to be initialized with. Default is Type().
	 */
	void resize(size_t size, const Type& value = Type())
	{
		// Check if the size is valid.
		if (!size)
			return;

		// Clear the old allocation. 
		clear();

		// Create a new memory block.
		pBegin = Allocator::CreateNewBlock(typeSize() * size);

		// Initialize the block with values.
		initializeBlock(pBegin, value, size);

		// Initialize the pointers.
		pNext = pBegin + size;
		pEnd = pBegin + size;
	}

	/**
	 * Find a certain value is present in the array.
	 * If the array does not contain any data or if the value is not found, -1 is returned.
	 */
	size_t find(const Type& value) const
	{
		// Iterate through the array to find the required value.
		for (size_t index = 0; index < size(); index++)
			if (pBegin[index] == value)
				return index;

		// Return if not found.
		return -1;
	}

	/**
	 * Return the memory location of a given index.
	 *
	 * @param index: The index of the element.
	 */
	const Type* location(long long index) const
	{
		// Process the index.
		if (index < 0)
			index = size() + index;

		// Return the casted pointer.
		return const_cast<Type*>(&pBegin[index]);
	}

	/**
	 * Return the number of occurences a specific value has in the array.
	 *
	 * @param value: The value to be checked.
	 */
	size_t occurence(const Type& value) const
	{
		size_t occur = 0;

		// Iterate throught the array to find the occurances.
		Type* itr = begin();
		while (itr != end())
			if (*itr == value)
				occur++;

		return occur;
	}

public:
	/**
	 * Get the type size in bytes.
	 */
	constexpr size_t typeSize() const noexcept { return sizeof(Type); }

	/**
	 * Get the number of elements stored in the array.
	 */
	const size_t size() const noexcept { return pNext - pBegin; }

	/**
	 * Get the number of elements which can be stored in the array.
	 */
	const size_t capacity() const noexcept { return pEnd - pBegin; }

	/**
	 * Get the maximum allocatable capacity of the array.
	 */
	constexpr size_t maximumCapacity() const { return -1; }

	/**
	 * Begin iterator of the array.
	 */
	const Type* begin() const noexcept { return pBegin; }

	/**
	 * End iterator of the array.
	 */
	const Type* end() const noexcept { return pNext; }

	/**
	 * Get the front element of the array.
	 */
	Type& front() { return pBegin[0]; }

	/**
	 * Get the front element of the array.
	 */
	const Type front() const { return pBegin[0]; }

	/**
	 * Get the back element of the array.
	 */
	Type& back() { return *(pNext - 1); }

	/**
	 * Get the back element of the array.
	 */
	const Type back() const { return *(pNext - 1); }

	/**
	 * Check if an index is valid.
	 */
	bool isValidIndex(long long index) const { return index < 0 ? ((size() + index) < size()) : (index < size()); }

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
	 * Assignment operator (copy).
	 *
	 * @param other: The other array.
	 */
	Array<Type, SizeBias, Allocator>& operator=(const Array<Type, SizeBias, Allocator>& other)
	{
		// Check if the other array has data in it.
		if (!other.size())
			return;

		// Clear the array if already allocated.
		clear();

		// Create a new memory block.
		pBegin = Allocator::CreateNewBlock(typeSize() * other.size());

		// Copy the other array's memory.
		copyBlock(pBegin, other.pBegin, other.pNext);

		// Initialize the pointers.
		pNext = pBegin + other.size();
		pEnd = pBegin + other.size();

		return *this;
	}

	/**
	 * Assignment operator (move).
	 *
	 * @param other: The other array.
	 */
	Array<Type, SizeBias, Allocator>& operator=(Array<Type, SizeBias, Allocator>&& other)
	{
		// Check if the other array has data in it.
		if (!other.size())
			return;

		// Clear the array if already allocated.
		clear();

		// Copy contents of the other array.
		pBegin = other.pBegin;
		pNext = other.pNext;
		pEnd = other.pEnd;

		// Reset the contents of the other array.
		other.pBegin = nullptr;
		other.pNext = nullptr;
		other.pEnd = nullptr;

		return *this;
	}

	/**
	 * Assignment operator.
	 *
	 * @param list: The initializer list.
	 */
	Array<Type, SizeBias, Allocator>& operator=(std::initializer_list<Type> list)
	{
		// Check if the initializer list contains data.
		if (!list.size())
			return;

		// Clear the array if already allocated.
		clear();

		// Create a new memory block.
		pBegin = Allocator::CreateNewBlock(typeSize() * list.size());

		// Copy the initializer list.
		copyBlock(pBegin, const_cast<Type*>(list.begin()), const_cast<Type*>(list.end()));

		// Initialize the pointers.
		pNext = pBegin + list.size();
		pEnd = pBegin + list.size();

		return *this;
	}

	/**
	 * Is equal operator.
	 *
	 * @param other: The other array.
	 */
	bool operator==(const Array<Type, SizeBias, Allocator>& other)
	{
		// Check if the sizes match. 
		if (other.size() != size())
			return false;

		// Check if the elements match. 
		for (size_t index = 0; index < size(); index++)
			if (at(index) != other[index])
				return false;

		return true;
	}

private:
	/**
	 * Copy a block of data from one address to another using the pointer range of the source.
	 *
	 * @param pDst: The destination address.
	 * @param pSrcBegin: The begin address of the memory block.
	 * @param pSrcEnd: The end address of the memory block.
	 */
	void copyBlock(Type* pDst, Type* pSrcBegin, Type* pSrcEnd)
	{
		std::memcpy(pDst, pSrcBegin, (pSrcEnd - pSrcBegin) * typeSize());
	}

	/**
	 * Move a block of data from one address to another using the pointer range of the source.
	 *
	 * @param pDst: The destination address.
	 * @param pSrcBegin: The begin address of the memory block.
	 * @param pSrcEnd: The end address of the memory block.
	 */
	void moveBlock(Type* pDst, Type* pSrcBegin, Type* pSrcEnd)
	{
		std::memmove(pDst, pSrcBegin, (pSrcEnd - pSrcBegin) * typeSize());
	}

	/**
	 * Initialize a block of memory with a value.
	 *
	 * @param pDst: The destination address.
	 * @param value: The value to be initialized with.
	 * @param count: The number of elements to be initialized.
	 */
	void initializeBlock(Type* pDst, const Type& value, size_t count)
	{
		while (count--) *pDst = value, pDst++;
	}

	/**
	 * Calculate the new capacity to be allocated.
	 *
	 * @param newElementCount: The number of new elements.
	 */
	size_t getNewCapacity(size_t newElementCount)
	{
		return capacity() + newElementCount; // TODO
	}

private:
	Type* pBegin = nullptr;		// Begin pointer of the array.
	Type* pNext = nullptr;		// Next pointer of the array.
	Type* pEnd = nullptr;		// End pointer of the array.
};
