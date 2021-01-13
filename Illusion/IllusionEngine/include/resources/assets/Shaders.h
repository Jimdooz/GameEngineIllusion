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

	struct ShaderResource {
		size_t id = 0;
		std::string vertex;
		std::string fragment;
		json uniforms;
		std::string name = "default shader";
		std::string relativePath = "";
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
			jsonLoaded["name"].is_null() ? relativePath : jsonLoaded["vertex"],
			relativePath
		};
	}

	void LoadAllShaders(std::string path = CurrentProject().path + "/Assets");
	void LoadAllShadersRecursive(std::string path = CurrentProject().path + "/Assets");

}