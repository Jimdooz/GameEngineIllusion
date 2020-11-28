#include "ecs/Component.h"

namespace illusion::ecs {
	void Component::AddComponent(entity_id id) {
		ToData[id::Index(id)] = component_id{ static_cast<u32>(ToEntity.size()) };
		AddComponentData(ToEntity, id);
		AddComponentDatas(id);
	}
	void Component::RemoveComponent(entity_id id) {
		const entity_id getId{ id::Index(id) };
		const entity_id lastId{ id::Index(ToEntity.back()) };
		const entity_id index{ ToData[getId] };

		RemoveComponentData(ToEntity, index);
		RemoveComponentDatas(index);
		ToData[lastId] = component_id{ index };
		ToData[getId] = component_id{ id::invalid_id };
	}

	void Component::CreateEntity(entity_id id) {
		id::id_type index = id::Index(id);
		if (index >= ToData.size()) ToData.resize(index + 1);
		ToData[index] = component_id{ id::invalid_id };
	}
	void Component::DestroyEntity(entity_id id) {
		id::id_type index = id::Index(id);
		if (ToData[index] != component_id{ id::invalid_id }) RemoveComponent(id);
		ToData[index] = component_id{ id::invalid_id };
	}
}