#pragma once

#include "ecs/Ecs.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

namespace illusioneditor::views::GameInspector {

	using namespace illusion;

	namespace {
		illusion::ecs::Scene* currentScene = nullptr;
		illusion::ecs::entity_id currentSelected;
	}

	void GenerateUiComponent(const illusion::ecs::PublicComponentDatas& data, illusion::ecs::Component *component) {
		ecs::component_id componentId = component->getIndex(currentSelected);
		if (component->getIndex(currentSelected) == ecs::id::invalid_id) return;

		if (data.type == typeid(illusion::util::Array<Vec3>).hash_code()) {
			illusion::util::Array<Vec3>& vec3 = *(illusion::util::Array<Vec3>*)data.data;
			float vec3a[3] = { vec3[componentId].x, vec3[componentId].y, vec3[componentId].z };
			ImGui::DragFloat3(data.name.c_str(), vec3a, 0.01f);
			vec3[componentId].x = vec3a[0];
			vec3[componentId].y = vec3a[1];
			vec3[componentId].z = vec3a[2];
		}else if (data.type == typeid(illusion::util::Array<Quaternion>).hash_code()) {
			illusion::util::Array<Quaternion>& vec4 = *(illusion::util::Array<Quaternion>*)data.data;
			float vec4a[4] = { vec4[componentId].x, vec4[componentId].y, vec4[componentId].z, vec4[componentId].w };
			ImGui::DragFloat4(data.name.c_str(), vec4a, 0.01f);
			vec4[componentId].x = vec4a[0];
			vec4[componentId].y = vec4a[1];
			vec4[componentId].z = vec4a[2];
			vec4[componentId].w = vec4a[3];
		} else if (data.type == typeid(illusion::util::Array<f32>).hash_code()) {
			illusion::util::Array<f32>& val = *(illusion::util::Array<f32>*)data.data;
			f32 floatValue = val[componentId];
			ImGui::DragFloat(data.name.c_str(), &floatValue, 0.01f);
			val[componentId] = floatValue;
		}
	}
	
	void Show() {
		ImGui::Begin("Inspector [DEV]");

		if (currentSelected != ecs::id::invalid_id && currentScene != nullptr) {

			ecs::component_id iSelected = (ecs::component_id)ecs::id::Index(currentSelected);

			if (iSelected >= currentScene->entities.m_entities.size()
				|| !currentScene->entities.IsAliveAtIndex(ecs::entity_id{ iSelected })) {
				ImGui::End();
				return;
			}

			if (ImGui::TreeNodeEx("Components###COMPONENT", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_FramePadding)) {
				for (auto const& [key, val] : currentScene->components) {
					std::string title = val->getName() + "###" + std::to_string(key);
					if (val->getIndex(currentSelected) == ecs::id::invalid_id) continue;
					if (ImGui::TreeNode(title.c_str())) {
						for (u32 i = 0; i < val->publicDatas.size(); i++) {
							if(val->publicDatas[i].visible)
								GenerateUiComponent(val->publicDatas[i], val);
						}
						ImGui::TreePop();
					}
				}
				ImGui::TreePop();
			}
			if (ImGui::TreeNodeEx("Systems###SYSTEM", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_FramePadding)) {
				ImVec4 saveStyle = ImGui::GetStyle().Colors[ImGuiCol_Text];
				ImGui::GetStyle().Colors[ImGuiCol_Text] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
				for (auto const& [key, val] : currentScene->systems) {
					if (val->ToData[iSelected] != ecs::id::invalid_id) ImGui::Button(val->getName().c_str());
				}
				ImGui::GetStyle().Colors[ImGuiCol_Text] = saveStyle;
				ImGui::TreePop();
			}

			if (ImGui::Button("Add Component")) {
				ImGui::OpenPopup("PopupComponent");
			}

			if (ImGui::BeginPopup("PopupComponent")) {
				for (auto const& [key, val] : currentScene->components) {
					if (key == typeid(ecs::core::Transform).hash_code()) continue;
					if (val->getIndex(currentSelected) != ecs::id::invalid_id) {
						std::string name = "Remove " + val->getName();
						if (ImGui::Button(name.c_str())) {
							currentScene->EntityRemoveComponent(currentSelected, key);
						}
					}
					else {
						std::string name = "Add " + val->getName();
						if (ImGui::Button(name.c_str())) {
							currentScene->EntityAddComponent(currentSelected, key);
						}
					}
				}
				ImGui::EndPopup();
			}

		}

		ImGui::End();
	}

	void SetScene(illusion::ecs::Scene& scene) {
		currentScene = &scene;
	}

	void SetSelected(illusion::ecs::entity_id selected) {
		currentSelected = selected;
	}

}