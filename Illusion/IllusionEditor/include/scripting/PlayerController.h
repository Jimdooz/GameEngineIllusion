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
		COMPONENT_PUBLIC(powerJump);
	}

	COMPONENT_DATA(Vec3, currentDirection);
	COMPONENT_DATA(float, powerJump);
	COMPONENT_DATA(boolean, canJump);

	// On Data added
	virtual void AddDatas(ecs::entity_id id) override {
		AddData(currentDirection, Vec3(0, 0, 1));
		AddData(powerJump, 10.0f);
		AddData<boolean>(canJump, true);
	}

	// On Data removed
	virtual void RemoveDatas(ecs::component_id index, ecs::entity_id id) {
		RemoveData(currentDirection, index);
		RemoveData(powerJump, index);
		RemoveData(canJump, index);
	}
};

struct PlayerControllerSystem : public ecs::System {
	SYSTEM_NAME("PlayerController System");
	SYSTEM_REGISTER(PlayerControllerSystem);

	ecs::core::Transform* transforms;
	ecs::core::Camera* cameras;
	PlayerController* playerControllers;
	core::physics::RigidBody* rigidbodies;

	/* la fonction Update */
	virtual void FixedUpdate() override {
		u32 size = static_cast<u32>(ToEntity.size());
		for (currIndex = 0; currIndex < size; currIndex++) {
			f32 gamepadTreshold = 0.2f;
			f32 gamepadX = Input::getGamepadAxis(GLFW_GAMEPAD_AXIS_LEFT_X);
			f32 gamepadY = Input::getGamepadAxis(GLFW_GAMEPAD_AXIS_LEFT_Y);
			f32 rightTriggerDelta = Input::getGamepadAxis(GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER);
			f32 speed = 3.0f;

			Vec3 finalVelocity;

			if (collisions().size() > 0) canJump() = true;

			if (abs(gamepadX) > gamepadTreshold || abs(gamepadY) > gamepadTreshold) {

				Vec3 cameraPosition;
				Vec3 forward = Vec3(0, 0, 1);
				Vec3 right = Vec3(0, 0, 0);

				if (cameras->size() > 0) {
					cameraPosition = cameras->GetPosition();
					Vec3 cameraDirection = cameras->GetRotation() * Vec4(0, 0, 1, 1);
					cameraDirection.y = 0;
					forward = glm::normalize(cameraDirection);
				}

				right = glm::normalize(glm::cross(-forward, Vec3(0, 1, 0)));

				Vec3 forwardMovement = forward * speed * gamepadY;
				Vec3 rightMovement = right * speed * gamepadX;

				Vec3 wantedDirection = forwardMovement + rightMovement;

				currentDirection() = glm::mix(currentDirection(), wantedDirection, Time::fixedDeltaTime * 5.0f);

				rotation() = glm::conjugate(Quaternion(glm::lookAt(Vec3(0, 0, 0), glm::normalize(currentDirection()), Vec3(0, 1, 0))));

				finalVelocity = currentDirection() + Vec3(0, velocity().y, 0);
			}
			else {
				//currentDirection() = glm::mix(currentDirection(), Vec3(0,0,0), Time::fixedDeltaTime * 10.0f);
				finalVelocity = Vec3(0, velocity().y, 0);
			}

			if (rightTriggerDelta > 0 && canJump()) {
				finalVelocity.y = powerJump();
				canJump() = false;
			}

			velocity() = finalVelocity;
		}
	}

	/* Definition des variables utiles */
	SYSTEM_USE_DATA(currentDirection, playerControllers, currentDirection, Vec3);
	SYSTEM_USE_DATA(powerJump, playerControllers, powerJump, f32);
	SYSTEM_USE_DATA(canJump, playerControllers, canJump, boolean);
	SYSTEM_USE_DATA(velocity, rigidbodies, velocity, Vec3);
	SYSTEM_USE_DATA(collisions, rigidbodies, collisions, util::Array<core::physics::CollisionRigidBody>);
	SYSTEM_USE_DATA(rotation, transforms, rotation, Quaternion);
	SYSTEM_USE_DATA(position, transforms, position, Vec3);

	/* Initialisation relative a la scene parente */
	virtual void Initialize(ecs::Scene& scene) override {
		transforms = scene.GetComponent<ecs::core::Transform>();
		cameras = scene.GetComponent<ecs::core::Camera>();
		playerControllers = scene.GetComponent<PlayerController>();
		rigidbodies = scene.GetComponent<core::physics::RigidBody>();
		SetDependencies(transforms, playerControllers, rigidbodies);
	}
};