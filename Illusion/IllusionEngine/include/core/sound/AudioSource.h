#pragma once
#include "ecs/Component.h"
#include "irrKlang.h"

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
		COMPONENT_DATA(irrklang::ISound*, sound_ptr);
		COMPONENT_DATA(boolean, is3D);
		COMPONENT_DATA(boolean, loop);
		COMPONENT_DATA(boolean, paused);
		COMPONENT_DATA(f32, volume);

		COMPONENT_DATA(u32, playPosition);

		COMPONENT_DATA(std::string, old_relativePath);
		COMPONENT_DATA(boolean, old_is3D);

		inline void FreeCurrentSound(ecs::component_id index) {
			if (sound_ptr[index] != nullptr) {
				sound_ptr[index]->stop(); //Stop secure
				sound_ptr[index]->drop(); //Free memory
			}
			sound_ptr[index] = nullptr;
		}

		inline void Play(ecs::component_id index) {
			if (sound_ptr[index] == nullptr) return;
			if (sound_ptr[index]->getPlayPosition() >= sound_ptr[index]->getPlayLength()) Restart(index);
			paused[index] = false;
			sound_ptr[index]->setPlayPosition(0);
			if (paused[index] != sound_ptr[index]->getIsPaused()) sound_ptr[index]->setIsPaused(paused[index]);
			INTERNAL_INFO(sound_ptr[index]);

		}
		inline void Pause(ecs::component_id index) {
			if (sound_ptr[index] == nullptr) return;
			paused[index] = true;
			if (paused[index] != sound_ptr[index]->getIsPaused()) sound_ptr[index]->setIsPaused(paused[index]);

		}
		inline void Restart(ecs::component_id index) {
			if (sound_ptr[index] == nullptr) return;
			sound_ptr[index]->setPlayPosition(0);
			paused[index] = false;
			if (paused[index] != sound_ptr[index]->getIsPaused()) sound_ptr[index]->setIsPaused(paused[index]);

		}

		virtual void AddDatas(ecs::entity_id id) override;
		virtual void RemoveDatas(ecs::component_id index, ecs::entity_id id) override;
	};

}
