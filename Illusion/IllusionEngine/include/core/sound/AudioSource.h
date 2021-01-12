#pragma once
#include "ecs/Component.h"

namespace illusion::core::sound {

	using namespace illusion::ecs;

	struct AudioSource : public Component {
		COMPONENT_NAME("AudioSource");
		COMPONENT_REGISTER(AudioSource);

		AudioSource(Scene* scene) : Component(scene) {
			COMPONENT_PUBLIC(relativePath);
			COMPONENT_PUBLIC(is3D);
			COMPONENT_PUBLIC(loop);
			COMPONENT_PUBLIC(paused);
			COMPONENT_PUBLIC(volume);
		}
		COMPONENT_DATA(std::string, relativePath);
		COMPONENT_DATA(size_t, sound_id);
		COMPONENT_DATA(boolean, idComputed);
		COMPONENT_DATA(boolean, is3D);
		COMPONENT_DATA(boolean, loop);
		COMPONENT_DATA(boolean, paused);
		COMPONENT_DATA(f32, volume);

		virtual void AddDatas(ecs::entity_id id) override;
		virtual void RemoveDatas(ecs::component_id index, ecs::entity_id id) override;
	};

}
