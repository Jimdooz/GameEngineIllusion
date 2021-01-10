#include "core/rendering/CoreComponents/PostProcess.h"

namespace illusion::core::rendering {

	void PostProcess::AddDatas(ecs::entity_id id) {
		AddData(exposure, 1.0f);
		AddData(gamma, 1.0f);

		AddData<boolean>(bloomEffect, false);
		AddData(bloomIntensity, 1.0f);
		AddData<u32>(bloomDiffusion, 1);
		AddData(bloomExpansion, 1.0f);
		AddData(bloomThreshold, 1.0f);
	}

	void PostProcess::RemoveDatas(ecs::component_id index, ecs::entity_id id) {
		RemoveData(index, exposure, gamma);
		RemoveData(index, bloomEffect, bloomIntensity, bloomExpansion, bloomThreshold, bloomDiffusion);
	}

}
