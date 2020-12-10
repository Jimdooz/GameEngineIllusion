#include "ecs/CoreComponents/Transform.h"
#include "ecs/Scene.h"

namespace illusion::ecs::core {

	void Transform::SetParent(ecs::entity_id id, ecs::entity_id parentId) {
		// If id or parentId not valid -> Stop
		if (!illusion::ecs::id::IsValid(id) || !illusion::ecs::id::IsValid(parentId)) return;

		illusion::ecs::component_id indexId = getIndex(id);
		illusion::ecs::component_id indexParentId = getIndex(parentId);

		// If index are not valid -> Stop
		if (!illusion::ecs::id::IsValid(indexId) || !illusion::ecs::id::IsValid(indexParentId)) return;

		// 1. On signale à l'ancien parent si il existe de supprimer l'id de ses enfants
		illusion::ecs::entity_id currentParentId = parent[indexId];
		if (parentId != currentParentId
			&& illusion::ecs::id::IsValid(currentParentId)
			&& illusion::ecs::id::IsValid(getIndex(currentParentId))) {
			RemoveChild(currentParentId, id);
		}

		// 2.	On modifie le parent de l'id et on signale au parent qu'il a un nouvel enfant
		SetChild(parentId, id);
		parent[id] = parentId;
	}

	void Transform::SetChild(ecs::entity_id id, ecs::entity_id childId) {
		// If id or childId not valid -> Stop
		if (!illusion::ecs::id::IsValid(id) || !illusion::ecs::id::IsValid(childId)) return;

		illusion::ecs::component_id indexId = getIndex(id);
		illusion::ecs::component_id indexChildId = getIndex(childId);

		// If index are not valid -> Stop
		if (!illusion::ecs::id::IsValid(indexId) || !illusion::ecs::id::IsValid(indexChildId)) return;

		// Si childId n'a pas déjà le parent en id
		util::Array<ecs::entity_id>& childsId = childs[indexId];
		if(parent[indexChildId] != id){
			childsId.push_back(childId);
		}
		parent[indexChildId] = id;
	}

	void Transform::RemoveChild(ecs::entity_id id, ecs::entity_id childId) {
		// If id or childId not valid -> Stop
		if (!illusion::ecs::id::IsValid(id) || !illusion::ecs::id::IsValid(childId)) return;

		illusion::ecs::component_id indexId = getIndex(id);
		illusion::ecs::component_id indexChildId = getIndex(childId);

		// If index are not valid -> Stop
		if (!illusion::ecs::id::IsValid(indexId) || !illusion::ecs::id::IsValid(indexChildId)) return;

		util::Array<ecs::entity_id>& childsId = childs[indexId];
		std::vector<ecs::entity_id>::iterator it = std::find(childsId.begin(), childsId.end(), childId);
		if (it != childsId.end()) {
			util::EraseUnordered(childsId, std::distance(childsId.begin(), it));
		}

		parent[indexChildId] = ecs::entity_id{ id::invalid_id };
	}

	void Transform::AddComponentDatas(ecs::entity_id id) {
		AddComponentData(position, Vec3(0, 0, 0));
		AddComponentData(rotation, Quaternion(0, 0, 0, 1));
		AddComponentData(scale, Vec3(1, 1, 1));
		AddComponentData(parent, entity_id{ ecs::id::invalid_id });
		AddComponentData(childs, util::Array<ecs::entity_id>());
	}

	void Transform::RemoveComponentDatas(ecs::component_id index, ecs::entity_id id) {
		// On signale au parent que l'id a été supprimé
		RemoveChild(id, parent[index]);

		// On Supprime les enfants
		u32 nbChilds = childs[index].size();
		for (u32 i = 0; i < nbChilds; i++) {
			index = getIndex(id); // On récupère l'index au cas où l'index aurait été modifié lors de la suppression d'un enfant
			scene->DestroyEntity(childs[index][i]);
		}
		// On récupère l'index au cas où l'index aurait été modifié lors de la suppression d'un enfant
		index = getIndex(id);

		RemoveComponentData(position, index);
		RemoveComponentData(rotation, index);
		RemoveComponentData(scale, index);
		RemoveComponentData(parent, index);
		RemoveComponentData(childs, index);
	}
}