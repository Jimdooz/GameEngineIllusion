#pragma once
#include <streambuf>
#include <sstream>

#include <fstream>
#include <iostream>
#include <filesystem>
namespace fs = std::filesystem;

#include "resources/Project.h"

#include "resources/DataConvertor.h"
#include "resources/system/Json.h"
using json = illusion::json;

namespace illusion::resources::assets {

	struct MaterialResource {
		size_t id;
		size_t shaderId;
		std::string name;
		std::string relativePath;
		std::string relativeShaderPath;
		json uniforms;
	};

	static MaterialResource LoadMaterial(std::string materialPath) {
		json jsonLoaded;

		{
			std::ifstream t(materialPath);
			std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
			jsonLoaded = json::parse(str);
		}

		json uniforms = jsonLoaded["uniforms"];
		std::string shaderPathRelative = "";

		size_t shaderId;
		if (jsonLoaded["shader"].is_string()) {
			std::string shaderPath = CurrentProject().path + "/Assets/" + std::string(jsonLoaded["shader"]);
			shaderPathRelative = fs::relative(shaderPath, CurrentProject().path + "/Assets/").string();
			shaderId = std::hash<std::string>{}(shaderPathRelative);
		}
		else {
			int testMainId = jsonLoaded["shader"];
			shaderId = testMainId;
		}

		std::string relativePath = fs::relative(materialPath, CurrentProject().path + "/Assets/").string();

		return {
			std::hash<std::string>{}(relativePath),
			shaderId,
			fs::path(relativePath).filename().string(),
			relativePath,
			shaderPathRelative,
			uniforms,
		};
	}

	static util::Array<MaterialResource> LoadAllMaterials(std::string path = CurrentProject().path + "/Assets") {
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
				catch (const std::exception& exception) {
					INTERNAL_ERR("Impossible de lire le material ", path + "/" + currPath, "\n", exception.what());
				}
			}
		}

		return allMaterials;
	}

}