#include "ecs/Component.h"
#include "ecs/Scene.h"

namespace illusion::ecs {

	Component::Component(Scene* scene) {
		this->scene = scene;
	}

	void Component::UseComponent(entity_id id) {
		ToData[id::Index(id)] = component_id{ static_cast<u32>(ToEntity.size()) };
		AddComponentData(ToEntity, id);
		AddComponentDatas(id);
	}

	void Component::RemoveComponent(entity_id id) {
		const entity_id getId{ id::Index(id) };
		component_id index{ ToData[getId] };

		// On supprime toutes les données liées à l'id
		RemoveComponentDatas(index, id);

		//// Inversion pour tableau compact

		// On récupère le nouvel index qui a pû être modifié lors de la fonction précédente
		// Ainsi que le dernier index de ToEntity pour l'échange
		component_id index_post{ ToData[getId] };
		entity_id lastId{ id::Index(ToEntity.back()) };

		// On supprime la position vers ToEntity
		RemoveComponentData(ToEntity, index_post);

		// On procède à l'échange
		ToData[lastId] = component_id{ index_post };
		ToData[getId] = component_id{ id::invalid_id };
	}

	void Component::OnEntityCreate(entity_id id) {
		id::id_type index = id::Index(id);
		if (index >= ToData.size()) ToData.resize(index + 1);
		ToData[index] = component_id{ id::invalid_id };
	}

	void Component::OnEntityDestroy(entity_id id) {
		id::id_type index = id::Index(id);
		if (ToData[index] != component_id{ id::invalid_id }) RemoveComponent(id);
		ToData[index] = component_id{ id::invalid_id };
	}
}