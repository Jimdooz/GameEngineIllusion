#pragma once

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "ecs/Scene.h"

#include "resources/assets/Shaders.h"
#include "resources/assets/Materials.h"

namespace illusioneditor::views::MaterialEditor {

	using namespace illusion;

	namespace {
		illusion::resources::assets::MaterialResource* currentMaterial = nullptr;
		illusion::ecs::Scene* currentScene;
	}

	static void GeneraterUiShaderPicker(illusion::ecs::Scene* scene, illusion::resources::assets::MaterialResource* material) {
		Shader *shader;
		if (!GetRenderEngine().ContainsShader(material->shaderId)) shader = &GetRenderEngine().defaultShader();
		else shader = &GetRenderEngine().shaders[material->shaderId];

		std::string titleShader = shader->resource.name + "###" + std::to_string(shader->resource.id);

		//Shaders
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.250f, 0.250f, 0.250f, 1.00f));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.109f, 0.117f, 0.129f, 1.00f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.109f, 0.117f, 0.129f, 1.00f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.109f, 0.117f, 0.129f, 1.00f));
		ImGui::Button(titleShader.c_str(), ImVec2(ImGui::GetWindowContentRegionWidth() * 0.6f - 25.0f, 0.0f));
		ImGui::PopStyleVar();
		ImGui::PopStyleColor(4);

		ImGui::SameLine();
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
		if (ImGui::Button("Change Shader###changeMaterial")) {
			ImGui::OpenPopup("ShaderInstanceChoice");
		}
		ImGui::PopStyleVar();

		//All shapes
		if (ImGui::BeginPopup("ShaderInstanceChoice")) {
			ImGui::Text("Shaders");
			for (auto const& [key, val] : GetRenderEngine().shaders) {
				if (ImGui::Button((val.resource.name + "###shaderid_" + std::to_string(key)).c_str())) {
					material->shaderId = key;
					material->relativeShaderPath = GetRenderEngine().shaders[key].resource.relativePath;
					illusion::resources::assets::SaveMaterial(*material);
					illusion::resources::assets::LoadAllMaterials();
					scene->renderer->ReGenerateMeshByShader();
				}
			}
			ImGui::EndPopup();
		}
	}

	static void GenerateUiMaterial(illusion::ecs::Scene* scene, illusion::resources::assets::MaterialResource* material) {
		if (!GetRenderEngine().ContainsShader(material->shaderId)) return;
		Shader &shader = GetRenderEngine().shaders[material->shaderId];

		bool needToSave = false;

		for (json::iterator it = shader.resource.uniforms.begin(); it != shader.resource.uniforms.end(); ++it) {
			std::string key = it.key();
			json &value = material->uniforms[key];
			const std::string typeV = it.value()["type"];
			if (value.is_null()) value = it.value()["default"];

			std::string title = key + "###" + key + typeV;

			if (typeV == "Vec2") {
				float v[] = { value[0], value[1] };
				ImGui::DragFloat3(title.c_str(), v, 0.01f);
				if (value[0] != v[0] || value[1] != v[1]) needToSave = true;
				value[0] = v[0]; value[1] = v[1];
			} else if (typeV == "Vec3") {
				float v[] = { value[0], value[1], value[2] };
				ImGui::DragFloat3(title.c_str(), v, 0.01f);
				if (value[0] != v[0] || value[1] != v[1] || value[2] != v[2]) needToSave = true;
				value[0] = v[0]; value[1] = v[1]; value[2] = v[2];
			} else if (typeV == "Vec4") {
				float v[] = { value[0], value[1], value[2], value[3] };
				ImGui::DragFloat4(title.c_str(), v, 0.01f);
				if (value[0] != v[0] || value[1] != v[1] || value[2] != v[2] || value[3] != v[3]) needToSave = true;
				value[0] = v[0]; value[1] = v[1]; value[2] = v[2]; value[3] = v[3];
			} else if (typeV == "Color3") {
				ImVec4 v(value[0], value[1], value[2], 1.0);
				ImGui::Text(key.c_str());
				ImGui::SameLine();
				if (ImGui::ColorButton(title.c_str(), v)) ImGui::OpenPopup(("ColorPicker###ColorPickerComponent_" + key + typeV).c_str());
				if (ImGui::BeginPopup(("ColorPicker###ColorPickerComponent_" + key + typeV).c_str())) {
					float vec4a[4] = { value[0], value[1], value[2], 1.0 };
					ImGui::ColorPicker4((title + "picker").c_str(), vec4a);
					if (value[0] != vec4a[0] || value[1] != vec4a[1] || value[2] != vec4a[2]) needToSave = true;
					value[0] = vec4a[0];
					value[1] = vec4a[1];
					value[2] = vec4a[2];
					ImGui::EndPopup();
				}
			} else if (typeV == "Color4") {
				ImVec4 v(value[0], value[1], value[2], value[3]);
				ImGui::Text(key.c_str());
				ImGui::SameLine();
				if (ImGui::ColorButton(title.c_str(), v)) ImGui::OpenPopup(("ColorPicker###ColorPickerComponent_" + key + typeV).c_str());
				if (ImGui::BeginPopup(("ColorPicker###ColorPickerComponent_" + key + typeV).c_str())) {
					float vec4a[4] = { value[0], value[1], value[2], value[3] };
					ImGui::ColorPicker4((title + "picker").c_str(), vec4a);
					if (value[0] != vec4a[0] || value[1] != vec4a[1] || value[2] != vec4a[2] || value[3] != vec4a[3]) needToSave = true;
					value[0] = vec4a[0];
					value[1] = vec4a[1];
					value[2] = vec4a[2];
					value[3] = vec4a[3];
					ImGui::EndPopup();
				}
			}
		}

		if(needToSave) illusion::resources::assets::SaveMaterial(*material);
	}

	static void Show() {
		if (currentMaterial == nullptr) return;

		ImGui::Begin("Material Editor");

		ImGui::Text(("Material : " + currentMaterial->name).c_str());

		ImGui::Dummy(ImVec2(0.0f, 5.0f));
		GeneraterUiShaderPicker(currentScene, currentMaterial);
		ImGui::Dummy(ImVec2(0.0f, 10.0f));
		GenerateUiMaterial(currentScene, currentMaterial);
		ImGui::Dummy(ImVec2(0.0f, 5.0f));

		if (ImGui::Button("Close###CloseMaterialEditor")) {
			currentMaterial = nullptr;
		}

		ImGui::End();
	}

	static void Close() {
		currentMaterial = nullptr;
	}

	static void EditMaterialPath(illusion::ecs::Scene *scene, std::string materialPath) {
		if (scene == nullptr) return;
		size_t idMaterial = illusion::resources::assets::IdMaterialPath(materialPath);

		if (GetRenderEngine().ContainsMaterial(idMaterial)) {
			currentMaterial = &GetRenderEngine().materials[idMaterial];
			currentScene = scene;
		}
	}
}