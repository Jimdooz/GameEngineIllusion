#include "ecs/Entity.h"
#include "ecs/Component.h"
#include "ecs/System.h"
#include "ecs/Scene.h"
#include "IllusionEngine.h"

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

///--> SYSTEMS
struct TwerkSystem : public ecs::System {

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

	// Init Scene
	//----------
	ecs::Scene scene;
	scene.UseSystem<TwerkSystem>();

	for (u32 i = 0; i < 10; i++)
		scene.CreateEntity();
	
	ecs::core::Transform *transforms = scene.GetComponent<ecs::core::Transform>();

	std::vector<float> fpsMesure;

	// Main Loop
	//---------
	while (!Window::shouldClose) {
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
			ImGui::Begin("Hello, world!");

			float fps[100];
			float average = 0.0;
			for (u32 i = 0; i < fpsMesure.size() && i < 100; i++) {
				fps[i] = fpsMesure[i];
				average += fps[i];
			}
			std::string plotLineTitle = "Frame Times : " + std::to_string((u32)round(average / 100.0)) + " fps";
			ImGui::PlotLines(plotLineTitle.c_str(), fps, 100);

			std::string ButtonTitle = "Add Entity (" + std::to_string(transforms->ToEntity.size()) + ")";
			if (ImGui::Button(ButtonTitle.c_str())) {
				scene.CreateEntity();
			}

			/*u32 nb = transforms->ToEntity.size();
			for (u32 i = 0; i < nb; i++) {
				std::string name = std::to_string(transforms->ToEntity[i]);
				if (ImGui::Button(name.c_str())) {
					scene.DestroyEntity(transforms->ToEntity[i]);
					nb--;
				}
			}*/
			/*if (ImGui::TreeNode("Loxel Entities"))
			{
				if (ImGui::TreeNode("Base"))
				{
					ImGui::Indent();
					ImGui::Text("Num Slots");
					ImGui::Text("Count");
					ImGui::Unindent();
					ImGui::TreePop();
				}
				if (ImGui::TreeNode("Slots"))
				{
					ImGui::TreePop();
				}
				ImGui::TreePop();
			}
			ImGui::Indent();
			ImGui::Text("Previous Modifications");
			ImGui::Text("Debug Ticks");
			ImGui::Unindent();*/
			ImGui::End();
		}

		//Render
		ImGui::Render();
		int display_w, display_h;
		glfwGetFramebufferSize(Window::glfwWindow, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(0.0, 1.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		auto start = high_resolution_clock::now();
		scene.Update();
		auto stop = high_resolution_clock::now();
		fpsMesure.push_back(1.0 / ( duration_cast<microseconds>(stop - start).count() / 1000000.0 ));
		if (fpsMesure.size() > 100) fpsMesure.erase(fpsMesure.begin());
		
		Input::Update();
		glfwSwapBuffers(Window::glfwWindow);
	}
	Window::Destroy();

	//Shutdown
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();

	ImGui::DestroyContext();

	/*
	ecs::Scene scene1; // E C S
	scene1.UseSystem<TwerkSystem>();


	f64 average = 0.0;
	f64 timeIteration = 50;

	ecs::entity_id id;
	ecs::entity_id firstId;
	for (u32 i = 0; i < 1000; i++) {
		id = scene1.CreateEntity();
	}

	INTERNAL_INFO("INIT END")

	for (int test = 0; test < timeIteration; test++) {
		auto start = high_resolution_clock::now();

		scene1.Update();
		//scene1.DestroyEntity(firstId);

		auto stop = high_resolution_clock::now();
		average += duration_cast<microseconds>(stop - start).count() ;
		std::cout << "DURATION : " << duration_cast<microseconds>(stop - start).count() / 1000000.0 << std::endl;
	}

	std::cout << "AVERAGE : " << 1.0 / ((average / 1000000.0) / timeIteration) << " FPS" << std::endl;
	*/
}