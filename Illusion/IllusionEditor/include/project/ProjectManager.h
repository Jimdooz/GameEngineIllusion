#pragma once

#include "ecs/Ecs.h"
#include "resources/assets/Scenes.h"

#include "resources/assets/Levels.h"

#include <fstream>
#include <iostream>
#include <filesystem>

#include <streambuf>
#include <sstream>
#include <resources/system/Json.h>
using json = illusion::json;

#include <string>
#include "resources/Project.h"

#include "resources/assets/Audio.h"
//#include "resources/assets/Shaders.h"
#include "resources/assets/Materials.h"
#include "resources/assets/Meshes.h"

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

			//Create Camera
			illusion::ecs::entity_id entity = scene.CreateEntity();
			scene.GetComponent<illusion::ecs::core::Transform>()->name[entity] = "Camera";
			scene.GetComponent<illusion::ecs::core::Transform>()->position[entity] = Vec3(0, 0, 3);
			scene.EntityAddComponent<illusion::ecs::core::Camera>(entity);

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

			// /!\ [Romain Saclier] To change
			config::projectPath = projectPath;
			config::projectName = jsonLoaded["projectName"];
			config::currentScenePath = jsonLoaded["currentScenePath"];

			config::projectOpen = true;

			illusion::resources::CurrentProject().path = config::projectPath;
			illusion::resources::CurrentProject().name = config::projectName;
			illusion::resources::CurrentProject().currentScenePath = config::currentScenePath;
			illusion::resources::CurrentProject().open = config::projectOpen;

			INFO("LOAD PROJECT : ", illusion::resources::CurrentProject().path);

			illusion::resources::assets::LoadAllSounds();

			illusion::resources::assets::LoadAllShaders();
			illusion::resources::assets::LoadAllMaterials();
			illusion::resources::assets::LoadAllMeshes();

			return true;
		}

		bool SaveScene(illusion::ecs::Scene &scene) {
			std::ofstream myfile;
			myfile.open(illusioneditor::project::config::projectPath + "/" + illusioneditor::project::config::currentScenePath);
			myfile << illusion::resources::assets::ExportScene(scene).dump();
			myfile.close();

			return true;
		}
	}
}