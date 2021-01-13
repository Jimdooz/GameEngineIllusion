#pragma once

#include "ecs/Component.h"

namespace illusion::core::rendering {

	using namespace illusion::ecs;

	struct PostProcess : public Component {
		PostProcess(Scene* scene) : Component(scene) {
			COMPONENT_PUBLIC(hdrEffect);
			COMPONENT_PUBLIC(exposure);
			COMPONENT_PUBLIC(gamma);

			COMPONENT_PUBLIC(shadowEffect);
			COMPONENT_PUBLIC(shadowIntensity);
			COMPONENT_PUBLIC(shadowSmooth);
			COMPONENT_PUBLIC(shadowWidth);
			COMPONENT_PUBLIC(shadowHeight);
			COMPONENT_PUBLIC(shadowQuality);

			COMPONENT_PUBLIC(bloomEffect);
			COMPONENT_PUBLIC(bloomIntensity);
			COMPONENT_PUBLIC(bloomDiffusion);
			COMPONENT_PUBLIC(bloomExpansion);
			COMPONENT_PUBLIC(bloomThreshold);
		}

		COMPONENT_NAME("Post Processing");
		COMPONENT_REGISTER(PostProcess);

		//Hdr
		COMPONENT_DATA(boolean, hdrEffect);
		COMPONENT_DATA(f32, exposure);
		COMPONENT_DATA(f32, gamma);

		//Shadow
		COMPONENT_DATA(boolean, shadowEffect);
		COMPONENT_DATA(f32, shadowIntensity);
		COMPONENT_DATA(i32, shadowSmooth);
		COMPONENT_DATA(f32, shadowWidth);
		COMPONENT_DATA(f32, shadowHeight);
		COMPONENT_DATA(u32, shadowQuality);

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
