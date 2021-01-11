#include "ecs/Entity.h"
#include "ecs/Component.h"
#include "ecs/System.h"
#include "ecs/Scene.h"
#include "IllusionEngine.h"

#include "views/UiTheme.h"
#include "views/GameStats.h"
#include "views/GameInspector.h"
#include "views/GameHierarchy.h"
#include "views/GameProject.h"

#include "views/EditorMenuBar.h"
#include "views/FileExplorer.h"

#include "views/MaterialEditor.h"

#include "resources/DataConvertor.h"
#include "resources/assets/Scenes.h"

#include "ecs/CoreComponents/Camera.h"

#include "core/Time.h"
#include "core/physics/PhysicsManager.h"
#include "core/physics/collisions/Collisions.h"
#include "core/physics/components/BoxCollider.h"
#include "core/physics/components/SphereCollider.h"
#include "core/physics/components/RigidBody.h"

#include "scene/SceneEditor.h"

#include "project/ProjectManager.h"

#include "core/rendering/shapes/defaultShapes.h"

#include <iostream>
#include <vector>
#include <bitset>
#include <map>
#include <string>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

//TEMP RENDERING
#include "core/rendering/Shader.h"
#include "core/rendering/Renderer.h"

//Scripting
#include "scripting/JumpBigCube.h"
#include "scripting/Planet.h"

#include "core/rendering/Renderer.h"

using namespace std::chrono;
using namespace illusion;
using namespace illusioneditor;
using namespace illusion::core::physics;


