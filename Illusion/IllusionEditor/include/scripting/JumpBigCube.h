#pragma once

#include "ecs/Component.h"

using namespace illusion;

struct JumpBigCube : public ecs::Component {
	COMPONENT_NAME("Jump My Big Cube");
	COMPONENT_REGISTER(JumpBigCube);

	JumpBigCube(ecs::Scene* scene) : Component(scene) {
		COMPONENT_PUBLIC(direction);
		COMPONENT_PUBLIC(powerJump);
	}

	COMPONENT_DATA(Vec3, direction);
	COMPONENT_DATA(f32, powerJump);
	COMPONENT_DATA(boolean, activeJump);
	COMPONENT_DATA(f32, timerJump);

	// On Data added
	virtual void AddDatas(ecs::entity_id id) override {
		AddData(direction, Vec3(0,1,1));
		AddData(powerJump, 1.f);
		AddData<boolean>(activeJump, false);
		AddData(timerJump, 0.0f);
	}

	// On Data removed
	virtual void RemoveDatas(ecs::component_id index, ecs::entity_id id) {
		RemoveData(index, direction, powerJump, activeJump, timerJump);
	}
};

struct JumpBigCubeSystem : public ecs::System {
	SYSTEM_NAME("JumpBigCube");
	SYSTEM_REGISTER(JumpBigCubeSystem);

	ecs::core::Transform* transform;
	JumpBigCube* jumpBigCube;
	core::physics::RigidBody* rigidbody;

	/* la fonction Update */
	SYSTEM_UPDATE_LOOP(
		if (collisions().size() > 0 && timerJump() > 0.1f) activeJump() = true;
		timerJump() += Time::deltaTime;

		if (Input::isKeyDown(GLFW_KEY_SPACE) && activeJump()) {
			if (collisions().size() > 0) {
				velocity() = glm::normalize(direction()) * powerJump();
			}
			else velocity() = glm::normalize(direction()) * powerJump();
			activeJump() = false;
			timerJump() = 0.0f;
		}

		if (Input::isKey(GLFW_KEY_LEFT)) velocity() = Vec3(-5, velocity().y, 0);
		if (Input::isKey(GLFW_KEY_RIGHT)) velocity() = Vec3(5, velocity().y, 0);

		if(!Input::isKey(GLFW_KEY_LEFT) && !Input::isKey(GLFW_KEY_RIGHT)) velocity() = Vec3(0, velocity().y, 0);
		position().z = 0;
	)

	/* Definition des variables utiles */
	SYSTEM_USE_DATA(direction, jumpBigCube, direction, Vec3);
	SYSTEM_USE_DATA(powerJump, jumpBigCube, powerJump, f32);
	SYSTEM_USE_DATA(activeJump, jumpBigCube, activeJump, boolean);
	SYSTEM_USE_DATA(timerJump, jumpBigCube, timerJump, f32);
	SYSTEM_USE_DATA(velocity, rigidbody, velocity, Vec3);
	SYSTEM_USE_DATA(collisions, rigidbody, collisions, util::Array<core::physics::CollisionRigidBody>);
	SYSTEM_USE_DATA(rotation, transform, rotation, Quaternion);
	SYSTEM_USE_DATA(position, transform, position, Vec3);

	/* Initialisation relative a la scene parente */
	virtual void Initialize(ecs::Scene& scene) override {
		transform = scene.GetComponent<ecs::core::Transform>();
		jumpBigCube = scene.GetComponent<JumpBigCube>();
		rigidbody = scene.GetComponent<core::physics::RigidBody>();
		SetDependencies(transform, jumpBigCube, rigidbody);
	}
};