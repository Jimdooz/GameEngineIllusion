#include "core/physics/PhysicsManager.h"

#include "core/physics/components/RigidBody.h"
#include "core/physics/components/BoxCollider.h"
#include "core/physics/components/SphereCollider.h"

namespace illusion::core::physics {

	using namespace illusion::core::physics::collisions;
	using namespace illusion::core::physics::primitives;

	void ComputePhysics(ecs::Scene& scene) {
		util::Array<ecs::entity_id> colliders1;
		util::Array<ecs::entity_id> colliders2;
		util::Array<CollisionManifold> results;

		f32 LinearProjectionPercent = 0.9f;
		f32 PenetrationSlack = 0.01f;
		int ImpulseIteration = 7;

		RigidBody* rigidbodies = scene.GetComponent<RigidBody>();
		BoxCollider* boxColliders = scene.GetComponent<BoxCollider>();
		SphereCollider* sphereColliders = scene.GetComponent<SphereCollider>();

		colliders1.reserve((boxColliders->ToEntity.size() - 1) * (boxColliders->ToEntity.size() - 1));
		colliders2.reserve((boxColliders->ToEntity.size() - 1) * (boxColliders->ToEntity.size() - 1));

		for (int i = 0, size = boxColliders->size(); i < size; ++i) {
			for (int j = i + 1; j < size; ++j) {
				CollisionManifold result;
				ResetCollisionManifold(&result);

				ecs::entity_id A = boxColliders->getId((ecs::component_id)i);
				ecs::entity_id B = boxColliders->getId((ecs::component_id)j);

				if (rigidbodies->exist(A) && rigidbodies->exist(B)) {
					result = FindCollisionFeatures(boxColliders->GetBox(A), boxColliders->GetBox(B));

					if (result.colliding) {
						colliders1.push_back(A);
						colliders2.push_back(B);
						results.push_back(result);
					}
				}

			}
		}


		// Calculate foces acting on the object
		for (int i = 0, size = rigidbodies->size(); i < size; ++i) {
			rigidbodies->ApplyForces(rigidbodies->getId((ecs::component_id)i));
		}

		for (int k = 0; k < ImpulseIteration; ++k) {
			for (int i = 0; i < results.size(); ++i) {
				int jSize = results[i].contacts.size();
				for (int j = 0; j < jSize; ++j) {
					ApplyImpulse(scene, colliders1[i], colliders2[i], results[i], j);
				}
			}
		}


		for (int i = 0, size = rigidbodies->size(); i < size; ++i) {
			rigidbodies->Update(rigidbodies->getId((ecs::component_id)i));
		}

		for (int i = 0, size = results.size(); i < size; ++i) {
			ecs::entity_id A = colliders1[i];
			ecs::entity_id B = colliders2[i];
			f32 totalMass = rigidbodies->InvMass(A) + rigidbodies->InvMass(B);
			if (totalMass == 0.0f) continue;

			float depth = fmaxf(results[i].depth - PenetrationSlack, 0.0f);
			float scalar = depth / totalMass;
			Vec3 correction = results[i].normal * scalar * LinearProjectionPercent;

			ecs::core::Transform* transform = scene.GetComponent<ecs::core::Transform>();
			Vec3& positionA = transform->position[transform->getIndex(A)];
			Vec3& positionB = transform->position[transform->getIndex(B)];

			if(!rigidbodies->fixed[rigidbodies->getIndex(A)]) positionA = positionA - correction * rigidbodies->InvMass(A);
			if(!rigidbodies->fixed[rigidbodies->getIndex(B)]) positionB = positionB + correction * rigidbodies->InvMass(B);
		}
	}

	CollisionManifold FindCollisionFeatures(ecs::Scene& scene, ecs::entity_id A, ecs::entity_id B) {
		CollisionManifold result;
		ResetCollisionManifold(&result);

		BoxCollider* boxColliders = scene.GetComponent<BoxCollider>();
		SphereCollider* sphereColliders = scene.GetComponent<SphereCollider>();

		if (boxColliders->exist(A)) {
			if (boxColliders->exist(B)) return FindCollisionFeatures(boxColliders->GetBox(A), boxColliders->GetBox(B));
		}

		return result;
	}

	void ApplyImpulse(ecs::Scene& scene, ecs::entity_id A, ecs::entity_id B, const CollisionManifold& M, int c) {
		RigidBody* rigidbodies = scene.GetComponent<RigidBody>();
		ecs::component_id indexA = rigidbodies->getIndex(A);
		ecs::component_id indexB = rigidbodies->getIndex(B);
		// Linear Velocity
		float invMass1 = rigidbodies->InvMass(A);
		float invMass2 = rigidbodies->InvMass(B);
		float invMassSum = invMass1 + invMass2;
		if (invMassSum == 0.0f) { return; }
		// Relative velocity
		Vec3 relativeVel = rigidbodies->velocity[indexB] - rigidbodies->velocity[indexA];
		// Relative collision normal
		Vec3 relativeNorm = M.normal;
		glm::normalize(relativeNorm);
		// Moving away from each other? Do nothing!
		if (glm::dot(relativeVel, relativeNorm) > 0.0f) {
			return;
		}
		float e = fminf(rigidbodies->cor[indexA], rigidbodies->cor[indexB]);
		float numerator = (-(1.0f + e) * glm::dot(relativeVel, relativeNorm));
		float j = numerator / invMassSum;
		if (M.contacts.size() > 0.0f && j != 0.0f) {
			j /= (float)M.contacts.size();
		}
		Vec3 impulse = relativeNorm * j;

		rigidbodies->velocity[indexA] = rigidbodies->velocity[indexA] - impulse * invMass1;
		rigidbodies->velocity[indexB] = rigidbodies->velocity[indexB] + impulse * invMass2;
		// Friction
		Vec3 t = relativeVel - (relativeNorm * glm::dot(relativeVel, relativeNorm));
		if (CMP(glm::length2(t), 0.0f)) {
			return;
		}
		glm::normalize(t);
		numerator = -glm::dot(relativeVel, t);
		float jt = numerator / invMassSum;
		if (M.contacts.size() > 0.0f && jt != 0.0f) {
			jt /= (float)M.contacts.size();
		}
		if (CMP(jt, 0.0f)) {
			return;
		}
		float friction = sqrtf(rigidbodies->friction[indexA] * rigidbodies->friction[indexB]);
		if (jt > j * friction) {
			jt = j * friction;
		}
		else if (jt < -j * friction) {
			jt = -j * friction;
		}

		Vec3 tangentImpuse = t * jt;

		rigidbodies->velocity[indexA] = rigidbodies->velocity[indexA] - tangentImpuse * invMass1;
		rigidbodies->velocity[indexB] = rigidbodies->velocity[indexB] + tangentImpuse * invMass2;
	}

}