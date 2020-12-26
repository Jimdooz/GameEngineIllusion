#pragma once

#include "ecs/Ecs.h"
#include "resources/assets/Scenes.h"

#include <fstream>
#include <iostream>
#include <filesystem>

#include <streambuf>
#include <sstream>
#include <resources/system/Json.h>
using json = illusion::json;

#include <string>

namespace illusioneditor::project {
	
	namespace config {
		bool projectOpen = false;
		std::string projectName = "";

		std::string projectPath = "";
		std::string currentScenePath = "";
	}

	namespace tools {
		void CreateProject(std::string projectPath, std::string projectName) {
			if (projectName.size() <= 0 || projectPath.size() <= 0) return;
			std::string realProjectPath = projectPath + "\\" + projectName;
			std::filesystem::create_directories(realProjectPath);
			std::filesystem::create_directories(realProjectPath + "\\Assets\\Scenes");

			json projectConfig;
			projectConfig["projectName"] = projectName;
			projectConfig["currentScenePath"] = "\\Assets\\Scenes\\default.scene";

			std::ofstream configFile;
			configFile.open(std::string(realProjectPath + "\\config.json"));
			configFile << projectConfig.dump(4);
			configFile.close();

			illusion::ecs::Scene scene;
			std::ofstream mySceneFile;
			mySceneFile.open(realProjectPath + "\\Assets\\Scenes\\default.scene");
			mySceneFile << illusion::resources::assets::ExportScene(scene).dump(4);
			mySceneFile.close();
		}

		bool LoadProject(std::string projectPath, std::string* error = nullptr) {
			json configFile;
			std::string readedFile;

			try {
				std::ifstream t(projectPath + "\\config.json");
				readedFile = std::string((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
				if (readedFile == "") throw "Invalid file";
			} catch (const char* msg) {
				if (error != nullptr) *error = "Unable to open config file";
				return false;
			}
			json jsonLoaded;
			try {
				jsonLoaded = json::parse(readedFile);
			} catch (...) {
				if (error != nullptr) *error = "Unable to parse config file";
				return false;
			}

			config::projectPath = projectPath;
			config::projectName = jsonLoaded["projectName"];
			config::currentScenePath = jsonLoaded["currentScenePath"];

			config::projectOpen = true;

			return true;
		}
	}
}