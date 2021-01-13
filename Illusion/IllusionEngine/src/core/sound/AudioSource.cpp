#include "core/sound/AudioSource.h"


namespace illusion::core::sound {
	void AudioSource::AddDatas(ecs::entity_id id) {
		AddData(relativePath, std::string(""));
		AddData<irrklang::ISound *>(sound_ptr, nullptr);
		AddData<boolean>(is3D, false);
		AddData<boolean>(loop, false);
		AddData<boolean>(paused, true);
		AddData(volume, 0.5f);

		AddData<u32>(playPosition, 0);

		AddData(old_relativePath, std::string(""));
		AddData<boolean>(old_is3D, false);
	}
	

	void AudioSource::RemoveDatas(ecs::component_id index, ecs::entity_id id) {
		FreeCurrentSound(index);
		RemoveData(relativePath, index);
		RemoveData(is3D, index);
		RemoveData(sound_ptr, index);
		RemoveData(loop, index);
		RemoveData(paused, index);
		RemoveData(volume, index);
		RemoveData(playPosition, index);
		RemoveData(old_relativePath, index);
		RemoveData(old_is3D, index);
	}
}