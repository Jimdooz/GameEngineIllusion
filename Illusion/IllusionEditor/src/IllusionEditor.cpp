#include "ecs/Entity.h"
#include "ecs/Component.h"
#include "ecs/System.h"
#include "ecs/Scene.h"
#include "IllusionEngine.h"

#include "views/UiTheme.h"
#include "views/GameInspector.h"
#include "views/GameHiearchy.h"

#include <fstream>
#include <resources/system/Json.h>
using json = illusion::json;

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
	// Declare component name
	COMPONENT_NAME("Rigidbody");

	// Declare constructor
	RigidBodyComponent(ecs::Scene* scene) : Component(scene) {
		// Display on inspector
		COMPONENT_PUBLIC(velocity);
	}

	// Declare datas
	COMPONENT_DATA(Vec3, velocity);

	// On Data added
	virtual void AddDatas(ecs::entity_id id) override {
		AddData(velocity, Vec3(0, 0, 0));
	}

	// On Data removed
	virtual void RemoveDatas(ecs::component_id index, ecs::entity_id id) {
		RemoveData(velocity, index);
	}
};

struct TwerkSystem : public ecs::System {
	SYSTEM_NAME("TWERK");

	ecs::core::Transform* transform;
	RigidBodyComponent* rigidbody;

	/* la fonction Update */
	SYSTEM_UPDATE_LOOP(
		/*position().x += 5;
		rotation().x++;
		rotation().y++;
		scale().x++;*/
	)

	/* Definition des variables utiles */
	SYSTEM_USE_DATA(position, transform, position, Vec3)
	SYSTEM_USE_DATA(rotation, transform, rotation, Quaternion)
	SYSTEM_USE_DATA(scale, transform, scale, Vec3)

	/* Initialisation relative a la scene parente */
	virtual void Initialize(ecs::Scene& scene) override {
		transform = scene.GetComponent<ecs::core::Transform>();
		rigidbody = scene.GetComponent<RigidBodyComponent>();
		SetDependencies(transform, rigidbody);
	}
};

json GenerateSaveDataComponent(const illusion::ecs::PublicComponentDatas& data, illusion::ecs::Component* component, ecs::entity_id id) {
	ecs::component_id componentId = component->getIndex(id);
	if (component->getIndex(id) == ecs::id::invalid_id) return nullptr;

	if (data.type == typeid(illusion::util::Array<Vec3>).hash_code()) {
		illusion::util::Array<Vec3>& vec3 = *(illusion::util::Array<Vec3>*)data.data;
		json array = json::array();
		array.push_back(vec3[componentId].x);
		array.push_back(vec3[componentId].y);
		array.push_back(vec3[componentId].z);
		return array;
	}
	else if (data.type == typeid(illusion::util::Array<Quaternion>).hash_code()) {
		illusion::util::Array<Quaternion>& vec4 = *(illusion::util::Array<Quaternion>*)data.data;
		float vec4a[4] = { vec4[componentId].w, vec4[componentId].x, vec4[componentId].y, vec4[componentId].z };
		json array = json::array();
		array.push_back(vec4[componentId].w);
		array.push_back(vec4[componentId].x);
		array.push_back(vec4[componentId].y);
		array.push_back(vec4[componentId].z);
		return array;
	}
	else if (data.type == typeid(illusion::util::Array<ecs::entity_id>).hash_code()) {
		illusion::util::Array<ecs::entity_id>& ids = *(illusion::util::Array<ecs::entity_id>*)data.data;
		if (!ecs::id::IsValid(ids[componentId])) return nullptr;
		json id = ecs::id::Index(ids[componentId]);
		return id;
	}

	return nullptr;
}

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

	json jsonScene = {};
	jsonScene["Components"] = json::array();
	jsonScene["Systems"] = json::array();
	jsonScene["Entity"] = {};

	for (auto const& [key, val] : scene.components) {
		jsonScene["Components"].push_back(std::to_string(key));
	}
	for (auto const& [key, val] : scene.systems) {
		jsonScene["Systems"].push_back(std::to_string(key));
	}

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

		jsonScene["Entity"] = json::object();
		jsonScene["Entity"]["Components"] = json::object();
		jsonScene["Entity"]["id"] = json::array();
		for (auto const& [key, val] : scene.components) {
			jsonScene["Entity"]["Components"][std::to_string(key)] = json::object();
			for (u32 i = 0; i < val->ToEntity.size(); i++) {
				jsonScene["Entity"]["Components"][std::to_string(key)][std::to_string(ecs::id::Index(val->ToEntity[i]))] = json::object();
				for (u32 j = 0; j < val->publicDatas.size(); j++) {
					jsonScene["Entity"]["Components"][std::to_string(key)][std::to_string(ecs::id::Index(val->ToEntity[i]))][val->publicDatas[j].name] = GenerateSaveDataComponent(val->publicDatas[j], val, scene.entities.GetId(ecs::id::Index(val->ToEntity[i])));
				}
			}
		}
		for (u32 i = 0; i < scene.entities.m_entities.size(); i++) {
			if(scene.entities.IsAliveAtIndex(ecs::entity_id(i))) jsonScene["Entity"]["id"].push_back(i);
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

		{
			ImGui::Begin("Json Test");

			if (ImGui::Button("Save")) {
				std::ofstream myfile;
				myfile.open("D:/GitHub/GameEngineIllusion/GameProjects/project1/scene2.json");
				myfile << jsonScene.dump(4);
				myfile.close();
			}

			ImGui::Text(jsonScene.dump(4).c_str());

			ImGui::End();
		}

		views::GameHiearchy::SetScene(scene);
		views::GameHiearchy::Show();

		views::GameInspector::SetScene(scene);
		views::GameInspector::SetSelected(views::GameHiearchy::selected);
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
