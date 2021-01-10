#pragma once

#include "ecs/Component.h"

namespace illusion::core::rendering {

	using namespace illusion::ecs;

	struct PostProcess : public Component {
		PostProcess(Scene* scene) : Component(scene) {
			COMPONENT_PUBLIC(exposure);
			COMPONENT_PUBLIC(gamma);

			COMPONENT_PUBLIC(bloomEffect);
			COMPONENT_PUBLIC(bloomIntensity);
			COMPONENT_PUBLIC(bloomDiffusion);
			COMPONENT_PUBLIC(bloomExpansion);
			COMPONENT_PUBLIC(bloomThreshold);
		}

		COMPONENT_NAME("Post Processing");
		COMPONENT_REGISTER(PostProcess);

		COMPONENT_DATA(f32, exposure);
		COMPONENT_DATA(f32, gamma);

		//Bloom
		COMPONENT_DATA(boolean, bloomEffect);
		COMPONENT_DATA(f32, bloomIntensity);
		COMPONENT_DATA(u32, bloomDiffusion);
		COMPONENT_DATA(f32, bloomExpansion);
		COMPONENT_DATA(f32, bloomThreshold);

		virtual void AddDatas(ecs::entity_id id) override;
		virtual void RemoveDatas(ecs::component_id index, ecs::entity_id id) override;
	};

}
