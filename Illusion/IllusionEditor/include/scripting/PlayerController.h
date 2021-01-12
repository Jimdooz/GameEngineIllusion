#pragma once

#include "ecs/Component.h"
#include "ecs/System.h"
#include "ecs/Scene.h"

#include "core/physics/components/RigidBody.h"

#include "Input.h"

using namespace illusion;

struct PlayerController : public ecs::Component {
	COMPONENT_NAME("PlayerController");
	COMPONENT_REGISTER(PlayerController);

	PlayerController(ecs::Scene* scene) : Component(scene) {
	}

	// On Data added
	virtual void AddDatas(ecs::entity_id id) override {
	}

	// On Data removed
	virtual void RemoveDatas(ecs::component_id index, ecs::entity_id id) {
	}
};

struct PlayerControllerSystem : public ecs::System {
	SYSTEM_NAME("PlayerController System");
	SYSTEM_REGISTER(PlayerControllerSystem);

	ecs::core::Transform* transforms;
	PlayerController* playerControllers;
	core::physics::RigidBody* rigidbodies;

	/* la fonction Update */
	virtual void FixedUpdate() override {
		u32 size = static_cast<u32>(ToEntity.size());
		for (currIndex = 0; currIndex < size; currIndex++) {
			f32 gamepadTreshold = 0.2f;
			f32 gamepadX = Input::getGamepadAxis(GLFW_GAMEPAD_AXIS_LEFT_X);
			f32 gamepadY = Input::getGamepadAxis(GLFW_GAMEPAD_AXIS_LEFT_Y);
			f32 speed = 3.0f;

			if (abs(gamepadX) > gamepadTreshold || abs(gamepadY) > gamepadTreshold) {
				INTERNAL_INFO(gamepadX, " / ", gamepadY);
				velocity() = Vec3(speed * gamepadX, velocity().y, speed * gamepadY);
			}
		}
	}

	/* Definition des variables utiles */
	SYSTEM_USE_DATA(velocity, rigidbodies, velocity, Vec3);
	SYSTEM_USE_DATA(collisions, rigidbodies, collisions, util::Array<core::physics::CollisionRigidBody>);
	SYSTEM_USE_DATA(rotation, transforms, rotation, Quaternion);
	SYSTEM_USE_DATA(position, transforms, position, Vec3);

	/* Initialisation relative a la scene parente */
	virtual void Initialize(ecs::Scene& scene) override {
		transforms = scene.GetComponent<ecs::core::Transform>();
		playerControllers = scene.GetComponent<PlayerController>();
		rigidbodies = scene.GetComponent<core::physics::RigidBody>();
		SetDependencies(transforms, playerControllers, rigidbodies);
	}
};