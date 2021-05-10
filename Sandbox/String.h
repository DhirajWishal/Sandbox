#pragma once
#include <string>

#ifdef USE_WCHAR
	#define TEXT(text)	L##text

#else
	#define TEXT(text)	text

#endif // USE_WCHAR

/**
 * A simple string class with variable primitive type.
 * Using this definition makes it easier for projects which use just one type of string (char or wchar_t).
 *
 * This object is not intended to be used as a direct substitute of the std::string/ std::wstring. This is a small implementation
 * targeted for educational purposes.
 */
class String {
public:
#ifdef USE_WCHAR
	// Primitive type.
	using Type = wchar_t;

#else 
	// Primitive type.
	using Type = char;

#endif // USE_WCHAR

	// String standard type.
	using TypeSTD = std::basic_string<Type, std::char_traits<Type>, std::allocator<Type>>;

public:
	/**
	 * Default constructor.
	 */
	String() noexcept : string(nullptr), length(0) {}

	/**
	 * Construct the string by allocating the size and keeping it.
	 *
	 * @param length: The length to be allocated to.
	 */
	String(size_t length) : string(nullptr), length(length)
	{
		string = CreateNewBlock(this->length + 1);
	}

	/**
	 * Construct the string object using a primitive string.
	 *
	 * @param pString: The primitive string.
	 */
	String(const Type* pString) : string(nullptr), length(0)
	{
		// Set the length of the string.
		length = GetStrLength(pString);

		// Allocate a new block in memory.
		string = CreateNewBlock(length + 1);

		// Copy the primitive string to the newly allocated memory block.
		std::memcpy(string, pString, length * typeSize());
	}

	/**
	 * The copy constructor.
	 *
	 * @param str: The other string.
	 */
	String(const String& str) : string(nullptr), length(str.length)
	{
		// Return if the other string is empty.
		if (!str.length || !str.string)
			return;

		// Allocate a new block in memory.
		string = CreateNewBlock(length + 1);

		// Copy the primitive string to the newly allocated memory block.
		std::memcpy(string, str.string, length * typeSize());
	}

	/**
	 * The move constructor.
	 *
	 * @param str: The other string.
	 */
	String(String&& str) noexcept : string(str.string), length(str.length)
	{
		str.string = nullptr;
		str.length = 0;
	}

	/**
	 * Construct this object using the standard template library string.
	 *
	 * @param str: The standard template string.
	 */
	String(const TypeSTD&& str) : string(nullptr), length(str.length())
	{
		// Return if the other string is empty.
		if (!str.size())
			return;

		// Allocate a new block in memory.
		string = CreateNewBlock(length + 1);

		// Copy the primitive string to the newly allocated memory block.
		std::memcpy(string, str.data(), length * typeSize());
	}

	/**
	 * Default destructor.
	 */
	~String()
	{
		clear();
	}

	/**
	 * Get the string stored in the string by its base type.
	 *
	 * @return: const BaseType pointer value of the stored string.
	 */
	const Type* str() const { return static_cast<const Type*>(this->string); }

	/**
	 * Get the length of a primitive string.
	 *
	 * @param pString: The primitive string.
	 */
	__forceinline static size_t GetStrLength(const Type* pString)
	{
		// Check if the string is a nullptr and if true, return 0.
		if (!pString)
			return 0;

		size_t count = 0;
		Type* pBegining = const_cast<Type*>(pString);

		// Count the number of characters till '\0' character. This is the null termination character and represents
		// the end of the string.
		while (*pBegining != '\0') count++, pBegining++;

		return count;
	}

	/**
	 * Get the size of the primitive type in bytes.
	 */
	__forceinline size_t typeSize() const { return sizeof(Type); }

	/**
	 * Get the number of characteres stored in the object.
	 */
	__forceinline size_t size() const { return length; }

	/**
	 * Clear all the values of the string.
	 */
	void clear()
	{
		if (string)
			delete[] string;

		// Set all the data so default.
		string = nullptr;
		length = 0;
	}

	/**
	 * Convert this object's data to the standard template string object.
	 */
	TypeSTD toStandard() const
	{
		return TypeSTD(string);
	}

	/**
	 * Begin iterator method.
	 */
	Type* begin() const
	{
		return string;
	}

	/**
	 * The end iterator method.
	 */
	Type* end() const
	{
		return reinterpret_cast<Type*>(reinterpret_cast<size_t>(string) + (length * typeSize()));
	}

	/**
	 * Allocate and reserve the array to a perticular size/ length.
	 * If the object contains data, it will be overridden.
	 *
	 * @param length: The length to which the string is allocated.
	 */
	void allocate(size_t length)
	{
		// Clear existing data.
		clear();

		// Create the new block and assign values.
		this->length = length;
		string = CreateNewBlock(this->length + 1);
	}

