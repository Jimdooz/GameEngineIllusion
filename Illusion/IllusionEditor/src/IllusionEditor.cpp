#include "ecs/Entity.h"
#include "ecs/Component.h"

#include <iostream>
#include <vector>
#include <bitset>
#include <chrono>
#include <map>

//auto start = high_resolution_clock::now();
//auto stop = high_resolution_clock::now();
//std::cout << "DURATION : " << duration_cast<microseconds>(stop - start).count() / 1000000.0 << std::endl;

using namespace std::chrono;
using namespace illusion;

struct ComponentSY {
	virtual void AddComponent(ecs::entity_id id) {
		INTERNAL_INFO("ADD COMPONENT ", id)
		ToData[ecs::id::Index(id)] = ecs::component_id{ static_cast<u32>(ToEntity.size()) };
		ToEntity.push_back(id);
	}
	virtual void RemoveComponent(ecs::entity_id id) {
		INTERNAL_INFO("REMOVE COMPONENT ", id, " ", ecs::id::Index(id))
		const ecs::entity_id getId{ ecs::id::Index(id) };
		const ecs::entity_id lastId{ ecs::id::Index(ToEntity.back()) };
		const ecs::entity_id index{ ToData[getId] };

		util::EraseUnordered(ToEntity, index);
		ToData[lastId] = ecs::component_id{ index };
		ToData[getId] = ecs::component_id{ ecs::id::invalid_id };
	}

	virtual void CreateEntity(ecs::entity_id id){
		INTERNAL_INFO("CREATE ENTITY")
		if (ecs::id::Index(id) >= ToData.size()) ToData.resize(ecs::id::Index(id) + 1);
		ToData[ecs::id::Index(id)] = ecs::component_id{ ecs::id::invalid_id };
	}
	virtual void DestroyEntity(ecs::entity_id id){
		if(ToData[ecs::id::Index(id)] != ecs::component_id{ ecs::id::invalid_id }) RemoveComponent(id);
		ToData[ecs::id::Index(id)] = ecs::component_id{ ecs::id::invalid_id };
	}

	illusion::util::Array<ecs::entity_id> ToEntity;
	illusion::util::Array<ecs::component_id> ToData;
};

struct TransformComponentSY : public ComponentSY {
	util::Array<Vec3> position;
	util::Array<Quaternion> rotation;
	util::Array<Vec3> scale;
};

struct RigidbodyComponentSY : public ComponentSY {
	util::Array<Vec3> velocity;
};

struct SceneSY {
	ecs::Entities entities;
	std::map<size_t, ComponentSY*> components;

	ecs::entity_id CreateEntity() {
		ecs::entity_id id = entities.Create({});
		for (auto const& [key, val] : components) {
			val->CreateEntity(id);
		}
		return id;
	}

	void DestroyEntity(ecs::entity_id id) {
		for (auto const& [key, val] : components) {
			val->DestroyEntity(id);
		}
		entities.Destroy(id);
	}

	/**
	 * Component Part
	 */
	template<typename C> void AddComponent() {
		components[typeid(C).hash_code()] = new C();
	}

	template<typename C> bool ComponentExist() {
		return components.find(typeid(C).hash_code()) != components.end();
	}

	template<typename C> ComponentSY& GetComponentSystem() {
		return *components[typeid(C).hash_code()];
	}

	template<typename C> C& GetComponent() {
		return static_cast<C&>(GetComponentSystem<C>());
	}
};

int main(int argc, char* argv[]) {
	SceneSY scene1;
	scene1.AddComponent<TransformComponentSY>();
	scene1.AddComponent<RigidbodyComponentSY>();

	for (u32 i = 0; i < 5; i++) {
		ecs::entity_id id = scene1.CreateEntity();
		scene1.GetComponent<TransformComponentSY>().AddComponent(id);
		scene1.DestroyEntity(id);
	}
	std::cin.get();
}