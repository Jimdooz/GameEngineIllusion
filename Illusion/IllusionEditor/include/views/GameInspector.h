#pragma once

#include "ecs/Ecs.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

namespace illusion::views::GameInspector {

	namespace {
		illusion::ecs::Scene* currentScene = nullptr;
		illusion::ecs::entity_id currentSelected;
	}
	
	void Show() {
		ImGui::Begin("Inspector [DEV]");

		if (currentSelected != ecs::id::invalid_id && currentScene != nullptr) {

			ecs::component_id iSelected = (ecs::component_id)ecs::id::Index(currentSelected);

			ImGui::Text("> Components");
			for (auto const& [key, val] : currentScene->components) {
				if (val->ToData[iSelected] != ecs::id::invalid_id) {
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
			ImGui::Text("> Systems");
			for (auto const& [key, val] : currentScene->systems) {
				if (val->ToData[iSelected] != ecs::id::invalid_id) ImGui::Text(val->getName().c_str());
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