	/**
	 * Append a character to the string.
	 *
	 * @param character: The character to be appended.
	 */
	void append(Type character)
	{
		length++;

		// Create a new memory block.
		Type* newBlock = CreateNewBlock(length + 1);

		// If the string has data, move them to the new block and delete the old allocation.
		if (string)
		{
			std::memmove(newBlock, string, (length - 1) * typeSize());
			delete[] string;
		}

		// Assign the new character and the new block to the string pointer.
		newBlock[length] = character;
		string = newBlock;
	}

	/**
	 * Access a character in a given index.
	 * 
	 * @param index: The index to be accessed.
	 */
	Type& at(long long index)
	{
		// Process negative indexes.
		if (index < 0)
			index = static_cast<long long>(length) + index;

		return string[index];
	}

	/**
	 * Access a character in a given index.
	 *
	 * @param index: The index to be accessed.
	 */
	const Type at(long long index) const
	{
		// Process negative indexes.
		if (index < 0)
			index = static_cast<long long>(length) + index;

		return string[index];
	}

	/**
	 * Create a sub string using the current string.
	 *
	 * The sub string is made using String[startIndex] and String[endIndex - 1] inclusive.
	 *
	 * @param startIndex: The start index to copy data from.
	 * @param endIndex: The end index to copy data.
	 */
	String subString(long long startIndex, long long endIndex)
	{
		// Process the start index.
		if (startIndex < 0)
			startIndex = length + startIndex;

		// Process the end index.
		if (endIndex < 0)
			endIndex = length + endIndex;

		// Create the new string with the requires size.
		String newBlock(endIndex - startIndex);

		// Copy data to it using the indexes.
		for (size_t index = static_cast<size_t>(startIndex); index < static_cast<size_t>(endIndex); index++)
			newBlock[index - startIndex] = string[index];

		return newBlock;
	}

	/**
	 * Find if a character is present in the string and return its index if found.
	 * The method returns -1 if the character is not found.
	 *
	 * @param character: The character to be searched.
	 */
	long long find(Type character)
	{
		// Iterate through the string and check if the character exists.
		for (size_t index = 0; index < length; index++)
			if (string[index] == character)
				return index;

		return -1;
	}

	/**
	 * Find if a string is present in this string and return the index of the first character if found.
	 * The method returns -1 if, the string is not found, if the length of the string is 0, or if the string
	 * to be checked is longer then the current string.
	 *
	 * @param str: The string to be searched for.
	 */
	long long find(String str)
	{
		// Check if the string is valid.
		if (!this->length || !str.length || (this->length < str.length))
			return -1;

		// Iterate through the string to find a match.
		for (size_t index = 0; index < length; index++)
		{
			if (string[index] == str[0])
			{
				long long candidate = index;

				// Iterate through to find if the characters match.
				for (size_t itr = index; itr < str.length; itr++)
				{
					if (string[itr] != str[itr - index])
					{
						itr = str.length;
						candidate = -1;
					}
				}

				// If the candidate is not -1, it means that we have found the right index.
				if (candidate != -1)
					return candidate;
			}
		}

		return -1;
	}

	/**
	 * Generate a hash using the current string.
	 */
	size_t hash()
	{
		size_t uHash = 0;

		for (size_t index = 0; index < length; index++)
		{
			size_t charNum = static_cast<size_t>(string[index]);

			uHash = uHash ^ (charNum * index);
		}

		return uHash;
	}

public:
	String& operator=(const Type* pString)
	{
		// Clear the existing data.
		clear();

		// Set the length of the string.
		length = GetStrLength(pString);

		// Allocate a new block in memory.
		string = CreateNewBlock(length + 1);

		// Copy the primitive string to the newly allocated memory block.
		std::memcpy(string, pString, length * typeSize());

		return *this;
	}

	/**
	 * Assignment operator (copy).
	 *
	 * @param str: The other string.
	 */
	String& operator=(const String& str)
	{
		// Return if the other string is empty.
		if (!str.length || !str.string)
			return*this;

		// Clear the existing data.
		clear();

		// Allocate a new block in memory.
		string = CreateNewBlock(length + 1);

		// Copy the primitive string to the newly allocated memory block.
		std::memcpy(string, str.string, length * typeSize());

		return *this;
	}

	/**
	 * Assignment operator (move).
	 *
	 * @param str: The other string.
	 */
	String& operator=(String&& str) noexcept
	{
		this->string = str.string;
		this->length = str.length;

		str.string = nullptr;
		str.length = 0;

		return *this;
	}

	/**
	 * Assignment operator (standard templater string).
	 *
	 * @param str: The standard template string.
	 */
	String& operator=(const TypeSTD& str)
	{
		// Return if the other string is empty.
		if (!str.size())
			return*this;

		length = str.length();

		// Allocate a new block in memory.
		string = CreateNewBlock(length + 1);

		// Copy the primitive string to the newly allocated memory block.
		std::memcpy(string, str.data(), length * typeSize());

		return *this;
	}

