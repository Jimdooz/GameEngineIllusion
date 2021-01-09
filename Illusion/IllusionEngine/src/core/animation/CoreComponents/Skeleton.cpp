#include "core/animation/CoreComponents/Skeleton.h"

namespace illusion::core::animation {

	void Skeleton::AddDatas(ecs::entity_id id) {
		AddData(bones, util::Array<Bone>());
		AddData<boolean>(idsComputed, false);
	}

	void Skeleton::RemoveDatas(ecs::component_id index, ecs::entity_id id) {
		RemoveData(index,bones,idsComputed);
	}

}