int main(int argc, char* argv[]) {
	// Create Window
	//--------
	Window::Create(1792, 1008,"Illusion Engine");

	// Test [Romain Saclier]
	//--------

	// Setup IMGUI
	//--------
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigWindowsMoveFromTitleBarOnly = true;
	ImGui_ImplGlfw_InitForOpenGL(Window::glfwWindow, true);
	ImGui_ImplOpenGL3_Init((char*)glGetString(GL_NUM_SHADING_LANGUAGE_VERSIONS));
	views::theme::InitTheme();

	// Init Engine
	//----------
	illusion::resources::JsonConvertor::Initialize();
	illusion::Time::Init();
	illusion::defaultshape::Initialize();

	// Init Editor
	//----------
	illusioneditor::scene::editor::Initialize();
	Shader::Initialize();

	// Init ECS
	//----------
	illusion::ecs::Component::AppendCoreComponents();

	illusion::ecs::Component::AppendComponents<PlanetComponent>();
	illusion::ecs::Component::AppendComponents<JumpBigCube>();
	//Systems
	illusion::ecs::System::AppendCoreSystems();

	illusion::ecs::System::AppendSystems<PlanetSystem>();
	illusion::ecs::System::AppendSystems<JumpBigCubeSystem>();

	//Load Project
	illusioneditor::project::tools::LoadProject("..\\..\\GameProjects\\Optimulus");

	// Init Scene
	//----------
	ecs::Scene scene;
	scene.UseComponent<PlanetComponent>();
	scene.UseComponent<JumpBigCube>();
	scene.UseComponent<MeshInstance>();
	scene.UseComponent<animation::Skeleton>();

	scene.UseSystem<PlanetSystem>();
	scene.UseSystem<JumpBigCubeSystem>();

	//Create Camera
	ecs::entity_id entity = scene.CreateEntity();
	scene.GetComponent<ecs::core::Transform>()->name[entity] = "Camera";
	scene.GetComponent<ecs::core::Transform>()->position[entity] = Vec3(0,0,3);
	scene.EntityAddComponent<ecs::core::Camera>(entity);

	std::vector<float> fpsMesure;

	std::filesystem::directory_entry currentPath("..\\GameProjects\\Optimulus");
	std::string pathName = currentPath.path().string();
	std::string projectName = "";
	
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glEnable(GL_DEPTH_TEST);
	

	f32 physicsTime = 0.0f;

	bool stepMode = false;

	bool ActiveGUI = true;

	// Main Loop
	//---------
	while (!Window::shouldClose) {
		views::GameStats::StartChronoData("Game");

		//Update time
		illusion::Time::UpdateTime(glfwGetTime());

		Input::Update();

		float x = (2.0f * Input::getMousePos().x) / Window::width - 1.0f;
		float y = 1.0f - (2.0f * Input::getMousePos().y) / Window::height;
		float z = 1.0f;
		Vec3 ray_nds = Vec3(x, y, z);
		Vec4 ray_clip = Vec4(ray_nds.x, ray_nds.y, -1.0, 1.0);

		//New Frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		if (Input::isKey(GLFW_KEY_LEFT_CONTROL) && Input::isKeyDown(GLFW_KEY_B)) ActiveGUI = !ActiveGUI;

		if (ActiveGUI) {
			{
				ImGui::Begin("Scene Options###SceneOptions");
				if (scene.pause) {
					if (ImGui::Button("Play")) {
						stepMode = false;
						scene.pause = false;
					}
				}
				else {
					if (ImGui::Button("Pause")) {
						stepMode = false;
						scene.pause = true;
					}
				}
				ImGui::SameLine();
				if (ImGui::Button("Reload")) {
					if (!illusioneditor::project::config::currentScenePath.empty()) {
						illusioneditor::views::GameProject::LoadScene(illusioneditor::project::config::projectPath + "/" + illusioneditor::project::config::currentScenePath);
						stepMode = false;
					}
				}
				ImGui::SameLine();
				if (ImGui::Button("Next Step")) {
					stepMode = true;
					scene.pause = false;
				}

				ImGui::SliderFloat("ScaleTime", &Time::timeScale, 0, 2);

				ImGui::End();
			}

			views::EditorMenuBar::SetScene(scene);
			views::EditorMenuBar::Show();

			views::FileExplorer::Show();

			views::GameHiearchy::SetScene(scene);
			views::GameHiearchy::Show();

			views::GameInspector::SetScene(scene);
			views::GameInspector::SetSelected(views::GameHiearchy::selected);
			views::GameInspector::Show();

			views::GameProject::SetScene(scene);
			views::GameProject::Show();

			views::GameStats::Show();

			views::MaterialEditor::Show();
		}

		//RENDER
		//--------
		ecs::core::Transform& transform = *scene.GetComponent<ecs::core::Transform>();
		ecs::core::Camera& camera = *scene.GetComponent<ecs::core::Camera>();
		MeshInstance& meshInstance = *scene.GetComponent<MeshInstance>();

		ImGui::Render();
		int display_w, display_h;
		glfwGetFramebufferSize(Window::glfwWindow, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		
		if (camera.size() > 0) {
			Vec4 background = camera.background[0];
			glClearColor(background.x, background.y, background.z, background.w);
		} else glClearColor(0.25f, 0.25f, 0.5f, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//UPDATE
		//--------
		if(views::GameStats::StartChronoData("Update Loop", "Game")) {
			scene.Update();
		}
		views::GameStats::EndChronoData("Update Loop", "Game");

		//COMPUTE MODELS POSITION
		//--------
		if (views::GameStats::StartChronoData("Compute Models", "Game")) {
			for (u32 i = 0; i < transform.ToEntity.size(); i++) {
				transform.ComputeModel((ecs::component_id)i);
			}

		}
		views::GameStats::EndChronoData("Compute Models", "Game");

		//PHYSICS
		//--------
		if (views::GameStats::StartChronoData("Physics", "Game")) {
			physicsTime += Time::unscaledDeltaTime;
			while (physicsTime >= Time::unscaledFixedDeltaTime) {
				physicsTime -= Time::unscaledFixedDeltaTime;

				//FIXED UPDATE
				//--------
				if (views::GameStats::StartChronoData("FixedUpdate Loop", "Game")) {
					scene.FixedUpdate();
				}
				views::GameStats::EndChronoData("FixedUpdate Loop", "Game");
				
				if (!scene.pause) ComputePhysics(scene);
				if (stepMode) scene.pause = true;
			}
		}
		views::GameStats::EndChronoData("Physics", "Game");

		//CAMERA MOVEMENT
		//--------
		if (camera.size() > 0)  {
			camera.UpdateVectors(camera.ToEntity[0]);
			if (Input::isMouse(1)) {
				camera.UpdateRotation(camera.ToEntity[0], Input::getMouseDelta().x, -Input::getMouseDelta().y);
			}
			else if (Input::isMouse(2)) {
				glfwSetCursor(Window::glfwWindow, glfwCreateStandardCursor(GLFW_HAND_CURSOR));
				transform.position[camera.ToEntity[0]] += camera.right[0] * -Input::getMouseDelta().x * Time::unscaledDeltaTime
					+ camera.up[0] * Input::getMouseDelta().y * Time::unscaledDeltaTime;
			}
			if (!ImGui::IsAnyItemHovered() && !ImGui::IsAnyWindowHovered())
				transform.position[camera.ToEntity[0]] += Input::getMouseWheelDelta() * camera.front[0] * camera.movementSpeed[0] * Time::unscaledDeltaTime;
		}

		//LATE UPDATE
		//--------
		if (views::GameStats::StartChronoData("LateUpdate Loop", "Game")) {
			scene.LateUpdate();
		}
		views::GameStats::EndChronoData("LateUpdate Loop", "Game");

		//RENDERING
		//--------
		if (views::GameStats::StartChronoData("Rendering", "Game")) {
			scene.renderer->Render();
		}
		views::GameStats::EndChronoData("Rendering", "Game");

		//EDITOR SELECTION
		//--------
		if (camera.size() > 0) {
			Vec4 ray_eye = glm::inverse(scene.renderer->projection) * ray_clip;
			ray_eye = Vec4(ray_eye.x, ray_eye.y, -1.0, 0.0);
			Vec3 ray_wor = (glm::inverse(scene.renderer->view) * ray_eye);
			ray_wor = glm::normalize(ray_wor);
			illusioneditor::scene::editor::EditorTests(scene, ray_wor);
			for (u32 i = 0; i < meshInstance.size(); i++) {
				illusioneditor::scene::editor::ClickedNearTest(scene, meshInstance.getId(i), ray_wor);
			}
			illusioneditor::scene::editor::DrawArrowTranslate(scene, scene.renderer->projection, scene.renderer->view);
		}

		//DRAW IMGUI
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		//SWAP BUFFER
		glfwSwapBuffers(Window::glfwWindow);

		if (Input::isKeyDown(GLFW_KEY_F11)) {
			Window::SetFullScreen(!Window::fullscreen);
		}
		views::GameStats::EndChronoData("Game");
	}

	//Shutdown
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();

	ImGui::DestroyContext();

	Window::Destroy();
}