	/**
	 * Concatenate a primitive string to the stored string.
	 *
	 * @param pString: The primitive string.
	 */
	String& operator+=(const Type* pString)
	{
		size_t newLength = length + GetStrLength(pString);

		// Create a new memory block.
		Type* pBlock = CreateNewBlock(newLength + 1);

		// Copy the existing and new data to the new memory block.
		std::memcpy(pBlock, string, length * typeSize());
		std::memcpy(reinterpret_cast<Type*>(reinterpret_cast<size_t>(pBlock) + (length * typeSize())), pString, GetStrLength(pString) * typeSize());

		// Clear and assign the new values to the string.
		clear();
		string = pBlock;
		length = newLength;

		return *this;
	}

	/**
	 * Concatenate a string to the stored string (copy).
	 *
	 * @param str: The primitive string.
	 */
	String& operator+=(const String& str)
	{
		size_t newLength = length + str.length;

		// Create a new memory block.
		Type* pBlock = CreateNewBlock(newLength + 1);

		// Copy the existing and new data to the new memory block.
		std::memcpy(pBlock, string, length * typeSize());
		std::memcpy(reinterpret_cast<Type*>(reinterpret_cast<size_t>(pBlock) + (length * typeSize())), str.string, str.length * typeSize());

		// Clear and assign the new values to the string.
		clear();
		string = pBlock;
		length = newLength;

		return *this;
	}

	/**
	 * Concatenate a string to the stored string (move).
	 *
	 * @param str: The primitive string.
	 */
	String& operator+=(String&& str)
	{
		size_t newLength = length + str.length;

		// Create a new memory block.
		Type* pBlock = CreateNewBlock(newLength + 1);

		// Copy the existing and new data to the new memory block.
		std::memcpy(pBlock, string, length * typeSize());
		std::memcpy(reinterpret_cast<Type*>(reinterpret_cast<size_t>(pBlock) + (length * typeSize())), str.string, str.length * typeSize());

		// Clear and assign the new values to the string.
		clear();
		string = pBlock;
		length = newLength;

		// Clear the other string.
		str.clear();

		return *this;
	}

	/**
	 * Is equal operator.
	 *
	 * @param pString: The primitive string.
	 */
	bool operator==(const Type* pString)
	{
		// Check if the sizes are equal.
		if (length != GetStrLength(pString))
			return false;

		// Check if the characters are equal.
		for (size_t itr = 0; itr < length; itr++)
			if (string[itr] != pString[itr])
				return false;

		return true;
	}

	/**
	 * Is equal operator.
	 *
	 * @param other: The other string.
	 */
	bool operator==(const String& other)
	{
		// Check if the sizes are equal.
		if (length != other.length)
			return false;

		// Check if the characters are equal.
		for (size_t itr = 0; itr < length; itr++)
			if (string[itr] != other.string[itr])
				return false;

		return true;
	}

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

private:
	/**
	 * Helper to create a new block in memory and initialize the memory to zero.
	 *
	 * @param count: The total count of characteres to be stored (including '\0').
	 */
	__forceinline Type* CreateNewBlock(size_t count) const
	{
		// Allocate a new block in memory.
		Type* pBlock = new Type[count];

		// Set the initial values to 0.
		std::memset(pBlock, 0, count * typeSize());

		return pBlock;
	}

private:
	size_t length = 0;			// Length of the string.
	Type* string = nullptr;		// The string data pointer.
};


/**
 * Global concatenation operator for the String object.
 *
 * @param lhs: Left hand side argument.
 * @param rhs: Right hand side argument (const Type*).
 */
template<class Type = char>
[[nodiscard]] __forceinline String operator+(const String& lhs, const Type* rhs)
{
	String newString = lhs;
	newString += rhs;

	return newString;
}

/**
 * Global concatenation operator for the String object.
 *
 * @param lhs: Left hand side argument (const Type*).
 * @param rhs: Right hand side argument.
 */
template<class Type = char>
[[nodiscard]] __forceinline String operator+(const Type* lhs, const String& rhs)
{
	String newString = lhs;
	newString += rhs;

	return newString;
}

/**
 * Global concatenation operator for the String object (copy).
 *
 * @param lhs: Left hand side argument.
 * @param rhs: Right hand side argument.
 */
template<class Type = char>
[[nodiscard]] __forceinline String operator+(const String& lhs, const String& rhs)
{
	String newString = lhs;
	newString += rhs;

	return newString;
}

/**
 * Global concatenation operator for the String object (move).
 *
 * @param lhs: Left hand side argument.
 * @param rhs: Right hand side argument.
 */
template<class Type = char>
[[nodiscard]] __forceinline String operator+(String&& lhs, String&& rhs)
{
	String newString = std::move(lhs);
	newString += std::move(rhs);

	return newString;
}
