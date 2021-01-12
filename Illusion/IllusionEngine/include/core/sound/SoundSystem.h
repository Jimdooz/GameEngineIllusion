#pragma once
#include "common/CommonHeaders.h"
#include "irrKlang.h"
#include "ecs/Scene.h"
#include "ecs/System.h"
#include "ecs/CoreComponents/Transform.h"
#include "core/sound/AudioSource.h"

#include "resources/Project.h"

namespace illusion::core::sound {

	struct SoundEngine {
		irrklang::ISoundEngine* soundEngine = irrklang::createIrrKlangDevice();
		util::Map<std::string, irrklang::ISoundSource*> sources;

		static SoundEngine main;
	};
	
	static inline SoundEngine& GetSoundEngine() {
		return SoundEngine::main;
	}

	//used to load files from 
	static void LoadSound(std::string fullPath) {
		std::string relativePath = fs::relative(fullPath, illusion::resources::CurrentProject().path + "/Assets").string();
		GetSoundEngine().sources[relativePath]= GetSoundEngine().soundEngine->addSoundSourceFromFile(fullPath.c_str(), irrklang::ESM_AUTO_DETECT, true);
		INTERNAL_INFO("Loaded Sound Source : ", relativePath, " ", &GetSoundEngine().sources);
	}
#define INVALID_SOUND_ID 99999999
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
					sound_id = StartSound(audioSourceComponent->relativePath[currIndex],audioSourceComponent->is3D[currIndex]);
					if(sound_id==INVALID_SOUND_ID){
						continue;
					}
					INTERNAL_INFO("ID SOUND GET : ", sound_id);
					audioSourceComponent->idComputed[currIndex] = true;
				}
				irrklang::ISound* sound = sounds[sound_id];
				if (sound==nullptr) {
					INTERNAL_ERR("SOUND PTR IS NULL for entity : ",transformComponent->name[transform_id], " addr ", sounds[0], " size ", sounds.size());
					continue;
				}
				//If sound type has changed update SoundType
				if (audioSourceComponent->is3D[currIndex]!=is3Dsounds[sound_id]) {
					ChangeSoundType(sound_id, audioSourceComponent->is3D[currIndex]);
				}
				// Update Sound Values
				INTERNAL_INFO("ID SOUND PTR : ", sound_id);
				sound->setIsPaused(audioSourceComponent->paused[currIndex]);
				sound->setIsLooped(audioSourceComponent->loop[currIndex]);
				sound->setVolume(audioSourceComponent->volume[currIndex]);
				if (audioSourceComponent->is3D[currIndex]) {
					UpdatePosition(sound, transformComponent->position[transform_id]);
				}
			}
		}
		virtual void Initialize(ecs::Scene& scene) override {
			transformComponent = scene.GetComponent<ecs::core::Transform>();
			audioSourceComponent = scene.GetComponent<AudioSource>();
			SetDependencies(audioSourceComponent);
		}
		~SoundSystem() {
			for (irrklang::ISound* sound : sounds) {
				if(sound!=nullptr)sound->drop();
			}
		}		
	private:
		util::Array<> sounds;
		util::Array<bool> is3Dsounds;
		// UTIL functions
		size_t StartSound(std::string relativePath, bool is3D) {
			if (GetSoundEngine().sources.find(relativePath)== GetSoundEngine().sources.end()) {
				INTERNAL_ERR("Sound Souce File Not FOUND : ", relativePath, " ", &GetSoundEngine().sources);
				return INVALID_SOUND_ID;
			}
			if (is3D) {
				sounds.push_back(GetSoundEngine().soundEngine->play3D(GetSoundEngine().sources[relativePath], irrklang::vec3df(),true, false, true));
			}
			else {
				irrklang::ISound* sound = GetSoundEngine().soundEngine->play2D(GetSoundEngine().sources[relativePath],true, false, true);
				sounds.push_back(sound);
			}
			is3Dsounds.push_back(is3D);
			if(sounds.size()>0)
			INTERNAL_INFO("Add sound at adress ", &sounds);
			return sounds.size() - 1;
		}
		inline void ChangeSoundType(size_t sound_id, bool is3D) {
			irrklang::ISoundSource* source =sounds[sound_id]->getSoundSource();
			if (is3D) {
				sounds[sound_id]= GetSoundEngine().soundEngine->play3D(source, irrklang::vec3df());
			}
			else {
				sounds[sound_id]= GetSoundEngine().soundEngine->play2D(source);
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