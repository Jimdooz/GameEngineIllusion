#include "ecs/Entity.h"
#include "ecs/Component.h"
#include "ecs/System.h"
#include "ecs/Scene.h"

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

///--> COMPONENTS
struct TransformComponentSY : public ecs::Component {
	util::Array<Vec3> position;
	util::Array<Quaternion> rotation;
	util::Array<Vec3> scale;

	virtual void AddComponentDatas(ecs::entity_id id) override {
		AddComponentData(position, Vec3(0, 0, 0));
		AddComponentData(rotation, Quaternion(0,0,0,1));
		AddComponentData(scale, Vec3(1,1,1));
	}
	virtual void RemoveComponentDatas(ecs::entity_id index) {
		RemoveComponentData(position, index);
		RemoveComponentData(rotation, index);
		RemoveComponentData(scale, index);
	}
};

struct RigidbodyComponentSY : public ecs::Component {
	util::Array<Vec3> velocity;

	virtual void AddComponentDatas(ecs::entity_id id) override {
		AddComponentData(velocity, Vec3(0, 0, 0));
	}
	virtual void RemoveComponentDatas(ecs::entity_id index) {
		RemoveComponentData(velocity, index);
	}
};

///--> SYSTEMS
struct TwerkSystem : public ecs::System {

	util::Array<Vec3*> position;

	virtual void Update() override {
		u32 size = ToEntity.size();
		for (u32 i = 0; i < size; i++) {
			INTERNAL_INFO(i, " ", position[i], " ", size);
			position[i]->x += 5;
		}
	}

	virtual void AddDatas(ecs::Scene& scene, ecs::entity_id id) override {
		TransformComponentSY &t = scene.GetComponent<TransformComponentSY>();
		AddData(position, &t.position[t.getIndex(id)]);
	}
	virtual void RemoveDatas(ecs::entity_id index) override {
		RemoveData(position, index);
	}

	virtual bool AcceptedComponent(ecs::Scene& scene, size_t hashComponent) override {
		if (hashComponent == typeid(TransformComponentSY).hash_code()) return true;
		return false;
	}
};

int main(int argc, char* argv[]) {
	ecs::Scene scene1;
	scene1.AddComponent<TransformComponentSY>();
	scene1.AddComponent<RigidbodyComponentSY>();

	scene1.AddSystem<TwerkSystem>();

	ecs::entity_id id;
	for (u32 i = 0; i < 1000; i++) {
		id = scene1.CreateEntity();
		scene1.AddComponentEntity<TransformComponentSY>(id);
	}
	scene1.Update();

	/*for (int test = 0; test < 10; test++) {
		auto start = high_resolution_clock::now();

		for (int i = 0; i < 1000; i++) {
			scene1.Update();
		}

		auto stop = high_resolution_clock::now();
		std::cout << "DURATION : " << duration_cast<microseconds>(stop - start).count() / 1000000.0 << std::endl;
	}

	INTERNAL_INFO(scene1.systems[0]->ToEntity.size())*/

	std::cin.get();
}