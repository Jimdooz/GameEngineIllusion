#pragma once

#include "ecs/Ecs.h"

#include "project/ProjectManager.h"
#include "resources/Project.h"

#include "resources/assets/Materials.h"

#include "core/rendering/Importer.h"

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
		bool itemMaterial = false;
		bool itemModel = false;

		bool needOpenPopupRename = false;
		std::string basePathFromRename = "";
		std::string pathFromRename = "";
		std::string pathToRename = "";

		bool needOpenPopupDelete = false;
		std::string pathToDelete = "";
	}

	void ResetItemIs() {
		itemScene = false;
		itemMaterial = false;
		itemModel = false;
	}

	bool hasEnding(std::string const& fullString, std::string const& ending) {
		if (fullString.length() >= ending.length()) {
			return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
		}
		else {
			return false;
		}
	}

	void CreateScene() {
		illusion::ecs::Scene scene;

		//Create Camera
		ecs::entity_id entity = scene.CreateEntity();
		scene.GetComponent<ecs::core::Transform>()->name[entity] = "Camera";
		scene.GetComponent<ecs::core::Transform>()->position[entity] = Vec3(0, 0, 3);
		scene.EntityAddComponent<ecs::core::Camera>(entity);

		std::ofstream mySceneFile;
		mySceneFile.open(realPathSelected + "/newScene.scene");
		mySceneFile << illusion::resources::assets::ExportScene(scene).dump(4);
		mySceneFile.close();
	}

	void CreateMaterial() {
		std::string pathMaterial = realPathSelected + "/newMaterial.material";

		json materialDefault;
		materialDefault["shader"] = 0;
		materialDefault["uniforms"] = json::object();

		INFO(pathMaterial, "->", materialDefault);

		std::ofstream myMaterialFile;
		myMaterialFile.open(pathMaterial);
		myMaterialFile << materialDefault.dump(4);
		myMaterialFile.close();
	}

	void LoadScene(std::string pathScene) {
		illusioneditor::views::MaterialEditor::Close();
		json jsonLoaded;
		{
			std::ifstream t(pathScene);
			std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
			jsonLoaded = json::parse(str);
		}

		illusioneditor::views::GameHiearchy::SetSelected((ecs::entity_id)ecs::id::invalid_id);
		resources::assets::LoadScene(*scene, jsonLoaded);
		// /!\ [Romain Saclier] On va devoir faire un changement sur l'endroit où se trouve les données du projet courant
		illusioneditor::project::config::currentScenePath = fs::relative(pathScene, illusioneditor::project::config::projectPath).string();
		illusion::resources::CurrentProject().currentScenePath = illusioneditor::project::config::currentScenePath;
	}

	void ReloadCurrentScene() {
		if(illusioneditor::project::config::projectPath != "" && illusioneditor::project::config::currentScenePath != "")
			LoadScene(illusioneditor::project::config::projectPath + "/" + illusioneditor::project::config::currentScenePath);
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
					ResetItemIs();
				}

				if (ImGui::BeginPopup("Popup Project")) {
					if (ImGui::BeginMenu("Create"))
					{
						if (ImGui::MenuItem("Folder")) CreateDirectory();
						if (ImGui::MenuItem("Scene")) CreateScene();
						if (ImGui::MenuItem("Material")) CreateMaterial();
						ImGui::EndMenu();
					}

					if (itemModel) {
						ImGui::Separator();
						if (ImGui::MenuItem("Import model")) illusion::import3DModel(itemSelected.c_str(), *scene);
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
							if (!p.is_directory()) continue;
							ImGui::TextColored((ImVec4)ImColor(255, 255, 255), fileName.c_str());
							if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
								pathSelected = p.path().filename().string();
								realPathSelected = p.path().string();
							}
						}
						for (auto& p : fs::directory_iterator(realPathSelected)) {
							std::string fileName = p.path().filename().string();
							if (p.is_directory()) continue;
							if (hasEnding(fileName, ".scene")) {
								ImGui::TextColored((ImVec4)ImColor(100, 100, 255), fileName.c_str());
								if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
									INTERNAL_INFO("LOAD SCENE");
									LoadScene(p.path().string());
								}

								if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(1)) {
									itemScene = true;
								}
							}else if (hasEnding(fileName, ".material")) {
								ImGui::TextColored((ImVec4)ImColor(247, 236, 89), fileName.c_str());

								if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
									illusioneditor::views::MaterialEditor::EditMaterialPath(scene, p.path().string());
								}

								if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(1)) {
									itemMaterial = true;
								}
							} else if (hasEnding(fileName, ".fbx") || hasEnding(fileName, ".obj") || hasEnding(fileName, ".dae")) {
								ImGui::TextColored((ImVec4)ImColor(203, 174, 186), fileName.c_str());

								if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
									//illusioneditor::views::MaterialEditor::EditMaterialPath(scene, p.path().string());
								}

								if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(1)) {
									itemModel = true;
								}
							}
							else {
								ImGui::TextColored((ImVec4)ImColor(100, 100, 100), fileName.c_str());
							}

							if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(1)) {
								itemSelected = p.path().string();
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
					ImGui::TextColored((ImVec4)ImColor(255, 100, 100), std::filesystem::directory_entry(pathToDelete).path().filename().string().c_str());
					ImGui::Separator();

					if (ImGui::Button("Close")) ImGui::CloseCurrentPopup();
					ImGui::SameLine();

					ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor(255, 100, 100));
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
					size_t n = pathToRename.length() + 64;
					char* buf1 = new char[n];
					strcpy(buf1, pathToRename.c_str());
					ImGui::InputText("###goto", buf1, n);

					pathToRename = std::string(buf1);
					delete[] buf1;

					if (ImGui::Button("Close")) ImGui::CloseCurrentPopup();
					ImGui::SameLine();
					//@Todo change rename to modify
					if (ImGui::Button("Rename")) {
						std::string newPath = basePathFromRename + "/" + pathToRename;
						fs::rename(pathFromRename, newPath);
						ImGui::CloseCurrentPopup();

						if (itemScene) {
							LoadScene(newPath);
						}
						//@todo changer la fonction pour uniquement modifier le tableau meshbyshader
						if (itemMaterial) {
							scene->renderer->GenerateMaterials();
							scene->renderer->ReGenerateMeshByShader();
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