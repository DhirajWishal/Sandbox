#pragma once
#include <memory>

template<class Type>
class SharedRef {
public:
	SharedRef() : pMemory(nullptr) {}
	~SharedRef() {}

	/**
	 * Create a new shared pointer.
	 *
	 * @param data: The data to be initialized with. Default is Type().
	 */
	__forceinline static SharedRef<Type> Create(const Type& data = Type())
	{
		return SharedRef<Type>();
	}

private:
	Type* pMemory = nullptr;
};

template<class Type>
class PairPtr {
public:
	PairPtr() : pParent(this), pChild(nullptr), pData(nullptr) {}

	PairPtr(PairPtr<Type>&& pPtr)
	{
		this->pParent = pPtr.pParent;
		this->pData = pPtr.pData;
		this->pChild = this;

		if (pParent)
			pParent->pChild = this;
		else
			pParent = this;
	}

	~PairPtr()
	{
		if ((pParent == this) && (pChild == nullptr) && (pData))
		{
			printf("Deleting!");
			delete pData;
		}

		pParent = nullptr;
		pChild = nullptr;
		pData = nullptr;
	}

	static PairPtr<Type> Create()
	{
		PairPtr<Type> newPtr;
		newPtr.pData = new Type;

		return newPtr;
	}

	Type operator->()
	{
		return *pData;
	}

	Type operator*()
	{
		return *pData;
	}

private:
	PairPtr* pParent = nullptr;
	PairPtr* pChild = nullptr;
	Type* pData = nullptr;
};
