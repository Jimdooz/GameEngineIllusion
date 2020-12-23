#include "ecs/Entity.h"
#include "ecs/Component.h"
#include "ecs/System.h"
#include "ecs/Scene.h"
#include "IllusionEngine.h"

#include "views/UiTheme.h"
#include "views/GameInspector.h"
#include "views/GameHiearchy.h"

#include <iostream>
#include <vector>
#include <bitset>
#include <chrono>
#include <map>
#include <string>
//auto start = high_resolution_clock::now();
//auto stop = high_resolution_clock::now();
//std::cout << "DURATION : " << duration_cast<microseconds>(stop - start).count() / 1000000.0 << std::endl;

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

using namespace std::chrono;
using namespace illusion;

struct RigidBodyComponent : public ecs::Component {
	RigidBodyComponent(ecs::Scene* scene) : Component(scene) {}

	COMPONENT_NAME("RIGIDBODY");

	COMPONENT_DATA(Vec3, velocity);

	virtual void AddDatas(ecs::entity_id id) override {
		AddData(velocity, Vec3(0, 0, 0));
	}
	virtual void RemoveDatas(ecs::component_id index, ecs::entity_id id) {
		RemoveData(velocity, index);
	}
};

///--> SYSTEMS
struct TwerkSystem : public ecs::System {
	SYSTEM_NAME("TWERK");

	ecs::core::Transform* transform;

	/* la fonction Update */
	SYSTEM_UPDATE_LOOP(
		position().x += 5;
		rotation().x++;
		rotation().y++;
		scale().x++;
	)

	/* D?finition des variables utiles */
	SYSTEM_USE_DATA(position, transform, position, Vec3)
	SYSTEM_USE_DATA(rotation, transform, rotation, Quaternion)
	SYSTEM_USE_DATA(scale, transform, scale, Vec3)

	/* Initialisation relative ? la sc?ne parente */
	virtual void Initialize(ecs::Scene& scene) override {
		transform = scene.GetComponent<ecs::core::Transform>();
		SetDependencies(transform);
	}
};

bool itemAlreadyDropped = false;
ecs::entity_id selected = (ecs::entity_id)(ecs::id::invalid_id);
float position[] = { 0 , 0 , 0 };
float rotation[] = { 0 , 0 , 0 };
float scale[] = { 1, 1 , 1 };

void ShowChild(ecs::entity_id parentId, ecs::core::Transform *transform, ecs::Scene &scene) {
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

float* my_color = new float(0);

int main(int argc, char* argv[]) {
	// Create Window
	//--------
	Window::Create(1280,720,"MyGame");

	// Setup IMGUI
	//--------
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(Window::glfwWindow, true);
	ImGui_ImplOpenGL3_Init((char*)glGetString(GL_NUM_SHADING_LANGUAGE_VERSIONS));
	illusion::views::theme::InitTheme();

	// Init Scene
	//----------
	ecs::Scene scene;
	scene.UseComponent<RigidBodyComponent>();
	scene.UseSystem<TwerkSystem>();
	ecs::core::Transform* transforms = scene.GetComponent<ecs::core::Transform>();	

	std::vector<float> fpsMesure;

	// Main Loop
	//---------
	while (!Window::shouldClose) {
		Input::Update();
		if (Input::isKeyDown(GLFW_KEY_A)) {
			INFO("PRESSED KEY A");
		}
		if (Input::getMouseWheelDelta() != 0)
		{
			INFO("MOUSE WHEEL : ", Input::getMouseWheelDelta());
		}
		if (Input::isKeyDown(GLFW_KEY_ESCAPE)) {
			INFO("PRESSED KEY ESCAPE");
			Window::Close();
		}

		//New Frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		{
			ImGui::Begin("Speed Test");

			float fps[100];
			float average = 0.0;
			for (u32 i = 0; i < fpsMesure.size() && i < 100; i++) {
				fps[i] = fpsMesure[i];
				average += fps[i];
			}
			std::string plotLineTitle = std::to_string((u32)round(average / 100.0)) + " fps";
			ImGui::Text(plotLineTitle.c_str());
			ImGui::PlotLines("###fpsPlotLines", fps, 100);

			ImGui::End();
		}

		views::GameHiearchy::SetScene(scene);
		views::GameHiearchy::SetSelected(&selected);
		views::GameHiearchy::Show();

		views::GameInspector::SetScene(scene);
		views::GameInspector::SetSelected(selected);
		views::GameInspector::Show();

		//Render
		ImGui::Render();
		int display_w, display_h;
		glfwGetFramebufferSize(Window::glfwWindow, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(0.25f, 0.25f, 0.5f, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		auto start = high_resolution_clock::now();
		scene.Update();
		auto stop = high_resolution_clock::now();
		fpsMesure.push_back(1.0 / ( duration_cast<microseconds>(stop - start).count() / 1000000.0 ));
		if (fpsMesure.size() > 100) fpsMesure.erase(fpsMesure.begin());
		
		glfwSwapBuffers(Window::glfwWindow);
	}
	Window::Destroy();

	//Shutdown
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();

	ImGui::DestroyContext();
}