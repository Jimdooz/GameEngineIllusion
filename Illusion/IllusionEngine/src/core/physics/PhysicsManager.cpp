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

		f32 LinearProjectionPercent = 0.8f;
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

		glPointSize(10);

		for (int k = 0; k < ImpulseIteration; ++k) {
			for (int i = 0; i < results.size(); ++i) {
				int jSize = results[i].contacts.size();
				for (int j = 0; j < jSize; ++j) {
					ApplyImpulse(scene, colliders1[i], colliders2[i], results[i], j);
				}
				rigidbodies->ApplyCurrentVelocity(colliders1[i]);
				rigidbodies->ApplyCurrentVelocity(colliders2[i]);
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
		// Helper
		RigidBody* rigidbodies = scene.GetComponent<RigidBody>();
		ecs::component_id indexA = rigidbodies->getIndex(A);
		ecs::component_id indexB = rigidbodies->getIndex(B);

		ecs::core::Transform* transforms = scene.GetComponent<ecs::core::Transform>();
		ecs::component_id indexTA = transforms->getIndex(A);
		ecs::component_id indexTB = transforms->getIndex(B);

		// Linear Velocity
		float invMass1 = rigidbodies->InvMass(A);
		float invMass2 = rigidbodies->InvMass(B);
		float invMassSum = invMass1 + invMass2;

		if (invMassSum == 0.0f) return;

		Vec3 r1 = M.contacts[c] - transforms->position[indexTA];
		Vec3 r2 = M.contacts[c] - transforms->position[indexTB];

		Mat4x4 i1 = rigidbodies->InvTensor(A);
		Mat4x4 i2 = rigidbodies->InvTensor(B);

		// Relative velocity
		Vec3 relativeVel = (rigidbodies->velocity[indexB] + glm::cross(rigidbodies->angVel[indexB], r2)) - (rigidbodies->velocity[indexA] + glm::cross(rigidbodies->angVel[indexA], r1));
		// Relative collision normal
		Vec3 relativeNorm = glm::normalize(M.normal);
		// Moving away from each other? Do nothing!
		if (glm::dot(relativeVel, relativeNorm) > 0.0f) return;

		// Compute the impulse needed
		float e = fmaxf(rigidbodies->cor[indexA], rigidbodies->cor[indexB]);
		float numerator = (-(1.0f + e) * glm::dot(relativeVel, relativeNorm));

		float d1 = invMassSum;
		Vec3 d2 = glm::cross(Vec3(i1 * Vec4(glm::cross(r1, relativeNorm), 1.0)), r1);
		Vec3 d3 = glm::cross(Vec3(i2 * Vec4(glm::cross(r2, relativeNorm), 1.0)), r2);
		float denominator = d1 + glm::dot(relativeNorm, d2 + d3);
		//INTERNAL_INFO(e, " ", numerator, " ", d1, " ", d2, " ", d3, " ", denominator);

		float j = (denominator == 0.0f) ? 0.0f : numerator / denominator;
		if (M.contacts.size() > 0.0f && j != 0.0f) {
			j /= (float)M.contacts.size();
		}
		Vec3 impulse = relativeNorm * j;

		rigidbodies->AddLinearImpulse(A, -impulse);
		rigidbodies->AddLinearImpulse(B, impulse);

		rigidbodies->AddRotationImpulse(A, M.contacts[c], -impulse);
		rigidbodies->AddRotationImpulse(B, M.contacts[c], impulse);

		// Friction
		Vec3 t = relativeVel - (relativeNorm * glm::dot(relativeVel, relativeNorm));
		if (CMP(glm::length2(t), 0.0f)) return;
		t = glm::normalize(t);
		numerator = -glm::dot(relativeVel, t);

		d1 = invMassSum;
		d2 = glm::cross(Vec3(i1 * Vec4(glm::cross(r1, t), 1.0)), r1);
		d3 = glm::cross(Vec3(i2 * Vec4(glm::cross(r2, t), 1.0)), r2);
		denominator = d1 + glm::dot(t, d2 + d3);

		if (denominator == 0.0f) return;

		float jt = numerator / denominator;
		if (M.contacts.size() > 0.0f && jt != 0.0f) {
			jt /= (float)M.contacts.size();
		}
		if (CMP(jt, 0.0f)) return;

		float friction = sqrtf(rigidbodies->friction[indexA] * rigidbodies->friction[indexB]);
		if (jt > j * friction) {
			jt = j * friction;
		}
		else if (jt < -j * friction) {
			jt = -j * friction;
		}

		Vec3 tangentImpuse = t * jt;

		rigidbodies->AddLinearImpulse(A, -tangentImpuse);
		rigidbodies->AddLinearImpulse(B, tangentImpuse);

		rigidbodies->AddRotationImpulse(A, M.contacts[c], -tangentImpuse);
		rigidbodies->AddRotationImpulse(B, M.contacts[c], tangentImpuse);
	}

}