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

	using namespace illusion;

	struct MaterialResource {
		size_t id;
		size_t shaderId;
		std::string name;
		std::string relativePath;
		std::string relativeShaderPath;
		json uniforms;
	};

	static size_t IdMaterialPath(std::string materialPath) {
		if (CurrentProject().path == "") return 0; //If no project loaded
		return std::hash<std::string>{}(fs::relative(materialPath, CurrentProject().path + "/Assets/").string());
	}

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
			IdMaterialPath(materialPath),
			shaderId,
			fs::path(relativePath).filename().string(),
			relativePath,
			shaderPathRelative,
			uniforms,
		};
	}

	void LoadAllMaterials(std::string path = CurrentProject().path + "/Assets");
	void LoadAllMaterialsRecursive(std::string path = CurrentProject().path + "/Assets");

	static bool SaveMaterial(MaterialResource& material) {
		json toSave;

		if (material.relativeShaderPath == "") toSave["shader"] = material.shaderId;
		else toSave["shader"] = material.relativeShaderPath;
		toSave["uniforms"] = material.uniforms;

		std::ofstream myfile;
		myfile.open(CurrentProject().path + "/Assets/" + material.relativePath);
		myfile << toSave.dump(4);
		myfile.close();

		return true;
	}
}