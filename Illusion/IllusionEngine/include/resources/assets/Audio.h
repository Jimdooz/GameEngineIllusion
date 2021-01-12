#pragma once
#pragma once
#include <streambuf>
#include <sstream>

#include <fstream>
#include <iostream>
#include <filesystem>
namespace fs = std::filesystem;

#include "core/sound/SoundSystem.h"
#include "resources/Project.h"


namespace illusion::resources::assets {

	static bool LoadAllSounds(std::string path = CurrentProject().path + "/Assets") {
		for (auto& p : fs::directory_iterator(path)) {
			std::string currPath = p.path().filename().string();
			if (p.is_directory() && currPath.find('.') == std::string::npos) {
				LoadAllSounds(path + "/" + currPath);
			}
			else if (currPath.find(".wav") != std::string::npos) {
				//LOAD ALL SOUNDS
				try {
					INTERNAL_INFO("TRY LOAD SOUND : ", path + "/" + currPath);
					illusion::core::sound::LoadSound(path + "/" + currPath);
				}
				catch (const std::exception& exception) {
					INTERNAL_ERR("Impossible de lire le mesh ", path + "/" + currPath, "\n", exception.what());
				}
			}
		}

		return true;
	}
}