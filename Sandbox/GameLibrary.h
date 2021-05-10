#pragma once

#include <type_traits>
#include <Windows.h>
#include <iostream>

///////////////////////////////////////////////////////////////////////////////////////////////////
////	Graphics
///////////////////////////////////////////////////////////////////////////////////////////////////

namespace Graphics
{
	class GraphicsComponent {
	public:
		GraphicsComponent() {}
		virtual ~GraphicsComponent() {}

		void LoadModel(const char* pAsset) {}

		virtual void SubmitGraphicsSpecification() {}
	};
}

///////////////////////////////////////////////////////////////////////////////////////////////////
////	Physics
///////////////////////////////////////////////////////////////////////////////////////////////////

class GameEntity;

namespace Physics
{
	class PhysicsComponent {
	public:
		PhysicsComponent() {}
		virtual ~PhysicsComponent() {}

		void SetupCapsule(float* pDimentions) {}

		virtual void OnCollition(GameEntity* pEntity) {}
	};
}

///////////////////////////////////////////////////////////////////////////////////////////////////
////	Audio
///////////////////////////////////////////////////////////////////////////////////////////////////

namespace Audio
{
	class AudioComponent {
	public:
		AudioComponent() {}
		virtual ~AudioComponent() {}

		void LoadAudio(const char* pAsset) {}

		void PlayOneTime() {}
		void PlayLoop() {}
	};
}

///////////////////////////////////////////////////////////////////////////////////////////////////
////	Game Core
///////////////////////////////////////////////////////////////////////////////////////////////////

class GameComponent {
public:
	GameComponent() {}
	virtual ~GameComponent() {}

	template<class Type>
	Type* Inherit() { return dynamic_cast<Type*>(this); }
};

class GameEntity : public GameComponent {
public:
	GameEntity() {}
	virtual ~GameEntity() {}

	virtual void OnSpawn() {}
	virtual void OnDespawn() {}

	virtual void OnUpdate() {}
};

class GameModule : public GameComponent {
public:
	GameModule() {}
	virtual ~GameModule() {}

	virtual void OnInitialize() {}

	template<class Entity>
	__forceinline Entity* SpawnEntity(const Entity& pInitializer = Entity())
	{
		if constexpr (!std::is_base_of<GameEntity, Entity>::value)
		{
			// Log error.
			return nullptr;
		}
		else
		{
			std::cout << "Corrext!";
		}

		// Check if its a graphics component.
		if constexpr (std::is_base_of<Graphics::GraphicsComponent, Entity>::value)
		{
			Sleep(100);
		}

		// Check if its a physics component.
		if constexpr (std::is_base_of<Physics::PhysicsComponent, Entity>::value)
		{
			Sleep(100);
		}

		// Check if its a audio component.
		if constexpr (std::is_base_of<Audio::AudioComponent, Entity>::value)
		{
			Sleep(100);
		}

		return nullptr;
	}
};

///////////////////////////////////////////////////////////////////////////////////////////////////
////	User Implementation
///////////////////////////////////////////////////////////////////////////////////////////////////

class Tree :
	public GameEntity,
	public Graphics::GraphicsComponent,
	public Physics::PhysicsComponent,
	public Audio::AudioComponent
{
public:
	Tree() {}
	~Tree() {}

	virtual void OnSpawn() override final
	{
		LoadModel("bg.obj");
		LoadAudio("bg.mp3");

		float extent[] = { 0.0f, 1.0f };
		SetupCapsule(extent);
	}

	virtual void OnDespawn() override final
	{
	}

	virtual void OnUpdate() override final
	{
	}

	virtual void OnCollition(GameEntity* pEntity) override final
	{
		auto pointer = pEntity->Inherit<Tree>();
	}
};

class Mesh : public Graphics::GraphicsComponent {};
class Body : public Physics::PhysicsComponent, public Graphics::GraphicsComponent {};
class AudioM : public GameEntity, public Audio::AudioComponent {};

class TestModule : public GameModule {
public:
	TestModule() {}
	~TestModule() {}

	virtual void OnInitialize() override final
	{
		SpawnEntity<Tree>();
		SpawnEntity<Mesh>();
		SpawnEntity<Body>();
		SpawnEntity<AudioM>();
	}
};
