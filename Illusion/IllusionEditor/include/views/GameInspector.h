#pragma once

#include "ecs/Ecs.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "MaterialEditor.h"

#include "core/sound/AudioSource.h"
#include "core/sound/SoundSystem.h"

namespace illusioneditor::views::GameInspector {

	using namespace illusion;

	namespace {
		illusion::ecs::Scene* currentScene = nullptr;
		illusion::ecs::entity_id currentSelected;

		std::string searchComponent = "";
		std::string searchMesh = "";
		std::string searchMaterial = "";
		std::string searchAudioSource = "";
	}

	void GenerateInputString(std::string name, std::string& str) {
		size_t n = str.length() + 64;
		char* buf1 = new char[n];
		strcpy(buf1, str.c_str());
		ImGui::InputText(name.c_str(), buf1, n);
		str = std::string(buf1);
		delete[] buf1;
	}

	inline bool ValueExist(const json& value, const std::string& key) {
		return value.find(key) != value.end();
	}

	void GenerateUiComponent(const illusion::ecs::PublicComponentDatas& data, illusion::ecs::Component *component) {
		ecs::component_id componentId = component->getIndex(currentSelected);
		if (component->getIndex(currentSelected) == ecs::id::invalid_id) return;

		if (data.type == typeid(illusion::util::Array<ecs::entity_id>).hash_code() && ValueExist(data.uiInformations, "entity")) {
			illusion::util::Array<ecs::entity_id>& entityIds = *(illusion::util::Array<ecs::entity_id>*)data.data;
			ecs::entity_id entityId = entityIds[componentId];

			//MESH
			ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.250f, 0.250f, 0.250f, 1.00f));
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.109f, 0.117f, 0.129f, 1.00f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.109f, 0.117f, 0.129f, 1.00f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.109f, 0.117f, 0.129f, 1.00f));
			if (currentScene->entities.IsAlive(entityId)) {
				ecs::core::Transform *transforms = currentScene->GetComponent<ecs::core::Transform>();
				ImGui::Button(transforms->name[transforms->getIndex(entityId)].c_str(), ImVec2(ImGui::GetWindowContentRegionWidth() * 0.6f - 25.0f, 0.0f));
			}
			else {
				ImGui::Button("Invalid Id", ImVec2(ImGui::GetWindowContentRegionWidth() * 0.6f - 25.0f, 0.0f));
			}
			if (ImGui::BeginDragDropTarget()) {
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("MoveEntity")) {
					ecs::entity_id id;
					memcpy(&id, payload->Data, sizeof(ecs::entity_id));
					entityIds[componentId] = id;
				}
				ImGui::EndDragDropTarget();
			}
			
			ImGui::PopStyleVar();
			ImGui::PopStyleColor(4);

			ImGui::SameLine();
			ImGui::Text(data.name.c_str());

		}else if (data.type == typeid(illusion::util::Array<Vec3>).hash_code()) {
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
		} else if (data.type == typeid(illusion::util::Array<Vec4>).hash_code()) {
			illusion::util::Array<Vec4>& vec4 = *(illusion::util::Array<Vec4>*)data.data;
			ImVec4 vec4aGet(vec4[componentId].x, vec4[componentId].y, vec4[componentId].z, vec4[componentId].w);
			ImGui::Text(data.name.c_str());
			ImGui::SameLine();
			if (ImGui::ColorButton(data.name.c_str(), vec4aGet)) {
				ImGui::OpenPopup(("ColorPicker###ColorPickerComponent_" + data.name).c_str());
			}
			if (ImGui::BeginPopup(("ColorPicker###ColorPickerComponent_" + data.name).c_str())) {
				float vec4a[4] = { vec4[componentId].x, vec4[componentId].y, vec4[componentId].z, vec4[componentId].w };
				ImGui::ColorPicker4(data.name.c_str(), vec4a);
				vec4[componentId].x = vec4a[0];
				vec4[componentId].y = vec4a[1];
				vec4[componentId].z = vec4a[2];
				vec4[componentId].w = vec4a[3];
				ImGui::EndPopup();
			}
			
		} else if (data.type == typeid(illusion::util::Array<f32>).hash_code()) {
			illusion::util::Array<f32>& val = *(illusion::util::Array<f32>*)data.data;
			f32 floatValue = val[componentId];
			ImGui::DragFloat(data.name.c_str(), &floatValue, 0.01f);
			val[componentId] = floatValue;
		} else if (data.type == typeid(illusion::util::Array<u32>).hash_code()) {
			illusion::util::Array<u32>& val = *(illusion::util::Array<u32>*)data.data;
			int floatValue = val[componentId];
			ImGui::DragInt(data.name.c_str(), &floatValue, 0.01f, 0);
			if (floatValue < 0) floatValue = 0;
			val[componentId] = floatValue;
		} else if (data.type == typeid(illusion::util::Array<i32>).hash_code()) {
			illusion::util::Array<i32>& val = *(illusion::util::Array<i32>*)data.data;
			int floatValue = val[componentId];
			ImGui::DragInt(data.name.c_str(), &floatValue, 0.01f);
			if (floatValue < 0) floatValue = 0;
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
		} else if (data.type == typeid(illusion::util::Array<size_t>).hash_code()) {
			illusion::util::Array<size_t>& val = *(illusion::util::Array<size_t>*)data.data;
			int floatValue = val[componentId];
			ImGui::InputInt(data.name.c_str(), &floatValue, 0.01f);
			val[componentId] = floatValue;
		}
	}

	void RenderMeshInstance(const size_t componentKey, MeshInstance *meshInstance, ecs::entity_id selected) {
		ecs::component_id index = meshInstance->getIndex(selected);
		size_t indexMesh = meshInstance->meshId[index];
		size_t indexMaterial = meshInstance->materialId[index];
		if (!GetRenderEngine().ContainsMesh(indexMesh)) indexMesh = 0;
		if (!GetRenderEngine().ContainsMaterial(indexMaterial)) indexMaterial = 0;
		std::string titleMesh = GetRenderEngine().meshes.at(indexMesh).name;
		std::string titleMaterial = GetRenderEngine().materials.at(indexMaterial).name;

		//MESH
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.250f, 0.250f, 0.250f, 1.00f));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.109f, 0.117f, 0.129f, 1.00f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.109f, 0.117f, 0.129f, 1.00f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.109f, 0.117f, 0.129f, 1.00f));
		ImGui::Button(titleMesh.c_str(), ImVec2(ImGui::GetWindowContentRegionWidth() * 0.6f - 25.0f, 0.0f));
		ImGui::PopStyleVar();
		ImGui::PopStyleColor(4);

		ImGui::SameLine();
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
		if (ImGui::Button("Change Mesh###changeMesh")) {
			ImGui::OpenPopup("MeshInstanceChoice");
		}
		ImGui::PopStyleVar();

		//All shapes
		if (ImGui::BeginPopup("MeshInstanceChoice")) {
			if (!ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0))
				ImGui::SetKeyboardFocusHere(0);
			GenerateInputString("###SearchMesh", searchMesh);
			ImGui::Separator();

			util::Array<std::string> groupNames;
			util::Array<int> groupNumbers;
			for (auto const& [key, val] : GetRenderEngine().meshes) {
				if (val.group == "") {
					if (searchMesh != "" && val.name.find(searchMesh) == std::string::npos) continue;
					if (key == indexMesh) ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.50f, 1.00f, 0.50f, 1.00f));
					if (ImGui::Button((val.name + "###meshid_" + std::to_string(key)).c_str())) {
						meshInstance->SetMesh(index, key);
					}
					if (key == indexMesh) ImGui::PopStyleColor();
				}
				else {
					std::vector<std::string>::iterator it = std::find(groupNames.begin(), groupNames.end(), val.group);
					if (it == groupNames.end()) {
						groupNames.push_back(val.group);
						groupNumbers.push_back(1);
					}
					else {
						size_t index = std::distance(groupNames.begin(), it);
						groupNumbers[index]++;
					}
				}
			}
			for (size_t i = 0; i < groupNames.size(); i++) {
				auto const& groupName = groupNames[i];

				if (groupNumbers[i] > 1) {
					//Case when the file contain more than one shape
					if (ImGui::TreeNode((groupName + "###GroupMesh_" + groupName).c_str())) {
						for (auto const& [key, val] : GetRenderEngine().meshes) {
							if (searchMesh != "" && val.name.find(searchMesh) == std::string::npos) continue;
							if (val.group == groupName) {
								if (key == indexMesh) ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.50f, 1.00f, 0.50f, 1.00f));
								if (ImGui::Button((val.name + "###meshid_" + std::to_string(key)).c_str())) {
									meshInstance->SetMesh(index, key);
								}
								if (key == indexMesh) ImGui::PopStyleColor();
							}
						}
						ImGui::TreePop();
					}
				}
				else {
					//Case when the file contain only one shape
					for (auto const& [key, val] : GetRenderEngine().meshes) {
						if (searchMesh != "" && val.name.find(searchMesh) == std::string::npos) continue;
						if (val.group == groupName) {
							if (key == indexMesh) ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.50f, 1.00f, 0.50f, 1.00f));
							if (ImGui::Button((groupName + "###meshid_" + std::to_string(key)).c_str())) {
								meshInstance->SetMesh(index, key);
							}
							if (key == indexMesh) ImGui::PopStyleColor();
							break; //Found the only one element
						}
					}
				}

			}
			ImGui::EndPopup();
		}

		//MATERIAL
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.250f, 0.250f, 0.250f, 1.00f));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.109f, 0.117f, 0.129f, 1.00f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.109f, 0.117f, 0.129f, 1.00f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.109f, 0.117f, 0.129f, 1.00f));
		ImGui::Button(titleMaterial.c_str(), ImVec2(ImGui::GetWindowContentRegionWidth() * 0.6f - 25.0f, 0.0f));
		ImGui::PopStyleVar();
		ImGui::PopStyleColor(4);

		ImGui::SameLine();
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
		if (ImGui::Button("Change Material###changeMaterial")) {
			ImGui::OpenPopup("MaterialInstanceChoice");
		}
		ImGui::PopStyleVar();

		//All shapes
		if (ImGui::BeginPopup("MaterialInstanceChoice")) {
			if (!ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0))
				ImGui::SetKeyboardFocusHere(0);
			GenerateInputString("###SearchMaterial", searchMaterial);
			ImGui::Separator();

			for (auto const& [key, val] : GetRenderEngine().materials) {
				if (searchMaterial != "" && val.name.find(searchMaterial) == std::string::npos) continue;
				if (key == indexMaterial) ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.50f, 1.00f, 0.50f, 1.00f));

				if (ImGui::Button((val.name + "###materialid_" + std::to_string(key)).c_str())) {
					meshInstance->SetMaterial(index, key);
				}

				if (key == indexMaterial) ImGui::PopStyleColor();
			}
			ImGui::EndPopup();
		}
		
	}
	
	void RenderAudioSourceInstance(const size_t componentKey, core::sound::AudioSource* audioSourceInstance, ecs::entity_id selected) {
		ecs::component_id index = audioSourceInstance->getIndex(selected);

		//SOUNDS
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.250f, 0.250f, 0.250f, 1.00f));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.109f, 0.117f, 0.129f, 1.00f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.109f, 0.117f, 0.129f, 1.00f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.109f, 0.117f, 0.129f, 1.00f));
		ImGui::Button(audioSourceInstance->relativePath[index].c_str(), ImVec2(ImGui::GetWindowContentRegionWidth() * 0.6f - 25.0f, 0.0f));
		ImGui::PopStyleVar();
		ImGui::PopStyleColor(4);

		ImGui::SameLine();
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
		if (ImGui::Button("Change Source###changeSourceAudio")) {
			ImGui::OpenPopup("RenderAudioSourceInstanceChoice");
		}
		ImGui::PopStyleVar();

		//All shapes
		if (ImGui::BeginPopup("RenderAudioSourceInstanceChoice")) {
			if (!ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0))
				ImGui::SetKeyboardFocusHere(0);
			GenerateInputString("###SearchAudio", searchAudioSource);
			ImGui::Separator();

			for (auto const& [key, val] : core::sound::GetSoundEngine().sources) {
				if (searchAudioSource != "" && key.find(searchAudioSource) == std::string::npos) continue;
				bool styleAdded = false;
				if (key == audioSourceInstance->relativePath[index]) {
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.50f, 1.00f, 0.50f, 1.00f));
					styleAdded = true;
				}

				if (ImGui::Button((key + "###audioSource_" + key).c_str())) {
					audioSourceInstance->relativePath[index] = key;
				}

				if (styleAdded) ImGui::PopStyleColor();
			}
			ImGui::EndPopup();
		}

		bool paused = audioSourceInstance->paused[index];
		ImGui::Checkbox("Pause###soundPaused", &paused);
		audioSourceInstance->paused[index] = paused;

		bool loop = audioSourceInstance->loop[index];
		ImGui::Checkbox("Loop###soundLoop", &loop);
		audioSourceInstance->loop[index] = loop;

		bool sound3d = audioSourceInstance->is3D[index];
		ImGui::Checkbox("3D Space###3DSoundSpace", &sound3d);
		audioSourceInstance->is3D[index] = sound3d;

		float volume = audioSourceInstance->volume[index];
		ImGui::SliderFloat("Volume###soundVolume", &volume, 0, 1);
		audioSourceInstance->volume[index] = volume;

		if (ImGui::Button("Play###PlaySound")) {
			audioSourceInstance->Play(index);
		}

		//f32 volume = audioSourceInstance->volume[index];
		//ImGui::Checkbox("Volume###soundVolume", &volume);
		//audioSourceInstance->loop[index] = volume;
	}

	void Show() {
		ImGui::Begin("Inspector");

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
			ImGui::SetNextItemWidth(fmaxf(80, ImGui::GetWindowContentRegionWidth() * 0.4f - 25.0f));
			ImGui::LabelText("###Position", "Position");
			ImGui::SameLine(); ImGui::SetNextItemWidth(fmaxf(50, ImGui::GetWindowContentRegionWidth() * 0.2f)); ImGui::DragFloat("###PX", &transform.position[indexTransform].x, 0.01f);
			ImGui::SameLine(); ImGui::SetNextItemWidth(fmaxf(50, ImGui::GetWindowContentRegionWidth() * 0.2f)); ImGui::DragFloat("###PY", &transform.position[indexTransform].y, 0.01f);
			ImGui::SameLine(); ImGui::SetNextItemWidth(fmaxf(50, ImGui::GetWindowContentRegionWidth() * 0.2f)); ImGui::DragFloat("###PZ", &transform.position[indexTransform].z, 0.01f);

			//Rotation
			Vec3 &realEuler = transform.rotationEuler[indexTransform];
			Vec3 euler = transform.rotationEuler[indexTransform];
			ImGui::SetNextItemWidth(fmaxf(80, ImGui::GetWindowContentRegionWidth() * 0.4f - 25.0f));
			ImGui::LabelText("###Rotation", "Rotation");
			ImGui::SameLine(); ImGui::SetNextItemWidth(fmaxf(50, ImGui::GetWindowContentRegionWidth() * 0.2f)); ImGui::DragFloat("###RX", &euler.x, 0.1f);
			ImGui::SameLine(); ImGui::SetNextItemWidth(fmaxf(50, ImGui::GetWindowContentRegionWidth() * 0.2f)); ImGui::DragFloat("###RY", &euler.y, 0.1f);
			ImGui::SameLine(); ImGui::SetNextItemWidth(fmaxf(50, ImGui::GetWindowContentRegionWidth() * 0.2f)); ImGui::DragFloat("###RZ", &euler.z, 0.1f);

			if (realEuler != euler) {
				realEuler = euler;
				transform.rotation[indexTransform] = glm::tquat(glm::radians(euler));
				transform.needUpdateEuler[indexTransform] = false;
			}

			//Scale
			ImGui::SetNextItemWidth(fmaxf(80, ImGui::GetWindowContentRegionWidth() * 0.4f - 25.0f));
			ImGui::LabelText("###Scale", "Scale");
			ImGui::SameLine(); ImGui::SetNextItemWidth(fmaxf(50, ImGui::GetWindowContentRegionWidth() * 0.2f)); ImGui::DragFloat("###SX", &transform.scale[indexTransform].x, 0.01f);
			ImGui::SameLine(); ImGui::SetNextItemWidth(fmaxf(50, ImGui::GetWindowContentRegionWidth() * 0.2f)); ImGui::DragFloat("###SY", &transform.scale[indexTransform].y, 0.01f);
			ImGui::SameLine(); ImGui::SetNextItemWidth(fmaxf(50, ImGui::GetWindowContentRegionWidth() * 0.2f)); ImGui::DragFloat("###SZ", &transform.scale[indexTransform].z, 0.01f);

			ImGui::Dummy(ImVec2(0.0f, 5.0f));
			ImGui::Separator();
			ImGui::Dummy(ImVec2(0.0f, 5.0f));

			for (auto const& [key, val] : currentScene->components) {
				if (val->getName() == "Transform") continue;

				std::string title = val->getName() + "###" + std::to_string(key);
				if (val->getIndex(currentSelected) == ecs::id::invalid_id) continue;
				if (ImGui::TreeNode(title.c_str())) {
					if (val->getName() == "Mesh Instance") RenderMeshInstance(key, static_cast<MeshInstance*>(val), currentSelected);
					else if (val->getName() == "AudioSource") RenderAudioSourceInstance(key, static_cast<core::sound::AudioSource*>(val), currentSelected);
					else {
						for (u32 i = 0; i < val->publicDatas.size(); i++) {
							if (val->publicDatas[i].visible)
								GenerateUiComponent(val->publicDatas[i], val);
						}
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
							currentScene->EmitComponentAdded(currentSelected, key);
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