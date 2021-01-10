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

	Mat4x4& Transform::ComputeModel(ecs::component_id component, bool forceRecompute) {
		if (!forceRecompute && currentTick[component] == Time::tick) return modelTransform[component];

		glm::mat4 &model = modelTransform[component];

		if (currentTick[component] == -1 || s_position[component] != position[component] || s_rotation[component] != rotation[component] || s_scale[component] != scale[component]) {
			elementTransform[component] = glm::translate(position[component]) * glm::toMat4(rotation[component]) * glm::scale(scale[component]);
			model = elementTransform[component];

			if (needUpdateEuler[component]) {
				Quaternion diff(glm::inverse(rotation[component]) * s_rotation[component]);
				rotationEuler[component] -= glm::degrees(glm::eulerAngles(diff));
			}
			needUpdateEuler[component] = true;

			s_position[component] = position[component];
			s_rotation[component] = rotation[component];
			s_scale[component] = scale[component];
		}

		if (id::IsValid(parent[component])) {
			ecs::component_id parentId = getIndex(parent[component]);
			ComputeModel(parentId);

			model = modelTransform[parentId] * elementTransform[component];
		}

		currentTick[component] = Time::tick;

		return model;
	}

	ecs::entity_id Transform::FindByName(ecs::entity_id startId, std::string path) {
		if (!id::IsValid(startId) || !scene->entities.IsAlive(startId)) return (ecs::entity_id)id::invalid_id;
		ecs::entity_id currentId = startId;
		
		std::stringstream test(path);
		std::string segment;
		std::vector<std::string> segname;

		while (std::getline(test, segment, '/')) segname.push_back(segment);


		for (size_t i = 0, size = segname.size(); i < size; i++) {
			//@Todo check if currentId is valid
			if (segname[i] == std::string(".")) {
				continue;
			}
			if (segname[i] == std::string("..")) {
				currentId = parent[getIndex(currentId)];
				continue;
			}
			util::Array<ecs::entity_id>& childsElement = childs[getIndex(currentId)];
			bool found = false;
			for (size_t j = 0; j < childsElement.size(); j++) {
				if (name[getIndex(childsElement[j])] == segname[i]) {
					currentId = childsElement[j];
					found = true;
					break;
				}
			}
			if(!found) return (ecs::entity_id)id::invalid_id;
		}

		return currentId;
	}

	void Transform::AddDatas(ecs::entity_id id) {
		AddData(currentTick, u8(-1));
		AddData(name, "Entity " + std::to_string(ecs::id::Index(id)) + " [" + std::to_string(ecs::id::Generation(id)) + "]");

		AddData(position, Vec3(0, 0, 0));
		AddData(rotation, Quaternion(1,0,0,0));
		AddData(scale, Vec3(1, 1, 1));

		AddData(rotationEuler, Vec3(0, 0, 0));
		AddData<boolean>(needUpdateEuler, false);

		AddData(s_position, Vec3(0, 0, 0));
		AddData(s_rotation, Quaternion(1, 0, 0, 0));
		AddData(s_scale, Vec3(1, 1, 1));

		AddData(elementTransform, Mat4x4(1.0));
		AddData(modelTransform, Mat4x4(1.0));

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

		RemoveData(currentTick, index);
		RemoveData(name, index);

		RemoveData(position, index);
		RemoveData(rotation, index);
		RemoveData(scale, index);

		RemoveData(rotationEuler, index);
		RemoveData(needUpdateEuler, index);

		RemoveData(s_position, index);
		RemoveData(s_rotation, index);
		RemoveData(s_scale, index);

		RemoveData(elementTransform, index);
		RemoveData(modelTransform, index);

		RemoveData(parent, index);
		RemoveData(childs, index);
	}

	void Transform::AfterRemoveComponent(entity_id id) {
		scene->DestroyEntity(id);
	}
}
