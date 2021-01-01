#pragma once

#include "Input.h"

#include "ecs/Ecs.h"
#include "ecs/Component.h"
#include "ecs/System.h"
#include "ecs/Scene.h"

#include "project/ProjectManager.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

namespace illusioneditor::views::EditorMenuBar {

	using namespace illusion;

	namespace {
		illusion::ecs::Scene* scene = nullptr;
	}

	void Show() {
		//Shortcut
		if (Input::isKey(GLFW_KEY_LEFT_CONTROL) && Input::isKeyDown(GLFW_KEY_S)) illusioneditor::project::tools::SaveScene(*scene);

		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Save", "CTRL+S")) {
					illusioneditor::project::tools::SaveScene(*scene);
				}
				if (ImGui::MenuItem("Save As", "CTRL+Shift+S")) {}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Edit"))
			{
				if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
				if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
				ImGui::Separator();
				if (ImGui::MenuItem("Cut", "CTRL+X")) {}
				if (ImGui::MenuItem("Copy", "CTRL+C")) {}
				if (ImGui::MenuItem("Paste", "CTRL+V")) {}
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}
	}

	void SetScene(illusion::ecs::Scene& currScene) {
		scene = &currScene;
	}
}