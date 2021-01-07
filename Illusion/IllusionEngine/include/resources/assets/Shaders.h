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

	struct ShaderResource {
		size_t id;
		std::string vertex;
		std::string fragment;
		json uniforms;
	};

	static ShaderResource LoadShader(std::string shaderPath) {
		json jsonLoaded;

		{
			std::ifstream t(shaderPath);
			std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
			jsonLoaded = json::parse(str);
		}

		std::string vertexPath = jsonLoaded["vertex"];
		std::string fragmentPath = jsonLoaded["fragment"];
		json uniforms = jsonLoaded["uniforms"];

		std::ifstream tVertex(CurrentProject().path + "/Assets/" + vertexPath);
		std::string strVertex((std::istreambuf_iterator<char>(tVertex)), std::istreambuf_iterator<char>());

		std::ifstream tFragment(CurrentProject().path + "/Assets/" + fragmentPath);
		std::string strFragment((std::istreambuf_iterator<char>(tFragment)), std::istreambuf_iterator<char>());

		std::string relativePath = fs::relative(shaderPath, CurrentProject().path + "/Assets/").string();

		return {
			std::hash<std::string>{}(relativePath),
			strVertex,
			strFragment,
			uniforms,
		};
	}

	static util::Array<ShaderResource> LoadAllShaders(std::string path = CurrentProject().path + "/Assets") {
		util::Array<ShaderResource> allShaders;

		for (auto& p : fs::directory_iterator(path)) {
			std::string currPath = p.path().filename().string();
			if (p.is_directory() && currPath.find('.') == std::string::npos) {

				util::Array<ShaderResource>& recursiveShader = LoadAllShaders(path + "/" + currPath);
				for (u32 i = 0; i < recursiveShader.size(); i++) {
					allShaders.push_back(recursiveShader[i]);
				}

			}
			else if (currPath.find(".shader") != std::string::npos) {
				//LOAD SHADER
				try {
					allShaders.push_back(LoadShader(path + "/" + currPath));
				}
				catch (const std::exception&) {
					INTERNAL_ERR("Impossible de lire le shader ", path + "/" + currPath);
				}
			}
		}

		return allShaders;
	}

}