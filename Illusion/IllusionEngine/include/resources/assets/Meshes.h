#pragma once
#include <streambuf>
#include <sstream>

#include <fstream>
#include <iostream>
#include <filesystem>
namespace fs = std::filesystem;

#include "core/rendering/Renderer.h"
#include "resources/Project.h"
#include "core/rendering/Importer.h"

#include "resources/DataConvertor.h"
#include "resources/system/Json.h"
using json = illusion::json;

namespace illusion::resources::assets {

	static bool LoadAllMeshes(illusion::Renderer& renderer, std::string path = CurrentProject().path + "/Assets") {
		for (auto& p : fs::directory_iterator(path)) {
			std::string currPath = p.path().filename().string();
			if (p.is_directory() && currPath.find('.') == std::string::npos) {
				LoadAllMeshes(renderer, path + "/" + currPath);
			}
			else if (currPath.find(".fbx") != std::string::npos || currPath.find(".obj") != std::string::npos || currPath.find(".dae") != std::string::npos) {
				//LOAD MESHES
				try {
					illusion::load3DModel((path + "/" + currPath).c_str(), renderer);
				}
				catch (const std::exception& exception) {
					INTERNAL_ERR("Impossible de lire le mesh ", path + "/" + currPath, "\n", exception.what());
				}
			}
		}

		return true;
	}
}