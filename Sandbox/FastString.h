#pragma once

/**
 * Fast string class.
 * This object stores strings in stack.
 */
class FastString {
public:
#ifdef USE_WCHAR
	// Primitive type.
	using Type = wchar_t;

#else 
	// Primitive type.
	using Type = char;

#endif // USE_WCHAR

public:
	/**
	 * Default constructor.
	 */
	FastString() : pString(nullptr) {}

	/**
	 * Default destructor.
	 */
	~FastString() {}

private:
	Type* pString = nullptr;	// The primitive string pointer.
};