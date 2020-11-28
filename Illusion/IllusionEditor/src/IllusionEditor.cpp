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

ecs::Scene scene1;

///--> SYSTEMS
struct TwerkSystem : public ecs::System {

	TransformComponentSY* transform;

	virtual void Update() override {
		u32 size = ToEntity.size();
		for (u32 i = 0; i < size; i++) {
			getPosition(i).x += 5;
		}
	}

	inline Vec3& getPosition(u32 index) {
		return transform->position[transform->getIndex(ToEntity[index])];
	}

	virtual void Initialize(ecs::Scene& scene) override {
		transform = scene.GetComponent<TransformComponentSY>();
	}

	virtual bool AcceptedComponent(ecs::Scene& scene, size_t hashComponent) override {
		if (hashComponent == typeid(TransformComponentSY).hash_code()) return true;
		return false;
	}
};

int main(int argc, char* argv[]) {
	scene1.AddComponent<TransformComponentSY>();
	scene1.AddComponent<RigidbodyComponentSY>();

	scene1.AddSystem<TwerkSystem>();

	f64 average = 0.0;
	f64 timeIteration = 100;

	ecs::entity_id id;
	for (u32 i = 0; i < 10000; i++) {
		id = scene1.CreateEntity();
		scene1.AddComponentEntity<TransformComponentSY>(id);
		//if (i % 2 == 0) scene1.DestroyEntity(id);
	}

	for (int test = 0; test < timeIteration; test++) {
		auto start = high_resolution_clock::now();

		scene1.Update();

		auto stop = high_resolution_clock::now();
		average += duration_cast<microseconds>(stop - start).count() ;
		//std::cout << "DURATION : " << duration_cast<microseconds>(stop - start).count() / 1000000.0 << std::endl;
	}

	std::cout << "AVERAGE : " << 1.0 / ((average / 1000000.0) / timeIteration) << " FPS" << std::endl;
}