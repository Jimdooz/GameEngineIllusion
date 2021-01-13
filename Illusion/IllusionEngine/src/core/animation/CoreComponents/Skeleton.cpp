#include "core/animation/CoreComponents/Skeleton.h"

namespace illusion::core::animation {

	void Skeleton::AddDatas(ecs::entity_id id) {
		AddData<boolean>(idsComputed, false);
		AddData(parentRelativePath, std::string());
		AddData(parentId,(ecs::entity_id) ecs::id::invalid_id);
		AddData(bones, util::Array<Bone>());
	}

	void Skeleton::RemoveDatas(ecs::component_id index, ecs::entity_id id) {
		RemoveData(idsComputed, index);
		RemoveData(parentRelativePath, index);
		RemoveData(parentId, index);
		RemoveData(bones,index);
	}

}