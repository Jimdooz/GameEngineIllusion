#pragma once

#include "ecs/Component.h"
#include "ecs/CoreComponents/Transform.h"

#include "./BoxCollider.h"
#include "./SphereCollider.h"

#include "ecs/Scene.h"

namespace illusion::core::physics {

	using namespace illusion;

	struct CollisionRigidBody {
		ecs::entity_id id;
		Vec3 normal;
	};

	struct RigidBody : public ecs::Component {
		// Declare component name
		COMPONENT_NAME("RigidBody");
		COMPONENT_REGISTER(RigidBody);

		// Declare constructor
		RigidBody(ecs::Scene* scene) : Component(scene) {
			// Display on inspector
			COMPONENT_PUBLIC(fixed);
			COMPONENT_PUBLIC(fixedRotationX);
			COMPONENT_PUBLIC(fixedRotationY);
			COMPONENT_PUBLIC(fixedRotationZ);

			COMPONENT_PUBLIC(gravity);
			COMPONENT_PUBLIC(mass);
			COMPONENT_PUBLIC(cor);
			COMPONENT_PUBLIC(friction);
		}

		COMPONENT_DATA(boolean, fixed);

		COMPONENT_DATA(boolean, fixedRotationX);
		COMPONENT_DATA(boolean, fixedRotationY);
		COMPONENT_DATA(boolean, fixedRotationZ);

		//Linear Velocity
		COMPONENT_DATA(Vec3, velocity);
		COMPONENT_DATA(Vec3, forces);
		COMPONENT_DATA(f32, mass);
		COMPONENT_DATA(f32, cor); //Coefficient of restitution
		COMPONENT_DATA(f32, friction);

		COMPONENT_DATA(Vec3, gravity);

		//Angular Velocity
		COMPONENT_DATA(Vec3, angVel);
		COMPONENT_DATA(Vec3, torques);
		COMPONENT_DATA(Mat4x4, invTensor);

		//Calculation
		COMPONENT_DATA(Vec3, n_velocity); // Next velocity
		COMPONENT_DATA(Vec3, dt_velocity);

		//Informations
		COMPONENT_DATA(util::Array<CollisionRigidBody>, collisions);

		Mat4x4 InvTensor(ecs::entity_id id) {
			ecs::component_id index = getIndex(id);

			if (invTensor[index] != Mat4x4(0)) return invTensor[index];

			BoxCollider* boxColliders = scene->GetComponent<BoxCollider>();
			SphereCollider* sphereColliders = scene->GetComponent<SphereCollider>();

			if (boxColliders->exist(id)) invTensor[index] = boxColliders->InvTensor(id, mass[index]);
			else if (sphereColliders->exist(id)) invTensor[index] = sphereColliders->InvTensor(id, mass[index]);

			return invTensor[index];
		}

		void AddRotationImpulse(ecs::entity_id id, const Vec3& point, const Vec3& impulse) {
			ecs::component_id index = getIndex(id);

			if (fixed[index]) return;

			//Get Position
			ecs::core::Transform* transform = scene->GetComponent<ecs::core::Transform>();
			Vec3& position = transform->position[transform->getIndex(id)];

			Vec3 centerOfMass = position;
			Vec3 torque = glm::cross(point - centerOfMass, impulse);

			if (fixedRotationX[index]) torque.x = 0;
			if (fixedRotationY[index]) torque.y = 0;
			if (fixedRotationZ[index]) torque.z = 0;

			torques[index] += torque;
		}

		void ApplyForces(ecs::entity_id id) {
			ecs::component_id index = getIndex(id);

			if (fixed[index]) return;

			forces[index] = gravity[index] * mass[index];
		}

		void AddLinearImpulse(ecs::entity_id id, const Vec3& impulse) {
			ecs::component_id index = getIndex(id);
			if (fixed[index]) return;

			n_velocity[index] += impulse * InvMass(id);
		}

		f32 InvMass(ecs::entity_id id) {
			ecs::component_id index = getIndex(id);

			if (mass[index] <= 0.0f) { return 0.0f; }
			if (fixed[index]) return 0.0f;
			return 1.0f / mass[index];
		}

