#pragma once

#include "ecs/Component.h"
#include "ecs/System.h"
#include "ecs/Scene.h"

#include "core/physics/components/RigidBody.h"
#include <glm/gtx/quaternion.hpp>

#include "Input.h"

using namespace illusion;

struct CameraPlayer : public ecs::Component {
	COMPONENT_NAME("CameraPlayer");
	COMPONENT_REGISTER(CameraPlayer);

	CameraPlayer(ecs::Scene* scene) : Component(scene) {
		COMPONENT_PUBLIC_CUSTOM(target, R"({ "entity": true })"_json);
		COMPONENT_PUBLIC(distanceTarget);
		COMPONENT_PUBLIC(speedRotationX);
		COMPONENT_PUBLIC(speedRotationZ);
	}

	COMPONENT_DATA(ecs::entity_id, target);
	COMPONENT_DATA(f32, distanceTarget);
	COMPONENT_DATA(f32, rotationsX);
	COMPONENT_DATA(f32, rotationsZ);

	COMPONENT_DATA(f32, speedRotationX);
	COMPONENT_DATA(f32, speedRotationZ);

	// On Data added
	virtual void AddDatas(ecs::entity_id id) override {
		AddData(target, (ecs::entity_id)ecs::id::invalid_id);
		AddData(distanceTarget, 3.0f);
		AddData(rotationsX, 0.0f);
		AddData(rotationsZ, 0.0f);
		AddData(speedRotationX, 1.0f);
		AddData(speedRotationZ, 1.0f);
	}

	// On Data removed
	virtual void RemoveDatas(ecs::component_id index, ecs::entity_id id) {
		RemoveData(target, index);
		RemoveData(distanceTarget, index);
		RemoveData(rotationsX, index);
		RemoveData(rotationsZ, index);
		RemoveData(speedRotationX, index);
		RemoveData(speedRotationZ, index);
	}
};

struct CameraPlayerSystem : public ecs::System {
	SYSTEM_NAME("CameraPlayer System");
	SYSTEM_REGISTER(CameraPlayerSystem);

	ecs::core::Transform* transforms;
	CameraPlayer* cameraControllers;
	ecs::core::Camera* cameras;

	ecs::Scene* scene;

	/* la fonction Update */
	virtual void LateUpdate() override {
		u32 size = static_cast<u32>(ToEntity.size());
		for (currIndex = 0; currIndex < size; currIndex++) {
			Vec3 cameraPosition = cameras->GetPosition();

			f32 gamepadTreshold = 0.2f;
			f32 gamepadX = Input::getGamepadAxis(GLFW_GAMEPAD_AXIS_RIGHT_X);
			f32 gamepadY = Input::getGamepadAxis(GLFW_GAMEPAD_AXIS_RIGHT_Y);
			f32 speed = 3.0f;

			if (abs(gamepadX) > gamepadTreshold || abs(gamepadY) > gamepadTreshold) {
				rotationsX() -= gamepadX * Time::unscaledDeltaTime * speedRotationX();
				rotationsZ() += gamepadY * Time::unscaledDeltaTime * speedRotationZ();
				if (rotationsZ() <= 0) rotationsZ() = 0.0f;
				if (rotationsZ() >= 1.0f) rotationsZ() = 1.0f;
			}
			
			if (scene->entities.IsAlive(target())) {
				Vec3 positionObject = transforms->GetPosition(target());
				float rotX = sin(rotationsX());
				float rotY = sin(rotationsZ()) * 2.0f;
				float rotZ = cos(rotationsX());
				position() = Vec3(positionObject.x, positionObject.y, positionObject.z) + glm::normalize(Vec3(rotX, rotY, rotZ)) * distanceTarget();

				Quaternion orientation = glm::conjugate(glm::toQuat(glm::lookAt(position(), positionObject, Vec3(0, 1, 0))));

				rotation() = orientation;
			}

		}
	}

	/* Definition des variables utiles */
	SYSTEM_USE_DATA(target, cameraControllers, target, ecs::entity_id);
	SYSTEM_USE_DATA(rotationsX, cameraControllers, rotationsX, f32);
	SYSTEM_USE_DATA(rotationsZ, cameraControllers, rotationsZ, f32);
	SYSTEM_USE_DATA(speedRotationX, cameraControllers, speedRotationX, f32);
	SYSTEM_USE_DATA(speedRotationZ, cameraControllers, speedRotationZ, f32);
	SYSTEM_USE_DATA(distanceTarget, cameraControllers, distanceTarget, f32);
	SYSTEM_USE_DATA(rotation, transforms, rotation, Quaternion);
	SYSTEM_USE_DATA(position, transforms, position, Vec3);

	/* Initialisation relative a la scene parente */
	virtual void Initialize(ecs::Scene& scene) override {
		this->scene = &scene;
		transforms = scene.GetComponent<ecs::core::Transform>();
		cameraControllers = scene.GetComponent<CameraPlayer>();
		cameras = scene.GetComponent<ecs::core::Camera>();
		SetDependencies(transforms, cameraControllers, cameras);
	}
};