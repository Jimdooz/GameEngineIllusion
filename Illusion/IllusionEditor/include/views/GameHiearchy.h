#pragma once

#include "ecs/Ecs.h"
#include "ecs/Component.h"
#include "ecs/System.h"
#include "ecs/Scene.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

namespace illusion::views::GameHiearchy {

	using namespace illusion;

	namespace {
		illusion::ecs::Scene* scene = nullptr;

		bool itemAlreadyDropped = false;
		ecs::entity_id selected = (ecs::entity_id)ecs::id::invalid_id;
	}

	void ShowChild(ecs::entity_id parentId, ecs::core::Transform* transform, ecs::Scene& scene) {
		ecs::component_id parentIndex = transform->getIndex(parentId);
		if (parentIndex != ecs::id::invalid_id) {
			util::Array<ecs::entity_id>& childs = transform->childs[parentIndex];
			std::string name = std::to_string(ecs::id::Index(parentId)) + " [" + std::to_string(ecs::id::Generation(parentId)) + "]";

			bool open = ImGui::TreeNodeEx(name.c_str(),
				ImGuiTreeNodeFlags_FramePadding | (selected == parentId ? ImGuiTreeNodeFlags_Selected : 0) | (childs.empty() ? ImGuiTreeNodeFlags_Leaf : 0),
				"Entity %s", name.c_str());
			if (ImGui::IsItemClicked()) {
				itemAlreadyDropped = false;
				selected = parentId;
			}
			if (ImGui::BeginDragDropTarget()) {
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("MoveEntity")) {
					ecs::entity_id id;
					memcpy(&id, payload->Data, sizeof(ecs::entity_id));
					if (!itemAlreadyDropped) {
						transform->SetParent(id, parentId);
						itemAlreadyDropped = true;
					}
				}
				ImGui::EndDragDropTarget();
			}
			if (ImGui::BeginDragDropSource()) {
				ImGui::Text("Moving Entity");
				ImGui::SetDragDropPayload("MoveEntity", &parentId, sizeof(ecs::entity_id), ImGuiCond_Once);
				ImGui::EndDragDropSource();
			}

			std::string popupName = "Menu Entity###" + std::to_string(parentId);

			if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(1)) {
				selected = parentId;
				ImGui::OpenPopup(popupName.c_str());
			}

			if (ImGui::BeginPopup(popupName.c_str())) {
				if (ImGui::MenuItem("Delete")) {
					scene.DestroyEntity(parentId);
					selected = (ecs::entity_id)ecs::id::invalid_id;
				}

				if (ImGui::MenuItem("Soft Delete")) {
					for (u32 i = 0; i < childs.size();) {
						transform->SetParent(childs[0], transform->parent[parentIndex]);
					}
					scene.DestroyEntity(parentId);
					selected = (ecs::entity_id)ecs::id::invalid_id;
				}

				ImGui::EndPopup();
			}

			if (open) {
				for (u32 i = 0; i < childs.size(); i++) {
					ShowChild(childs[i], transform, scene);
				}
				ImGui::TreePop();
			}
		}
	}

	void Show() {
		ImGui::Begin("Hierarchy [DEV]");
		ecs::core::Transform* transforms = scene->GetComponent<ecs::core::Transform>();

		std::string ButtonTitle = "Add Entity (" + std::to_string(transforms->ToEntity.size()) + ")";
		if (ImGui::Button(ButtonTitle.c_str())) {
			scene->CreateEntity();
		}

		//u32 nb = transforms->ToEntity.size();
		for (u32 i = 0; i < transforms->ToEntity.size(); i++) {
			if (transforms->parent[i] == ecs::id::invalid_id) {
				ShowChild(transforms->ToEntity[i], transforms, *scene);
			}
		}

		ImGui::End();
	}

	void SetScene(illusion::ecs::Scene& currScene) {
		scene = &currScene;
	}

	void SetSelected(illusion::ecs::entity_id currSelected) {
		selected = currSelected;
	}
} 