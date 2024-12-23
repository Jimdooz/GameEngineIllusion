#include "ecs/Component.h"
#include "ecs/Scene.h"

#include "ecs/CoreComponents/Camera.h"

#include "core/physics/components/RigidBody.h"
#include "core/physics/components/BoxCollider.h"
#include "core/physics/components/SphereCollider.h"

#include "core/rendering/CoreComponents/directionalLight.h"
#include "core/rendering/CoreComponents/pointLight.h"
#include "core/rendering/CoreComponents/PostProcess.h"

#include "core/animation/CoreComponents/Animator.h"
#include "core/animation/CoreComponents/Skeleton.h"

#include "core/sound/AudioSource.h"

using namespace illusion::core;

namespace illusion::ecs {
	util::Map<size_t, Component*> Component::AllComponents;

	Component::Component(Scene* scene) {
		this->scene = scene;
	}

	void Component::UseComponent(entity_id id) {
		if (ToData[id::Index(id)] != ecs::id::invalid_id) return;
		ToData[id::Index(id)] = component_id{ static_cast<u32>(ToEntity.size()) };
		AddData(ToEntity, id);
		AddDatas(id);
	}

	void Component::RemoveComponent(entity_id id) {
		const entity_id getId{ id::Index(id) };
		component_id index{ ToData[getId] };

		if (index == id::invalid_id) return;

		// On supprime toutes les données liées � l'id
		RemoveDatas(index, id);

		//// Inversion pour tableau compact

		// On r�cup�re le nouvel index qui a p� �tre modifi� lors de la fonction pr�c�dente
		// Ainsi que le dernier index de ToEntity pour l'�change
		component_id index_post{ ToData[getId] };
		entity_id lastId{ id::Index(ToEntity.back()) };

		// On supprime la position vers ToEntity
		RemoveData(ToEntity, index_post);

		// On proc�de � l'�change
		ToData[lastId] = component_id{ index_post };
		ToData[getId] = component_id{ id::invalid_id };

		AfterRemoveComponent(id);
	}

	void Component::AfterRemoveComponent(entity_id id){}

	void Component::OnEntityCreate(entity_id id) {
		id::id_type index = id::Index(id);
		if (index >= ToData.size()) ToData.resize(static_cast<size_t>(index) + 1);
		ToData[index] = component_id{ id::invalid_id };
	}

	void Component::OnEntityDestroy(entity_id id) {
		id::id_type index = id::Index(id);
		if (ToData[index] != component_id{ id::invalid_id }) RemoveComponent(id);
		ToData[index] = component_id{ id::invalid_id };
	}

	void Component::AppendCoreComponents() {
		Component::AppendComponents<core::Transform>();
		Component::AppendComponents<core::Camera>();

		Component::AppendComponents<physics::RigidBody>();
		Component::AppendComponents<physics::BoxCollider>();
		Component::AppendComponents<physics::SphereCollider>();

		Component::AppendComponents<MeshInstance>();

		Component::AppendComponents<rendering::DirectionalLight>();
		Component::AppendComponents<rendering::PointLight>();
		Component::AppendComponents<rendering::PostProcess>();

		Component::AppendComponents<animation::Animator>();
		Component::AppendComponents<animation::Skeleton>();

		Component::AppendComponents<sound::AudioSource>();
	}
}
