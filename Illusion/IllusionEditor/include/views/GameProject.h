#pragma once

#include "ecs/Ecs.h"

#include "project/ProjectManager.h"

#include <string>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <fstream>
#include <iostream>
#include <filesystem>
namespace fs = std::filesystem;

namespace illusioneditor::views::GameProject {

	using namespace illusion;

	namespace {
		illusion::ecs::Scene* scene = nullptr;

		std::string pathSelected = "";
		std::string realPathSelected = "";

		std::string itemSelected = "";
		bool itemScene = false;

		bool needOpenPopupRename = false;
		std::string basePathFromRename = "";
		std::string pathFromRename = "";
		std::string pathToRename = "";

		bool needOpenPopupDelete = false;
		std::string pathToDelete = "";
	}

	bool hasEnding(std::string const& fullString, std::string const& ending) {
		if (fullString.length() >= ending.length()) {
			return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
		}
		else {
			return false;
		}
	}

	void RenamePath(std::string path) {
		needOpenPopupRename = true;
		pathFromRename = std::filesystem::directory_entry(path).path().string();
		basePathFromRename = std::filesystem::directory_entry(path).path().parent_path().string();
		pathToRename = std::filesystem::directory_entry(path).path().filename().string();
	}

	void DeletePath(std::string path) {
		needOpenPopupDelete = true;
		pathToDelete = path;
	}

	void CreateDirectory() {
		fs::create_directories(realPathSelected + "/New Folder");
		realPathSelected = realPathSelected + "/New Folder";
		pathSelected = "New Folder";
		RenamePath(realPathSelected);
	}

	void CreateScene() {
		illusion::ecs::Scene scene;
		std::ofstream mySceneFile;
		mySceneFile.open(realPathSelected + "/newScene.scene");
		mySceneFile << illusion::resources::assets::ExportScene(scene).dump(4);
		mySceneFile.close();
	}

	void LoadScene(std::string pathScene) {
		json jsonLoaded;
		{
			std::ifstream t(pathScene);
			std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
			jsonLoaded = json::parse(str);
		}

		illusioneditor::views::GameHiearchy::SetSelected((ecs::entity_id)ecs::id::invalid_id);
		resources::assets::LoadScene(*scene, jsonLoaded);
		illusioneditor::project::config::currentScenePath = fs::relative(pathScene, illusioneditor::project::config::projectPath).string();
	}

	void RecursiveTreeAssets(std::string path) {
		for (auto& p : fs::directory_iterator(path)) {
			std::string fileName = p.path().filename().string();
			std::string filePath = p.path().string();
			if (p.is_directory() && fileName.find('.') == std::string::npos) {

				bool open = ImGui::TreeNodeEx(fileName.c_str(),
					ImGuiTreeNodeFlags_FramePadding | (realPathSelected == filePath ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow, fileName.c_str());

				if (ImGui::IsItemClicked()) {
					pathSelected = fileName;
					realPathSelected = filePath;
				}

				std::string popupName = "Project Menu##" + filePath;

				if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(1)) {
					pathSelected = fileName;
					realPathSelected = filePath;
					ImGui::OpenPopup(popupName.c_str());
				}

				if (ImGui::BeginPopup(popupName.c_str())) {

					if (ImGui::BeginMenu("Create"))
					{
						if (ImGui::MenuItem("Folder")) {
							CreateDirectory();
						}
						ImGui::EndMenu();
					}

					if (ImGui::MenuItem("Delete")) DeletePath(filePath);
					if (ImGui::MenuItem("Rename")) RenamePath(filePath);

					ImGui::EndPopup();
				}

				if (open) {
					RecursiveTreeAssets(filePath);
					ImGui::TreePop();
				}
			}
		}
	}

