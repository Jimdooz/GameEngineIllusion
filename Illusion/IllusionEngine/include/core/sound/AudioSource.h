#pragma once
#include "ecs/Component.h"

namespace illusion::core::sound {

	using namespace illusion::ecs;

	struct AudioSource : public Component {
		COMPONENT_NAME("AudioSource");
		COMPONENT_REGISTER(AudioSource);

		AudioSource(Scene* scene) : Component(scene) {
			COMPONENT_PROTECTED(sourcePath);
			COMPONENT_PROTECTED(is3D);
			COMPONENT_PROTECTED(loop);
			COMPONENT_PROTECTED(paused);
			COMPONENT_PROTECTED(volume);
		}
		COMPONENT_DATA(std::string, sourcePath);
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