		void ApplyCurrentVelocity(ecs::entity_id id) {
			ecs::component_id index = getIndex(id);
			if (fixed[index]) return;

			//Get Position
			ecs::core::Transform* transform = scene->GetComponent<ecs::core::Transform>();
			Vec3& position = transform->position[transform->getIndex(id)];
			Quaternion& rotation = transform->rotation[transform->getIndex(id)];

			velocity[index] += n_velocity[index];
			dt_velocity[index] += n_velocity[index];
			angVel[index] += Vec3(InvTensor(id) * Vec4(torques[index], 1.0));

			position += n_velocity[index] * Time::fixedDeltaTime;
			Quaternion wanted = glm::quat(Vec3(InvTensor(id) * Vec4(torques[index], 1.0)) * Time::fixedDeltaTime);
			Vec3 euler = glm::degrees(glm::eulerAngles(wanted));
			if (fixedRotationX[index]) euler.x = 0;
			if (fixedRotationY[index]) euler.y = 0;
			if (fixedRotationZ[index]) euler.z = 0;
			wanted = glm::tquat(glm::radians(euler));
			rotation = wanted * rotation;

			n_velocity[index] = Vec3(0, 0, 0);
			torques[index] = Vec3(0, 0, 0);
		}

		void Update(ecs::entity_id id) {
			//Get Index
			ecs::component_id index = getIndex(id);

			if (fixed[index]) return;

			//Get Position
			ecs::core::Transform* transform = scene->GetComponent<ecs::core::Transform>();
			Vec3& position = transform->position[transform->getIndex(id)];
			Quaternion& rotation = transform->rotation[transform->getIndex(id)];

			const f32 damping = 0.98f;
			Vec3 acceleration = forces[index] * InvMass(id);
			velocity[index] += acceleration * Time::fixedDeltaTime;
			velocity[index] *= damping;

			angVel[index] *= damping;

			position += (velocity[index] - dt_velocity[index]) * Time::fixedDeltaTime;

			Quaternion wanted = glm::quat(angVel[index] * Time::fixedDeltaTime);
			Vec3 euler = glm::degrees(glm::eulerAngles(wanted));
			if (fixedRotationX[index]) euler.x = 0;
			if (fixedRotationY[index]) euler.y = 0;
			if (fixedRotationZ[index]) euler.z = 0;
			wanted = glm::tquat(glm::radians(euler));

			rotation = wanted * rotation;

			torques[index] = Vec3(0, 0, 0);
			n_velocity[index] = Vec3(0, 0, 0);
			dt_velocity[index] = Vec3(0, 0, 0);
		}

		// On Data added
		virtual void AddDatas(ecs::entity_id id) override {
			AddData<boolean>(fixed, false);
			AddData<boolean>(fixedRotationX, false);
			AddData<boolean>(fixedRotationY, false);
			AddData<boolean>(fixedRotationZ, false);

			AddData(velocity, Vec3(0, 0, 0));
			AddData(forces, Vec3(0, 0, 0));

			AddData(mass, 1.0f);
			AddData(cor, 0.0f);
			AddData(friction, 0.6f);

			AddData(gravity, Vec3(0, -9.82f, 0));

			AddData(angVel, Vec3(0, 0, 0));
			AddData(torques, Vec3(0, 0, 0));
			AddData(invTensor, Mat4x4(0));

			AddData(n_velocity, Vec3(0,0,0));
			AddData(dt_velocity, Vec3(0, 0, 0));

			AddData(collisions, util::Array<CollisionRigidBody>());
		}

		// On Data removed
		virtual void RemoveDatas(ecs::component_id index, ecs::entity_id id) {
			RemoveData(fixed, index);
			RemoveData(fixedRotationX, index);
			RemoveData(fixedRotationY, index);
			RemoveData(fixedRotationZ, index);

			RemoveData(velocity, index);
			RemoveData(forces, index);
			RemoveData(gravity, index);

			RemoveData(mass, index);
			RemoveData(cor, index);
			RemoveData(friction, index);

			RemoveData(angVel, index);
			RemoveData(torques, index);
			RemoveData(invTensor, index);

			RemoveData(n_velocity, index);
			RemoveData(dt_velocity, index);

			RemoveData(collisions, index);
		}

	};
}