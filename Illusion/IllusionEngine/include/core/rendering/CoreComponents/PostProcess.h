#pragma once

#include "ecs/Component.h"

namespace illusion::core::rendering {

	using namespace illusion::ecs;

	struct PostProcess : public Component {
		PostProcess(Scene* scene) : Component(scene) {
			COMPONENT_PUBLIC(bloomEffect);
			COMPONENT_PUBLIC(bloomIntensity);
			COMPONENT_PUBLIC(bloomThreshold);
			COMPONENT_PUBLIC(bloomDiffusion);
		}

		COMPONENT_NAME("Post Processing");
		COMPONENT_REGISTER(PostProcess);

		//Bloom
		COMPONENT_DATA(boolean, bloomEffect);
		COMPONENT_DATA(f32, bloomIntensity);
		COMPONENT_DATA(f32, bloomThreshold);
		COMPONENT_DATA(f32, bloomDiffusion);

		virtual void AddDatas(ecs::entity_id id) override;
		virtual void RemoveDatas(ecs::component_id index, ecs::entity_id id) override;
	};

}
