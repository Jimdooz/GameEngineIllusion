#pragma once
#include <streambuf>
#include <sstream>

#include <fstream>
#include <iostream>
#include <filesystem>
namespace fs = std::filesystem;

#include "resources/Project.h"

#include "ecs/Scene.h"
#include "resources/DataConvertor.h"
#include "resources/system/Json.h"
using json = illusion::json;

namespace illusion::resources::assets {

	struct MaterialResource {
		size_t id;
		size_t shaderId;
		json uniforms;
	};

	MaterialResource LoadMaterial(std::string materialPath) {
		json jsonLoaded;

		{
			std::ifstream t(materialPath);
			std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
			jsonLoaded = json::parse(str);
		}

		std::string shaderPath = project::projectPath + "/Assets/" + std::string(jsonLoaded["shader"]);
		json uniforms = jsonLoaded["uniforms"];


		std::string shaderPathRelative = fs::relative(shaderPath, project::projectPath + "/Assets/").string();
		std::string relativePath = fs::relative(materialPath, project::projectPath + "/Assets/").string();

		return {
			std::hash<std::string>{}(relativePath),
			std::hash<std::string>{}(shaderPathRelative),
			uniforms,
		};
	}

	util::Array<MaterialResource> LoadAllMaterials(std::string path = project::projectPath + "/Assets/") {
		util::Array<MaterialResource> allMaterials;

		for (auto& p : fs::directory_iterator(path)) {
			std::string currPath = p.path().filename().string();
			if (p.is_directory() && currPath.find('.') == std::string::npos) {

				util::Array<MaterialResource>& recursiveMaterial = LoadAllMaterials(path + "/" + currPath);
				for (u32 i = 0; i < recursiveMaterial.size(); i++) {
					allMaterials.push_back(recursiveMaterial[i]);
				}

			}
			else if (currPath.find(".material") != std::string::npos) {
				//LOAD SHADER
				try {
					allMaterials.push_back(LoadMaterial(path + "/" + currPath));
				}
				catch (const std::exception&) {
					INTERNAL_ERR("Impossible de lire le material ", path + "/" + currPath);
				}
			}
		}

		return allMaterials;
	}

}