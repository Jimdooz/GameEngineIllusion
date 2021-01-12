#include "core/rendering/CoreComponents/PostProcess.h"

namespace illusion::core::rendering {

	void PostProcess::AddDatas(ecs::entity_id id) {
		AddData<boolean>(hdrEffect, false);
		AddData(exposure, 1.0f);
		AddData(gamma, 1.0f);

		AddData<boolean>(shadowEffect, false);
		AddData(shadowIntensity, 1.0f);
		AddData(shadowSmooth, 1);

		AddData<boolean>(bloomEffect, false);
		AddData(bloomIntensity, 1.0f);
		AddData<u32>(bloomDiffusion, 1);
		AddData(bloomExpansion, 1.0f);
		AddData(bloomThreshold, 1.0f);
	}

	void PostProcess::RemoveDatas(ecs::component_id index, ecs::entity_id id) {
		RemoveData(index, shadowEffect, shadowIntensity, shadowSmooth);
		RemoveData(index, hdrEffect, exposure, gamma);
		RemoveData(index, bloomEffect, bloomIntensity, bloomExpansion, bloomThreshold, bloomDiffusion);
	}

}
