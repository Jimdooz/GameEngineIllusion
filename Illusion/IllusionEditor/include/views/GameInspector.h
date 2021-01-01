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

		std::string searchComponent = "";
	}

	void GenerateInputString(std::string name, std::string& str) {
		int n = str.length() + 64;
		char* buf1 = new char[n];
		strcpy(buf1, str.c_str());
		ImGui::InputText(name.c_str(), buf1, n);
		str = std::string(buf1);
		delete buf1;
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
		} else if (data.type == typeid(illusion::util::Array<boolean>).hash_code()) {
			illusion::util::Array<boolean>& val = *(illusion::util::Array<boolean>*)data.data;
			bool value = val[componentId];
			ImGui::Checkbox(data.name.c_str(), &value);
			val[componentId] = value;
		} else if (data.type == typeid(illusion::util::Array<std::string>).hash_code()) {
			illusion::util::Array<std::string>& val = *(illusion::util::Array<std::string>*)data.data;
			std::string& value = val[componentId];
			GenerateInputString(data.name, value);
		}
	}
	
	void Show() {
		ImGui::Begin("Inspector [DEV]");

		if (currentSelected != ecs::id::invalid_id && currentScene != nullptr) {

			ecs::component_id iSelected = (ecs::component_id)ecs::id::Index(currentSelected);
			ecs::core::Transform &transform = *currentScene->GetComponent<ecs::core::Transform>();
			ecs::component_id indexTransform = transform.getIndex(currentSelected);

			if (iSelected >= currentScene->entities.m_entities.size()
				|| !currentScene->entities.IsAliveAtIndex(ecs::entity_id{ iSelected })) {
				ImGui::End();
				return;
			}

			ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
			GenerateInputString("###EntityName", transform.name[indexTransform]);
			ImGui::Dummy(ImVec2(0.0f, 5.0f));

			//Position
			ImGui::SetNextItemWidth(fmaxf(80, ImGui::GetWindowContentRegionWidth() * 0.4 - 25));
			ImGui::LabelText("###Position", "Position");
			ImGui::SameLine(); ImGui::SetNextItemWidth(fmaxf(50, ImGui::GetWindowContentRegionWidth() * 0.2)); ImGui::DragFloat("###PX", &transform.position[indexTransform].x, 0.01f);
			ImGui::SameLine(); ImGui::SetNextItemWidth(fmaxf(50, ImGui::GetWindowContentRegionWidth() * 0.2)); ImGui::DragFloat("###PY", &transform.position[indexTransform].y, 0.01f);
			ImGui::SameLine(); ImGui::SetNextItemWidth(fmaxf(50, ImGui::GetWindowContentRegionWidth() * 0.2)); ImGui::DragFloat("###PZ", &transform.position[indexTransform].z, 0.01f);

			//Rotation
			Vec3 euler = glm::degrees(glm::eulerAngles(transform.rotation[indexTransform]));
			if (abs(euler.x) >= 180) euler.x = 0;
			if (abs(euler.y) >= 180) euler.y = 0;
			if (abs(euler.z) >= 180) euler.z = 0;
			ImGui::SetNextItemWidth(fmaxf(80, ImGui::GetWindowContentRegionWidth() * 0.4 - 25));
			ImGui::LabelText("###Rotation", "Rotation");
			ImGui::SameLine(); ImGui::SetNextItemWidth(fmaxf(50, ImGui::GetWindowContentRegionWidth() * 0.2)); ImGui::DragFloat("###RX", &euler.x, 0.01f);
			ImGui::SameLine(); ImGui::SetNextItemWidth(fmaxf(50, ImGui::GetWindowContentRegionWidth() * 0.2)); ImGui::DragFloat("###RY", &euler.y, 0.01f);
			ImGui::SameLine(); ImGui::SetNextItemWidth(fmaxf(50, ImGui::GetWindowContentRegionWidth() * 0.2)); ImGui::DragFloat("###RZ", &euler.z, 0.01f);
			if (abs(euler.x) >= 180) euler.x = 0;
			if (abs(euler.y) >= 180) euler.y = 0;
			if (abs(euler.z) >= 180) euler.z = 0;

			transform.rotation[indexTransform] = glm::tquat(glm::radians(euler));

			//Scale
			ImGui::SetNextItemWidth(fmaxf(80, ImGui::GetWindowContentRegionWidth() * 0.4 - 25));
			ImGui::LabelText("###Scale", "Scale");
			ImGui::SameLine(); ImGui::SetNextItemWidth(fmaxf(50, ImGui::GetWindowContentRegionWidth() * 0.2)); ImGui::DragFloat("###SX", &transform.scale[indexTransform].x, 0.01f);
			ImGui::SameLine(); ImGui::SetNextItemWidth(fmaxf(50, ImGui::GetWindowContentRegionWidth() * 0.2)); ImGui::DragFloat("###SY", &transform.scale[indexTransform].y, 0.01f);
			ImGui::SameLine(); ImGui::SetNextItemWidth(fmaxf(50, ImGui::GetWindowContentRegionWidth() * 0.2)); ImGui::DragFloat("###SZ", &transform.scale[indexTransform].z, 0.01f);

			ImGui::Dummy(ImVec2(0.0f, 5.0f));
			ImGui::Separator();
			ImGui::Dummy(ImVec2(0.0f, 5.0f));

			for (auto const& [key, val] : currentScene->components) {
				if (val->getName() == "Transform") continue;
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

			ImGui::Dummy(ImVec2(0.0f, 5.0f));
			ImGui::Separator();
			ImGui::Dummy(ImVec2(0.0f, 5.0f));

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
				if (!ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0))
					ImGui::SetKeyboardFocusHere(0);
				GenerateInputString("###Search", searchComponent);
				ImGui::Separator();

				for (auto const& [key, val] : currentScene->components) {
					if (key == typeid(ecs::core::Transform).hash_code()) continue;
					if (searchComponent != "" && val->getName().find(searchComponent) == std::string::npos) continue;

					if (val->getIndex(currentSelected) != ecs::id::invalid_id) {
						ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.00f, 0.20f, 0.20f, 1.00f));
						std::string name = "- " + val->getName();
						if (ImGui::Button(name.c_str())) {
							currentScene->EntityRemoveComponent(currentSelected, key);
						}
						ImGui::PopStyleColor();
					}
					else {
						std::string name = "+ " + val->getName();
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