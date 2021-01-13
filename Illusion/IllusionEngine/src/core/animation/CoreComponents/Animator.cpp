#include "core/animation/CoreComponents/Animator.h"

namespace illusion::core::animation {

	void Animator::AddDatas(ecs::entity_id id) {
		AddData(animator, AnimatorElement());
	}

	void Animator::RemoveDatas(ecs::component_id index, ecs::entity_id id) {
		RemoveData(animator, index);
	}

}
