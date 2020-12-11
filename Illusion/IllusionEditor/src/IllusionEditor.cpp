#include "ecs/Entity.h"
#include "ecs/Component.h"
#include "ecs/System.h"
#include "ecs/Scene.h"
#include "IllusionEngine.h"

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

///--> SYSTEMS
struct TwerkSystem : public ecs::System {

	ecs::core::Transform* transform;

	/* la fonction Update */
	SYSTEM_UPDATE_LOOP(
		position().x += 5;
		rotation().x++;
		rotation().y++;
		scale().x++;
	)

	/* D?finition des variables utiles */
	SYSTEM_USE_DATA(position, transform, position, Vec3)
	SYSTEM_USE_DATA(rotation, transform, rotation, Quaternion)
	SYSTEM_USE_DATA(scale, transform, scale, Vec3)

	/* Initialisation relative ? la sc?ne parente */
	virtual void Initialize(ecs::Scene& scene) override {
		transform = scene.GetComponent<ecs::core::Transform>();
		SetDependencies(transform);
	}
};

int main(int argc, char* argv[]) {
	Window::Create(1280,720,"MyGame");
	while (!Window::shouldClose) {
		if (Input::isKeyDown(GLFW_KEY_A)) {
			INFO("PRESSED KEY A");
		}
		if (Input::getMouseWheelDelta() != 0)
		{
			INFO("MOUSE WHEEL : ", Input::getMouseWheelDelta());
		}
		if (Input::isKeyDown(GLFW_KEY_ESCAPE)) {
			INFO("PRESSED KEY ESCAPE");
			Window::Close();
		}
		Window::Update();		
	}
	Window::Destroy();

	/*
	ecs::Scene scene1; // E C S
	scene1.UseSystem<TwerkSystem>();


	f64 average = 0.0;
	f64 timeIteration = 50;

	ecs::entity_id id;
	ecs::entity_id firstId;
	for (u32 i = 0; i < 1000; i++) {
		id = scene1.CreateEntity();
	}

	INTERNAL_INFO("INIT END")

	for (int test = 0; test < timeIteration; test++) {
		auto start = high_resolution_clock::now();

		scene1.Update();
		//scene1.DestroyEntity(firstId);

		auto stop = high_resolution_clock::now();
		average += duration_cast<microseconds>(stop - start).count() ;
		std::cout << "DURATION : " << duration_cast<microseconds>(stop - start).count() / 1000000.0 << std::endl;
	}

	std::cout << "AVERAGE : " << 1.0 / ((average / 1000000.0) / timeIteration) << " FPS" << std::endl;
	*/
}