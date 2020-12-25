#include "ecs/Entity.h"
#include "ecs/Component.h"
#include "ecs/System.h"
#include "ecs/Scene.h"
#include "IllusionEngine.h"

#include "views/UiTheme.h"
#include "views/GameInspector.h"
#include "views/GameHiearchy.h"

#include "resources/DataConvertor.h"
#include "resources/assets/Scenes.h"

#include <streambuf>
#include <sstream>
#include <resources/system/Json.h>
using json = illusion::json;

#include <fstream>
#include <iostream>
#include <filesystem>
namespace fs = std::filesystem;

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
using namespace illusioneditor;

struct RigidBodyComponent : public ecs::Component {
	// Declare component name
	COMPONENT_NAME("Rigidbody");
	COMPONENT_REGISTER(RigidBodyComponent);

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
	SYSTEM_REGISTER(TwerkSystem);

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

int main(int argc, char* argv[]) {
	// Create Window
	//--------
	Window::Create(1280,720,"MyGame");

	for (auto& p : fs::directory_iterator("D:/")) {
		std::cout << p.path() << '\n';
		if (p.is_directory()) {
			for (auto& p2 : fs::directory_iterator(p.path())) {
				//std::cout<< "\t" << p2.path() << '\n';
			}
		}
	}

	// Setup IMGUI
	//--------
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(Window::glfwWindow, true);
	ImGui_ImplOpenGL3_Init((char*)glGetString(GL_NUM_SHADING_LANGUAGE_VERSIONS));
	views::theme::InitTheme();

	// Init Engine
	//----------
	illusion::resources::JsonConvertor::Initialize();

	// Init ECS
	//----------
	illusion::ecs::Component::AppendCoreComponents();
	illusion::ecs::Component::AppendComponents<RigidBodyComponent>();
	illusion::ecs::System::AppendSystems<TwerkSystem>();

	// Init Scene
	//----------
	ecs::Scene scene;

	json jsonLoaded;
	{
		std::ifstream t("D:/GitHub/GameEngineIllusion/GameProjects/project1/scene2.json");
		std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
		jsonLoaded = json::parse(str);
	}

	if (jsonLoaded.is_null()) {
		scene.UseComponent<RigidBodyComponent>();
		scene.UseSystem<TwerkSystem>();
	}
	else {
		resources::assets::LoadScene(scene, jsonLoaded);
	}

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
			if (ImGui::BeginMainMenuBar())
			{
				if (ImGui::BeginMenu("File"))
				{
					if (ImGui::MenuItem("Save", "CTRL+S")) {
						std::ofstream myfile;
						myfile.open("D:/GitHub/GameEngineIllusion/GameProjects/project1/scene2.json");
						myfile << resources::assets::ExportScene(scene).dump(4);
						myfile.close();
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

		{
			ImGui::Begin("Game Engine Stats");

			float fps[100];
			float average = 0.0;
			for (u32 i = 0; i < fpsMesure.size() && i < 100; i++) {
				fps[i] = 1.0 / std::max(fpsMesure[i] / 1000000.0, 0.000001);
				average += fpsMesure[i];
			}
			int fpsGet = (int)round(1.0 / ((average / 100.0) / 1000000.0));
			std::string plotLineTitle = fpsGet > 100000 ? ":) lot of fps" : (std::to_string(fpsGet) + " fps");
			std::string elapsedTime = std::to_string((average / 100.0) / 1000000.0) + " s";
			ImGui::Text(plotLineTitle.c_str());
			ImGui::SameLine();
			ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor::ImColor(100, 100, 100));
			ImGui::Text(elapsedTime.c_str());
			ImGui::PopStyleColor(1);
			ImGui::PlotLines("Update speed###fpsPlotLines", fps, 100);


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
		auto duration = duration_cast<microseconds>(stop - start).count();
		fpsMesure.push_back(duration > 0 ? duration : 1);
		if (fpsMesure.size() > 100) fpsMesure.erase(fpsMesure.begin());

		glfwSwapBuffers(Window::glfwWindow);
	}
	Window::Destroy();

	//Shutdown
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();

	ImGui::DestroyContext();
}
