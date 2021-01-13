#pragma once
#include "common/CommonHeaders.h"
#include "irrKlang.h"
#include "ecs/Scene.h"
#include "ecs/System.h"
#include "ecs/CoreComponents/Transform.h"
#include "ecs/CoreComponents/Camera.h"
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
		GetSoundEngine().sources[relativePath] = GetSoundEngine().soundEngine->addSoundSourceFromFile(fullPath.c_str(), irrklang::ESM_AUTO_DETECT, true);
		INTERNAL_INFO("Loaded Sound Source : ", relativePath, " ", &GetSoundEngine().sources);
	}
	struct SoundSystem : ecs::System {
		SYSTEM_NAME("Sound System");
		SYSTEM_REGISTER(SoundSystem);
		ecs::core::Transform* transforms;
		ecs::core::Camera* cameras;
		AudioSource* audios;

		virtual void LateUpdate() override {
			u32 size = static_cast<u32>(ToEntity.size());
			for (currIndex = 0; currIndex < size; currIndex++) {
				if (audios->sound_ptr[currIndex] != nullptr) {
					if (audios->sound_ptr[currIndex]->isFinished())
						audios->paused[currIndex] = true;
				}

				//If sound source has changed or spatialization change -> update Sound
				if (audios->old_is3D[currIndex] != audios->is3D[currIndex]
					|| audios->old_relativePath[currIndex] != audios->relativePath[currIndex]
					|| audios->sound_ptr[currIndex] == nullptr
					|| (audios->sound_ptr[currIndex]->isFinished() && audios->loop[currIndex])
					) {
					audios->FreeCurrentSound((component_id)currIndex);
					audios->sound_ptr[currIndex] = StartSound(audios->relativePath[currIndex], audios->is3D[currIndex], (bool)audios->loop[currIndex], (bool)audios->paused[currIndex]);

					INTERNAL_INFO(audios->sound_ptr[currIndex]);


					//Set new value to old...
					audios->old_is3D[currIndex] = audios->is3D[currIndex];
					audios->old_relativePath[currIndex] = audios->relativePath[currIndex];
				}

				if (audios->sound_ptr[currIndex] == nullptr) continue;

				irrklang::ISound* sound_ptr = audios->sound_ptr[currIndex];
				// Update Sound Values

				bool p = audios->paused[currIndex] ? true : false;
				bool l = audios->loop[currIndex] ? true : false;
				if (p != sound_ptr->getIsPaused())
					sound_ptr->setIsPaused(p);
				if (l != sound_ptr->isLooped())
					sound_ptr->setIsLooped(l);

				if (audios->volume[currIndex] != sound_ptr->getVolume())
					sound_ptr->setVolume(audios->volume[currIndex]);
				audios->playPosition[currIndex] = sound_ptr->getPlayPosition();

				Vec3 positionSound = transforms->GetPosition(audios->getId(currIndex));
				Vec3 cameraWorldPos = cameras->GetPosition();
				if (cameras->size() > 0) positionSound = glm::translate(cameraWorldPos) * Vec4(positionSound, 1.0);

				if (audios->is3D[currIndex]) {
					UpdatePosition(sound_ptr, positionSound);
				}
			}
		}
		virtual void Initialize(ecs::Scene& scene) override {
			transforms = scene.GetComponent<ecs::core::Transform>();
			cameras = scene.GetComponent<ecs::core::Camera>();
			audios = scene.GetComponent<AudioSource>();
			SetDependencies(audios);
		}

	private:
		// UTIL functions
		inline irrklang::ISound* StartSound(std::string relativePath, bool is3D, bool loop, bool paused) {
			if (GetSoundEngine().sources.find(relativePath) == GetSoundEngine().sources.end()) return nullptr;
			INTERNAL_INFO("START SOUND");
			if (is3D) {
				return GetSoundEngine().soundEngine->play3D(GetSoundEngine().sources[relativePath], irrklang::vec3df(), false, true, true);
			}
			return GetSoundEngine().soundEngine->play2D(GetSoundEngine().sources[relativePath], false, true, true);
		}
		inline void UpdatePosition(irrklang::ISound* sound, Vec3& pos) {
			irrklang::vec3df sound_pos;
			sound_pos.X = pos.x;
			sound_pos.Y = pos.y;
			sound_pos.Z = pos.z;
			if (sound_pos != sound->getPosition())
				sound->setPosition(sound_pos);
		}
		//END UTIL		
	};
}