	void Show() {

		if (illusioneditor::project::config::projectOpen) {

			{
				ImGui::Begin("Project");

				ImGui::Columns(2);

				ImGui::BeginChild("##ProjectRegion");
				{
					bool open = ImGui::TreeNodeEx("Assets", (realPathSelected == illusioneditor::project::config::projectPath + "/Assets" ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow);

					if (ImGui::IsItemClicked()) {
						pathSelected = "Assets";
						realPathSelected = illusioneditor::project::config::projectPath + "/Assets";
					}

					if (open) {
						RecursiveTreeAssets(illusioneditor::project::config::projectPath + "/Assets");
						ImGui::TreePop();
					}
				}
				ImGui::EndChild();
				ImGui::NextColumn();
				ImGui::BeginChild("##ProjectRegion2");

				if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(1)) {
					ImGui::OpenPopup("Popup Project");
					itemSelected = "";
					itemScene = false;
				}

				if (ImGui::BeginPopup("Popup Project")) {
					if (ImGui::BeginMenu("Create"))
					{
						if (ImGui::MenuItem("Folder")) CreateDirectory();
						if (ImGui::MenuItem("Scene")) CreateScene();
						ImGui::EndMenu();
					}

					if (itemSelected != "") {
						ImGui::Separator();
						if (ImGui::MenuItem("Rename")) RenamePath(itemSelected);
						if (ImGui::MenuItem("Delete")) DeletePath(itemSelected);
					}

					ImGui::EndPopup();
				}
				
				if (pathSelected != "") {
					try {
						for (auto& p : fs::directory_iterator(realPathSelected)) {
							std::string fileName = p.path().filename().string();
							if (!p.is_directory()) {
								if (hasEnding(fileName, ".scene")) {
									ImGui::TextColored((ImVec4)ImColor::ImColor(100, 100, 255), fileName.c_str());
									if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
										INTERNAL_INFO("LOAD SCENE");
										LoadScene(p.path().string());
									}

									if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(1)) {
										itemScene = true;
									}
								}
								else {
									ImGui::TextColored((ImVec4)ImColor::ImColor(100, 100, 100), fileName.c_str());
								}
								
								if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(1)) {
									itemSelected = p.path().string();
								}
							}
							else {
								ImGui::TextColored((ImVec4)ImColor::ImColor(255, 255, 255), fileName.c_str());
								if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
									pathSelected = p.path().filename().string();
									realPathSelected = p.path().string();
								}
							}
						}
					} catch (const std::exception&) {
						try {
							realPathSelected = std::filesystem::directory_entry(realPathSelected).path().parent_path().string();
						}
						catch (const std::exception&) {
							realPathSelected = illusioneditor::project::config::projectPath + "/Assets";
						}
					}
				}

				ImGui::EndChild();

				//POPUPS

				//POPUP DELETE
				if (needOpenPopupDelete) {
					ImGui::OpenPopup("Delete Directory");
					needOpenPopupDelete = false;
				}
				if (ImGui::BeginPopupModal("Delete Directory", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
					ImGui::Text("Are your sure you want to delete this item ?");
					ImGui::TextColored((ImVec4)ImColor::ImColor(255, 100, 100), std::filesystem::directory_entry(pathToDelete).path().filename().string().c_str());
					ImGui::Separator();

					if (ImGui::Button("Close")) ImGui::CloseCurrentPopup();
					ImGui::SameLine();

					ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor::ImColor(255, 100, 100));
					if (ImGui::Button("Delete")) {
						fs::remove_all(pathToDelete);
						ImGui::CloseCurrentPopup();
					}
					ImGui::PopStyleColor(1);

					ImGui::EndPopup();
				}

				//POPUP RENAME
				if (needOpenPopupRename) {
					ImGui::OpenPopup("Rename Directory");
					needOpenPopupRename = false;
				}
				if (ImGui::BeginPopupModal("Rename Directory", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
					int n = pathToRename.length() + 64;
					char* buf1 = new char[n];
					strcpy(buf1, pathToRename.c_str());
					ImGui::InputText("###goto", buf1, n);

					pathToRename = std::string(buf1);
					delete buf1;

					if (ImGui::Button("Close")) ImGui::CloseCurrentPopup();
					ImGui::SameLine();
					if (ImGui::Button("Rename")) {
						std::string newPath = basePathFromRename + "/" + pathToRename;
						fs::rename(pathFromRename, newPath);
						ImGui::CloseCurrentPopup();

						if (itemScene) {
							LoadScene(newPath);
						}

					}
					ImGui::EndPopup();
				}

				ImGui::End();
			}

		}

	}

	void SetScene(illusion::ecs::Scene& currScene) {
		scene = &currScene;
	}
}