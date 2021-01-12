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
	}

	COMPONENT_DATA(ecs::entity_id, target);

	// On Data added
	virtual void AddDatas(ecs::entity_id id) override {
		AddData(target, (ecs::entity_id)ecs::id::invalid_id);
	}

	// On Data removed
	virtual void RemoveDatas(ecs::component_id index, ecs::entity_id id) {
		RemoveData(index, target);
	}
};

struct CameraPlayerSystem : public ecs::System {
	SYSTEM_NAME("CameraPlayer System");
	SYSTEM_REGISTER(CameraPlayerSystem);

	ecs::core::Transform* transforms;
	CameraPlayer* cameraControllers;
	ecs::core::Camera* cameras;

	/* la fonction Update */
	virtual void LateUpdate() override {
		u32 size = static_cast<u32>(ToEntity.size());
		for (currIndex = 0; currIndex < size; currIndex++) {
			Vec3 cameraPosition = cameras->getPosition();

			Quaternion orientation = glm::conjugate(glm::toQuat(glm::lookAt(cameraPosition, Vec3(0,0,1), Vec3(0,1,0))));
		}
	}

	/* Definition des variables utiles */
	SYSTEM_USE_DATA(rotation, transforms, rotation, Quaternion);
	SYSTEM_USE_DATA(position, transforms, position, Vec3);

	/* Initialisation relative a la scene parente */
	virtual void Initialize(ecs::Scene& scene) override {
		transforms = scene.GetComponent<ecs::core::Transform>();
		cameraControllers = scene.GetComponent<CameraPlayer>();
		cameras = scene.GetComponent<ecs::core::Camera>();
		SetDependencies(transforms, cameraControllers, cameras);
	}
};