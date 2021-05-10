#pragma once
#include <memory>
#include <unordered_map>

/**
 * Automated memory manager.
 * A singleton class which deletes all the heap allocated memory upon termination.
 * Blocks allocated by this must be deleted by this deleted by this or else errors might occur.
 */
class AutomatedMemoryManager {
	/**
	 * Default contructor.
	 */
	AutomatedMemoryManager() {}

	/**
	 * Default destructor.
	 * This deallocates all the allocated memory.
	 */
	~AutomatedMemoryManager() { deallocateAll(); }

	// Static instance of the automated memory manager.
	static AutomatedMemoryManager instance;

	/**
	 * Structre containing information about a memory allocation.
	 */
	struct MemoryBlock {
		/**
		 * Default constructor.
		 * 
		 * @param pMemory: The to the memory block.
		 * @param size: The size of the memory block.
		 * @param alignment: The alignment of the block.
		 */
		MemoryBlock(void* pMemory = 0, size_t size = 0, size_t alignment = 0)
			: pMemory(pMemory), byteSize(size), alignment(alignment) {}

		void* pMemory = nullptr;	// Pointer to the memory block.
		size_t byteSize = 0;		// Memory block size.
		size_t alignment = 0;		// Alignment of the block.
	};

public:
	/**
	 * Allocate a new memory block using the automated memory manager.
	 *
	 * @param size: The size of the allocation. Default is sizeof(Type).
	 * @param alignment: The alignment of the allocation. Default is sizeof(Type).
	 */
	template<class Type>
	static Type* allocate(size_t size = sizeof(Type), size_t alignment = sizeof(Type))
	{
		// Create a new memory block with the new alocation and add it to the map. 
		MemoryBlock newBlock(operator new (size, std::align_val_t{ alignment }), size, alignment);
		instance.memoryBlockMap[reinterpret_cast<size_t>(newBlock.pMemory)] = newBlock;

		// Return the casted memory block.
		return static_cast<Type*>(newBlock.pMemory);
	}

	/**
	 * Deallocate a memory block using its address.
	 *
	 * @param pMemory: The address of the memory block.
	 */
	static void deallocate(void* pMemory)
	{
		// Check if the memory block is valid.
		if (!pMemory)
			return;

		// Check if the memory block is alocated by the automated memory manager.
		if (instance.memoryBlockMap.find(reinterpret_cast<size_t>(pMemory)) == instance.memoryBlockMap.end())
			return;

		// Get the allocated memory block.
		MemoryBlock memBlock = instance.memoryBlockMap[reinterpret_cast<size_t>(pMemory)];

		// Delete the memory block and erase it from the map.
		operator delete (memBlock.pMemory, memBlock.byteSize, std::align_val_t{ memBlock.alignment });
		instance.memoryBlockMap.erase(reinterpret_cast<size_t>(pMemory));
	}

	/**
	 * Deallocate all the allocated memory blocks.
	 * Make sure that the pointers are not dangling after the deallocation.
	 */
	static void deallocateAll()
	{
		// Deallocated the allocated memory blocks.
		for (auto block : instance.memoryBlockMap)
			operator delete (block.second.pMemory, block.second.byteSize, std::align_val_t{ block.second.alignment });

		// Clear the memory map.
		instance.memoryBlockMap.clear();
	}

private:
	std::unordered_map<size_t, MemoryBlock> memoryBlockMap;		// Map of the alocated memory blocks,
};

// Static instance definition of the Automated Memory Manager.
inline AutomatedMemoryManager AutomatedMemoryManager::instance;