#pragma once
#include <unordered_map>
#include <vector>
#include <string>
#include <typeinfo>

/**
 * Base container class.
 * This class is the parent for the templated container object.
 */
class ContainerBase {
public:
	/**
	 * Default constructor.
	 */
	ContainerBase() {}

	/**
	 * Default destructor.
	 */
	virtual ~ContainerBase() {}

	/**
	 * Virtual method to get the count of the stored data.
	 */
	virtual size_t size() { return 0; }
};

/**
 * Container for the component system.
 * This object stores the object data.
 *
 * @tparam Type: The type of the data.
 */
template<class Type>
class Container : public ContainerBase {
public:
	/**
	 * Default constructor.
	 */
	Container() {}

	/**
	 * Default destructor.
	 */
	~Container() {}

	/**
	 * Get the pointer of the vector storing the data.
	 */
	std::vector<Type>* getVector() const { return const_cast<std::vector<Type>*>(&dataStore); }

	virtual size_t size() override final { return dataStore.size(); }

private:
	std::vector<Type> dataStore;	// Vector containing the data.
};

/**
 * The component registry.
 * This object contains all the components.
 */
class ComponentRegistry {
public:
	/**
	 * Default constructor.
	 */
	ComponentRegistry() {}

	/**
	 * Default destructor.
	 */
	~ComponentRegistry()
	{
		clear();
	}

	/**
	 * Check if a certain type is registered in the registry.
	 */
	template<class Type>
	bool isRegistered()
	{
		// Return false if there are no type names stored.
		if (!typeNames.size())
			return false;

		// Get the type ID as a string.
		std::string typeName = typeid(Type).name();

		// Iterate through the vector to check if the type name is registered.
		for (size_t index = 0; index < typeNames.size(); index++)
			if (typeNames[index] == typeName)
				return true;

		return false;
	}

	/**
	 * Register a new type.
	 * If the type already exists, it returns and does not do anything.
	 */
	template<class Type>
	void registerType()
	{
		// Return if the type is already registered.
		if (isRegistered<Type>())
			return;

		// Get the type ID as a string.
		std::string typeName = typeid(Type).name();

		// Register the new type.
		containerMap[typeName] = new Container<Type>;
		typeNames.push_back(std::move(typeName));
	}

	/**
	 * Get the container of a specific type.
	 * This creates a new container if it was not registered.
	 */
	template<class Type>
	Container<Type>* getContainer()
	{
		// Register the type. It does not do anything if the type is already registered.
		registerType<Type>();

		// Return the casted container.
		return dynamic_cast<Container<Type>*>(containerMap[typeid(Type).name()]);
	}

	/**
	 * Clear the stored data.
	 */
	void clear()
	{
		// Clear the container map.
		for (auto iterator : containerMap)
			delete iterator.second;

		containerMap.clear();
		typeNames.clear();
	}

private:
	std::vector<std::string> typeNames;								// Vector containing all the registered type names.
	std::unordered_map<std::string, ContainerBase*> containerMap;	// Unordered map containing all the containers.
};

/**
 * The component system.
 * This object contains utilities to create, manage and delete component registries.
 * If the component system is terminated, all the registries created by this will be destroyed.
 */
class ComponentSystem {
public:
	/**
	 * Default constructor.
	 */
	ComponentSystem() {}

	/**
	 * Default destructor.
	 */
	~ComponentSystem()
	{
		clearAllCreatedRegistries();
	}

	/**
	 * Create a new component registry.
	 * Returns the newly allocated registry pointer.
	 */
	ComponentRegistry* createRegistry()
	{
		// Allocate a new registry and add it to the garbage collector.
		auto pRegistry = new ComponentRegistry;
		garbageCollector[getPointerAsInteger(pRegistry)] = pRegistry;

		return pRegistry;
	}

	/**
	 * Destroy a created registry.
	 * This clears the registry from the garbage collector.
	 *
	 * @param pComponentRegistry: The allocated component registry.
	 */
	void destroyComponentRegistry(ComponentRegistry* pComponentRegistry)
	{
		// Check if the component is registry is valid.
		if (!pComponentRegistry)
			return;

		// Remove the entry from the garbage collector and deallocate the registry.
		garbageCollector.erase(getPointerAsInteger(pComponentRegistry));
		delete pComponentRegistry;
	}

	/**
	 * Clear all the created registries.
	 */
	void clearAllCreatedRegistries()
	{
		// Iterate through the map and deallocate all the registries.
		for (auto iterator : garbageCollector)
			delete iterator.second;

		garbageCollector.clear();
	}

public:
	/**
	 * Add a component to the component registry.
	 *
	 * @param pRegistry: The registry to add the data to.
	 * @param component: The component data to be added.
	 */
	template<class Type>
	void addComponent(ComponentRegistry* pRegistry, const Type& component = Type())
	{
		// Check if the registry is valid.
		if (!pRegistry)
			return;

		// Add the component to the vector.
		pRegistry->getContainer<Type>()->getVector()->push_back(component);
	}

	/**
	 * Get a component from the registry using its index.
	 *
	 * @param pRegistry: The registry to access data from.
	 * @param index: The index of the component.
	 * @warn: Make sure that the pRegistry is a valid registry pointer.
	 */
	template<class Type>
	Type& getComponent(ComponentRegistry* pRegistry, size_t index)
	{
		// Get the component using the index.
		return pRegistry->getContainer<Type>()->getVector()->at(index);
	}

	/**
	 * Get a component from the registry using its index.
	 *
	 * @param pRegistry: The registry to access data from.
	 * @param index: The index of the component.
	 */
	template<class Type>
	const Type getComponent(ComponentRegistry* pRegistry, size_t index) const
	{
		// Check if the registry is valid.
		if (!pRegistry)
			return Type();

		// Get the component using the index.
		return pRegistry->getContainer<Type>()->getVector()->at(index);
	}

private:
	/**
	 * Get the pointer as an integer.
	 * This converts the address to an integer.
	 *
	 * @param pointer: The pointer to be converted to a integer.
	 */
	__forceinline size_t getPointerAsInteger(void* pointer)
	{
		return reinterpret_cast<size_t>(pointer);
	}

private:
	std::unordered_map<size_t, ComponentRegistry*> garbageCollector;  // Map storing the allocated component registries.
};
