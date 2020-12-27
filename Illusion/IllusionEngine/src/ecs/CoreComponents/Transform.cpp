#include "ecs/CoreComponents/Transform.h"
#include "ecs/Scene.h"

namespace illusion::ecs::core {

	void Transform::SetParent(ecs::entity_id id, ecs::entity_id parentId) {
		// If id or parentId not valid -> Stop
		if (!illusion::ecs::id::IsValid(id)) return;
		if (!illusion::ecs::id::IsValid(parentId)) {
			RemoveParent(id);
			return;
		}
		if (HaveParentRecursive(parentId, id)) return;

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
		parent[indexId] = parentId;
	}

	void Transform::RemoveParent(ecs::entity_id id) {
		if (!illusion::ecs::id::IsValid(id)) return;

		illusion::ecs::component_id indexId = getIndex(id);

		if (!illusion::ecs::id::IsValid(indexId)) return;

		ecs::entity_id parentId = parent[indexId];
		if (parentId != id::invalid_id) RemoveChild(parentId, id);

		parent[indexId] = (ecs::entity_id)id::invalid_id;
	}

	void Transform::SetChild(ecs::entity_id id, ecs::entity_id childId) {
		// If id or childId not valid -> Stop
		if (!illusion::ecs::id::IsValid(id) || !illusion::ecs::id::IsValid(childId)) return;

		illusion::ecs::component_id indexId = getIndex(id);
		illusion::ecs::component_id indexChildId = getIndex(childId);

		// If index are not valid -> Stop
		if (!illusion::ecs::id::IsValid(indexId) || !illusion::ecs::id::IsValid(indexChildId)) return;

		// Si childId n'a pas d�j� le parent en id
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

	bool Transform::HaveParentRecursive(ecs::entity_id id, ecs::entity_id parentId) {
		if (parentId == id::invalid_id || id == id::invalid_id) return false;
		if (id == parentId) return false; // Une entité ne peut pas se contenir soit m�me
		component_id index = getIndex(id);
		while (index != id::invalid_id) {
			id = parent[index];
			if (id == parentId) return true;
			if (id == id::invalid_id) return false;
			index = getIndex(id);
		}
		return false;
	}

	Mat4x4 Transform::ComputeModel(ecs::component_id component) {
		if (Time::tick != -1 && currentTick[component] == Time::tick) return computedModel[component];
		currentTick[component] = Time::tick;

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, position[component]);
		model = model * glm::toMat4(rotation[component]);
		model = glm::scale(model, scale[component]);

		if (id::IsValid(parent[component])) {
			model = ComputeModel(getIndex(parent[component])) * model;
		}

		computedModel[component] = model;

		return model;
	}

	void Transform::AddDatas(ecs::entity_id id) {
		AddData(position, Vec3(0, 0, 0));
		AddData(rotation, Quaternion(1,0,0,0));
		AddData(scale, Vec3(1, 1, 1));
		AddData(currentTick, u8(0));
		AddData(computedModel, Mat4x4(1.0f));

		AddData(parent, entity_id{ ecs::id::invalid_id });
		AddData(childs, util::Array<ecs::entity_id>());
	}

	void Transform::RemoveDatas(ecs::component_id index, ecs::entity_id id) {
		// On signale au parent que l'id a été supprimé
		RemoveChild(parent[index], id);

		// On Supprime les enfants
		for (u32 i = 0; i < childs[index].size();) {
			// On récupère l'index au cas où l'index aurait été modifié lors de la suppression d'un enfant
			scene->DestroyEntity(childs[index][i]);
			index = getIndex(id);
		}

		// On récupère l'index au cas où l'index aurait été modifié lors de la suppression d'un enfant
		index = getIndex(id);

		RemoveData(position, index);
		RemoveData(rotation, index);
		RemoveData(scale, index);
		RemoveData(currentTick, index);
		RemoveData(computedModel, index);

		RemoveData(parent, index);
		RemoveData(childs, index);
	}

	void Transform::AfterRemoveComponent(entity_id id) {
		scene->DestroyEntity(id);
	}
}
