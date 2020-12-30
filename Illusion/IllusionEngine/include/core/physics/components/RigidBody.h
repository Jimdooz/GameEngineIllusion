#pragma once

#include "ecs/Component.h"
#include "ecs/CoreComponents/Transform.h"

#include "ecs/Scene.h"

namespace illusion::core::physics {

	using namespace illusion;

	struct RigidBody : public ecs::Component {
		// Declare component name
		COMPONENT_NAME("RigidBody");
		COMPONENT_REGISTER(RigidBody);

		// Declare constructor
		RigidBody(ecs::Scene* scene) : Component(scene) {
			// Display on inspector
			COMPONENT_PUBLIC(fixed);
			COMPONENT_PUBLIC(gravity);
			COMPONENT_PUBLIC(velocity);
			COMPONENT_PUBLIC(forces);
			COMPONENT_PUBLIC(mass);
		}

		COMPONENT_DATA(boolean, fixed);

		COMPONENT_DATA(Vec3, velocity);
		COMPONENT_DATA(Vec3, forces);
		COMPONENT_DATA(f32, mass);
		COMPONENT_DATA(f32, cor); //Coefficient of restitution
		COMPONENT_DATA(f32, friction);

		COMPONENT_DATA(Vec3, gravity);

		void ApplyForces(ecs::entity_id id) {
			ecs::component_id index = getIndex(id);

			if(!fixed[index])
				forces[index] = gravity[index] * mass[index];
		}

		void AddLinearImpulse(ecs::entity_id id, const Vec3& impulse) {
			ecs::component_id index = getIndex(id);

			velocity[index] = velocity[index] + impulse;
		}

		f32 InvMass(ecs::entity_id id) {
			ecs::component_id index = getIndex(id);

			if (mass[index] == 0.0f) { return 0.0f; }
			if (fixed[index]) return 0.0f;
			return 1.0f / mass[index];
		}

		void Update(ecs::entity_id id) {
			//Get Index
			ecs::component_id index = getIndex(id);

			if (fixed[index]) return;

			//Get Position
			ecs::core::Transform* transform = scene->GetComponent<ecs::core::Transform>();
			Vec3& position = transform->position[transform->getIndex(id)];

			const f32 damping = 0.98f;
			Vec3 acceleration = forces[index] * InvMass(id);
			velocity[index] = velocity[index] + acceleration * Time::fixedDeltaTime;
			velocity[index] = velocity[index] * damping;
			position = position + velocity[index] * Time::fixedDeltaTime;
		}

		// On Data added
		virtual void AddDatas(ecs::entity_id id) override {
			AddData<boolean>(fixed, false);

			AddData(velocity, Vec3(0, 0, 0));
			AddData(forces, Vec3(0, 0, 0));

			AddData(mass, 1.0f);
			AddData(cor, 0.5f);
			AddData(friction, 0.6f);

			AddData(gravity, Vec3(0, -9.82f, 0));
		}

		// On Data removed
		virtual void RemoveDatas(ecs::component_id index, ecs::entity_id id) {
			RemoveData(fixed, index);

			RemoveData(velocity, index);
			RemoveData(forces, index);

			RemoveData(mass, index);
			RemoveData(cor, index);
			RemoveData(friction, index);

			RemoveData(gravity, index);
		}

	};
}