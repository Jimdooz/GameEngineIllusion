#pragma once
#include "common/CommonHeaders.h"
#include "irrKlang.h"
#include "ecs/Scene.h"
#include "ecs/System.h"
#include "ecs/CoreComponents/Transform.h"
#include "core/sound/AudioSource.h"
namespace illusion::core::sound {
	struct SoundSystem : ecs::System {
		SYSTEM_NAME("Sound System");
		SYSTEM_REGISTER(SoundSystem);
		ecs::core::Transform* transformComponent;
		AudioSource* audioSourceComponent;

		virtual void LateUpdate() override {
			u32 size = static_cast<u32>(ToEntity.size());
			for (currIndex = 0; currIndex < size; currIndex++) {
				size_t& sound_id = audioSourceComponent->sound_id[currIndex];
				component_id transform_id=transformComponent->getIndex(ToEntity[currIndex]);

				if (!audioSourceComponent->idComputed[currIndex]) {
					sound_id = StartSound(audioSourceComponent->sourcePath[currIndex],audioSourceComponent->is3D[currIndex]);
					audioSourceComponent->idComputed[currIndex] = true;
				}
				irrklang::ISound* sound = sounds[sound_id];
				if (sound==nullptr) {
					INTERNAL_ERR("SOUND PTR IS NULL for entity : ",transformComponent->name[transform_id]);
					continue;
				}
				//If sound type has changed update SoundType
				if (audioSourceComponent->is3D[currIndex]!=is3Dsounds[sound_id]) {
					ChangeSoundType(sound_id, audioSourceComponent->is3D[currIndex]);
				}
				// Update Sound Values
				sound->setIsPaused((audioSourceComponent->paused[currIndex]));
				sound->setIsLooped((audioSourceComponent->loop[currIndex]));
				sound->setVolume((audioSourceComponent->volume[currIndex]));
				if (audioSourceComponent->is3D[currIndex]) {
					UpdatePosition(sound, transformComponent->position[transform_id]);
				}
			}
		}
		virtual void Initialize(ecs::Scene& scene) override {
			transformComponent = scene.GetComponent<ecs::core::Transform>();
			SetDependencies(audioSourceComponent);
		}
		~SoundSystem() {
			for (irrklang::ISound* sound : sounds) {
				if(sound!=nullptr)sound->drop();
			}
			for (auto &[key,source] : sources) {
				if (source != nullptr)source->drop();
			}
			soundEngine->drop();
		}
	private:
		irrklang::ISoundEngine* soundEngine;
		util::Map<std::string,irrklang::ISoundSource*> sources;
		util::Array<irrklang::ISound*> sounds;
		util::Array<bool> is3Dsounds;
		// UTIL functions
		inline void LoadSound(std::string sourcePath) {
			sources[sourcePath]=soundEngine->addSoundSourceFromFile(sourcePath.c_str(), irrklang::ESM_AUTO_DETECT, true);
			INTERNAL_INFO("Loaded Sound Source : ", sourcePath);
		}
		inline size_t StartSound(std::string sourcePath, bool is3D) {
			if (sources.find(sourcePath)==sources.end()) {
				INTERNAL_ERR("Sound Souce File Not FOUND");
			}
			if (is3D) {
				sounds.push_back(soundEngine->play3D(sources[sourcePath], irrklang::vec3df()));
			}
			else {
				sounds.push_back(soundEngine->play2D(sources[sourcePath]));
			}
			is3Dsounds.push_back(is3D);
			return sounds.size() - 1;
		}
		inline void ChangeSoundType(size_t sound_id, bool is3D) {
			irrklang::ISoundSource* source =sounds[sound_id]->getSoundSource();
			if (is3D) {
				sounds[sound_id]=soundEngine->play3D(source, irrklang::vec3df());
			}
			else {
				sounds[sound_id]=soundEngine->play2D(source);
			}
			is3Dsounds[sound_id] = is3D;
		}
		inline void UpdatePosition(irrklang::ISound *sound,Vec3& pos) {
			irrklang::vec3df sound_pos;
			sound_pos.X = pos.x;
			sound_pos.Y = pos.y;
			sound_pos.Z = pos.z;
			sound->setPosition(sound_pos);
		}
		//END UTIL		
	};
}