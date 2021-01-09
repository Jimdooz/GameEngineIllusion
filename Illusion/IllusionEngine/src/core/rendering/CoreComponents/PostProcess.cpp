#include "core/rendering/CoreComponents/PostProcess.h"

namespace illusion::core::rendering {

	void PostProcess::AddDatas(ecs::entity_id id) {
		AddData<boolean>(bloomEffect, false);
		AddData(bloomIntensity, 1.0f);
		AddData(bloomThreshold, 1.0f);
		AddData(bloomDiffusion, 1.0f);
	}

	void PostProcess::RemoveDatas(ecs::component_id index, ecs::entity_id id) {
		RemoveData(index, bloomEffect, bloomIntensity, bloomThreshold, bloomDiffusion);
	}

}
