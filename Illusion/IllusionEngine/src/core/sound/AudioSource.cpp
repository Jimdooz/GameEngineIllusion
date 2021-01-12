#include "core/sound/AudioSource.h"


namespace illusion::core::sound {
	void AudioSource::AddDatas(ecs::entity_id id) {
		AddData(relativePath, std::string("Audio\\breakout.mp3"));
		AddData<size_t>(sound_id, 0);
		AddData<boolean>(idComputed, false);
		AddData<boolean>(is3D, false);
		AddData<boolean>(loop, false);
		AddData<boolean>(paused, false);
		AddData(volume, 1.0f);
	}
	void AudioSource::RemoveDatas(ecs::component_id index, ecs::entity_id id) {
		RemoveData(index, relativePath, sound_id, idComputed, is3D, loop, paused, volume);
	}
}