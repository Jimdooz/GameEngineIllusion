#include "ecs/Component.h"
#include "ecs/Scene.h"

namespace illusion::ecs {

	Component::Component(Scene* scene) {
		this->scene = scene;
	}

	void Component::UseComponent(entity_id id) {
		ToData[id::Index(id)] = component_id{ static_cast<u32>(ToEntity.size()) };
		AddData(ToEntity, id);
		AddDatas(id);
	}

	void Component::RemoveComponent(entity_id id) {
		const entity_id getId{ id::Index(id) };
		component_id index{ ToData[getId] };

		if (index == id::invalid_id) return;

		// On supprime toutes les donn�es li�es � l'id
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
		if (index >= ToData.size()) ToData.resize(index + 1);
		ToData[index] = component_id{ id::invalid_id };
	}

	void Component::OnEntityDestroy(entity_id id) {
		id::id_type index = id::Index(id);
		if (ToData[index] != component_id{ id::invalid_id }) RemoveComponent(id);
		ToData[index] = component_id{ id::invalid_id };
	}
}
