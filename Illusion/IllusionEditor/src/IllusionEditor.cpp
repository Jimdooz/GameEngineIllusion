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

#include "core/Time.h"
#include "core/physics/collisions/Collisions.h"
#include "project/ProjectManager.h"

#include <iostream>
#include <vector>
#include <bitset>
#include <map>
#include <string>
//auto start = high_resolution_clock::now();
//auto stop = high_resolution_clock::now();
//std::cout << "DURATION : " << duration_cast<microseconds>(stop - start).count() / 1000000.0 << std::endl;

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

//TEMP RENDERING
#include "tmp/Shader.h"

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

struct PlanetComponent : public ecs::Component {
	// Declare component name
	COMPONENT_NAME("Planet");
	COMPONENT_REGISTER(PlanetComponent);

	// Declare constructor
	PlanetComponent(ecs::Scene* scene) : Component(scene) {
		// Display on inspector
		COMPONENT_PUBLIC(speed);
	}

	// Declare datas
	COMPONENT_DATA(f32, speed);
	COMPONENT_DATA(f32, evolution);

	// On Data added
	virtual void AddDatas(ecs::entity_id id) override {
		AddData(speed, f32(0));
		AddData(evolution, f32(0));
	}

	// On Data removed
	virtual void RemoveDatas(ecs::component_id index, ecs::entity_id id) {
		RemoveData(speed, index);
		RemoveData(evolution, index);
	}
};

struct PlanetSystem : public ecs::System {
	SYSTEM_NAME("Planet");
	SYSTEM_REGISTER(PlanetSystem);

	ecs::core::Transform* transform;
	PlanetComponent* planet;

	/* la fonction Update */
	SYSTEM_UPDATE_LOOP(
		rotation() = glm::rotate(rotation(), Vec3(0, evolution(), 0));
		evolution() += speed() * Time::deltaTime;
	)

	/* Definition des variables utiles */
	SYSTEM_USE_DATA(position, transform, position, Vec3);
	SYSTEM_USE_DATA(rotation, transform, rotation, Quaternion);
	SYSTEM_USE_DATA(speed, planet, speed, f32);
	SYSTEM_USE_DATA(evolution, planet, evolution, f32);

	/* Initialisation relative a la scene parente */
	virtual void Initialize(ecs::Scene& scene) override {
		transform = scene.GetComponent<ecs::core::Transform>();
		planet = scene.GetComponent<PlanetComponent>();
		SetDependencies(transform, planet);
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
	illusion::Time::Init();

	// Init ECS
	//----------
	illusion::ecs::Component::AppendCoreComponents();
	illusion::ecs::Component::AppendComponents<RigidBodyComponent>();
	illusion::ecs::Component::AppendComponents<PlanetComponent>();
	illusion::ecs::System::AppendSystems<TwerkSystem>();
	illusion::ecs::System::AppendSystems<PlanetSystem>();

	// Init Scene
	//----------
	ecs::Scene scene;
	scene.UseComponent<RigidBodyComponent>();
	scene.UseComponent<PlanetComponent>();
	scene.UseSystem<TwerkSystem>();
	scene.UseSystem<PlanetSystem>();

	/*for (u32 i = 0; i < 100; i++) {
		ecs::entity_id entity = scene.CreateEntity();
		if (i > 0) {
			scene.GetComponent<ecs::core::Transform>()->SetParent(ecs::entity_id{ i }, ecs::entity_id{ i - 1 });
			scene.GetComponent<ecs::core::Transform>()->position[i] = Vec3(1, 0, 0);
		}
		scene.EntityAddComponent<PlanetComponent>(ecs::entity_id{ i });
		scene.GetComponent<PlanetComponent>()->speed[i] = 1.0f;
		scene.GetComponent<ecs::core::Transform>()->scale[i] = Vec3(0.5, 0.5, 0.5);
	}*/

	std::vector<float> fpsMesure;

	std::filesystem::directory_entry currentPath("D:\\GitHub\\GameEngineIllusion\\GameProjects\\Optimulus");
	std::string pathName = currentPath.path().string();
	std::string projectName = "";

	// Temp rendering
	//---------
	float vertices[] = {
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};

	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	Shader ourShader("D:\\GitHub\\GameEngineIllusion\\GameProjects\\Optimulus\\Assets\\Shader\\vertexShader.glsl",
					"D:\\GitHub\\GameEngineIllusion\\GameProjects\\Optimulus\\Assets\\Shader\\fragmentShader.glsl");
	ourShader.use();
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// Main Loop
	//---------
	while (!Window::shouldClose) {
		views::GameStats::StartChronoData("Game");

		//Update time
		illusion::Time::UpdateTime(glfwGetTime());

		Input::Update();

		double xpos, ypos;
		glfwGetCursorPos(Window::glfwWindow, &xpos, &ypos);

		float x = (2.0f * xpos) / Window::width - 1.0f;
		float y = 1.0f - (2.0f * ypos) / Window::height;
		float z = 1.0f;
		Vec3 ray_nds = Vec3(x, y, z);
		Vec4 ray_clip = Vec4(ray_nds.x, ray_nds.y, -1.0, 1.0);

		if (Input::isKey(GLFW_KEY_LEFT_CONTROL) && Input::isKeyDown(GLFW_KEY_S)) {
			std::ofstream myfile;
			myfile.open(illusioneditor::project::config::projectPath + "/" + illusioneditor::project::config::currentScenePath);
			myfile << resources::assets::ExportScene(scene).dump(4);
			myfile.close();
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
						myfile.open(illusioneditor::project::config::projectPath + "/" + illusioneditor::project::config::currentScenePath);
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
			ImGui::Begin("File Explorer");

			ImGui::Button("..");
			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
				currentPath = std::filesystem::directory_entry(currentPath.path().parent_path());
				pathName = currentPath.path().string();
			}
			ImGui::SameLine();
			int n = pathName.length() + 64;
			char *buf1 = new char[n];
			strcpy(buf1, pathName.c_str());
			ImGui::InputText("###goto", buf1, n);

			pathName = std::string(buf1);
			delete buf1;

			ImGui::SameLine();
			if (ImGui::Button("Go")) {
				currentPath = std::filesystem::directory_entry(pathName);
			}

			ImGui::Spacing();
			ImGui::Spacing();
			ImGui::Spacing();
			ImGui::BeginChild("##ScrollingRegion", ImVec2(0, ImGui::GetWindowHeight() * 1.0f - ImGui::GetFontSize() * 10.0f));
			ImGui::Columns(2);

			try {
				for (auto& p : fs::directory_iterator(currentPath)) {
					std::string path = p.path().filename().string();
					if (p.is_directory() && path.find('.') == std::string::npos) {
						ImGui::Button(path.c_str());
						if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
							std::cout << p.path() << std::endl;
							currentPath = std::filesystem::directory_entry(p.path());
							pathName = currentPath.path().string();
						}
					}
					else {
						ImGui::TextColored((ImVec4)ImColor::ImColor(100, 100, 100), path.c_str());
					}
					ImGui::NextColumn();
				}
			}
			catch (const std::exception&) {
				try {
					currentPath = std::filesystem::directory_entry(currentPath.path().parent_path());
				}
				catch (const std::exception&) {
					currentPath = std::filesystem::directory_entry("D:\\");
				}
				pathName = currentPath.path().string();
			}
			
			ImGui::EndChild();

			ImGui::BeginChild("##BottomRegion");

			n = projectName.length() + 64;
			char* buf2 = new char[n];
			strcpy(buf2, projectName.c_str());
			ImGui::InputText("Project Name###createProject", buf2, n);

			projectName = std::string(buf2);
			delete buf2;

			if (ImGui::Button("Create Project")) {
				illusioneditor::project::tools::CreateProject(pathName, projectName);
			}
			ImGui::SameLine();
			if (ImGui::Button("Load Project")) {
				std::string error;
				bool success = illusioneditor::project::tools::LoadProject(pathName, &error);

				/*json jsonLoaded;
				{
					std::ifstream t(illusioneditor::project::config::projectPath + "/" + illusioneditor::project::config::currentScenePath);
					std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
					jsonLoaded = json::parse(str);
				}
				scene.Reset();
				resources::assets::LoadScene(scene, jsonLoaded);*/

				INTERNAL_INFO("SUCCESS ", success, " -> ", error);
			}
			ImGui::EndChild();

			ImGui::End();
		}

		{
			ImGui::Begin("Scene Options###SceneOptions");
			if (scene.pause) {
				if (ImGui::Button("Play")) scene.pause = false;
			} else {
				if (ImGui::Button("Pause")) scene.pause = true;
			}
			ImGui::SameLine();
			if (ImGui::Button("Reload")) {
				illusioneditor::views::GameProject::LoadScene(illusioneditor::project::config::projectPath + "/" + illusioneditor::project::config::currentScenePath);
				scene.pause = true;
			}
			ImGui::End();
		}

		views::GameHiearchy::SetScene(scene);
		views::GameHiearchy::Show();

		views::GameInspector::SetScene(scene);
		views::GameInspector::SetSelected(views::GameHiearchy::selected);
		views::GameInspector::Show();

		views::GameProject::SetScene(scene);
		views::GameProject::Show();

		views::GameStats::Show();

		//RENDER
		//--------
		ImGui::Render();
		int display_w, display_h;
		glfwGetFramebufferSize(Window::glfwWindow, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(0.25f, 0.25f, 0.5f, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);

		ecs::core::Transform& transform = *scene.GetComponent<ecs::core::Transform>();

		//UPDATE
		if(views::GameStats::StartChronoData("Update Loop", "Game")) {
			scene.Update();
		}
		views::GameStats::EndChronoData("Update Loop", "Game");

		//COMPUTE MODELS POSITION
		if(views::GameStats::StartChronoData("Compute Models", "Game")) {
			for (u32 i = 0; i < transform.ToEntity.size(); i++) {
				// calculate the model matrix for each object and pass it to shader before drawing
				transform.ComputeModel(ecs::component_id{ i });
			}

		}
		views::GameStats::EndChronoData("Compute Models", "Game");

		//DRAW RENDERING
		if(views::GameStats::StartChronoData("Rendering", "Game")) {
			// activate shader
			ourShader.use();

			glm::mat4 view = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
			glm::mat4 projection = glm::mat4(1.0f);

			// create transformations
			projection = glm::perspective(glm::radians(45.0f), (float)Window::width / (float)Window::height, 0.1f, 100.0f);
			view = glm::translate(view, Vec3(0.0f, 0.0f, -3.0f));
			// pass transformation matrices to the shader
			ourShader.setMat4("projection", projection); // note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
			ourShader.setMat4("view", view);
			ourShader.setMat4("model", glm::mat4(1.0f));

			Vec4 ray_eye = glm::inverse(projection) * ray_clip;
			ray_eye = Vec4(ray_eye.x, ray_eye.y, -1.0, 0.0);
			Vec3 ray_wor = (glm::inverse(view) * ray_eye);
			ray_wor = glm::normalize(ray_wor);

			// render boxes
			glBindVertexArray(VAO);

			for (u32 i = 0; i < transform.ToEntity.size(); i++) {
				// calculate the model matrix for each object and pass it to shader before drawing
				ourShader.setBool("collision", false);
				ourShader.setMat4("model", transform.computedModel[i]);

				Vec4 worldPosition = transform.computedModel[i] * Vec4(0, 0, 0, 1);

				core::physics::primitives::AABB aabb(transform.computedModel[i] * Vec4(0,0,0,1), Vec3(0.5 * transform.scale[i].x,0.5 * transform.scale[i].y,0.5 * transform.scale[i].z));
				core::physics::primitives::Ray ray(Vec3(0.0f, 0.0f, 3.0f), ray_wor);

				if (core::physics::collisions::Raycast(aabb, ray) >= 0) ourShader.setBool("collision", true);

				glDrawArrays(GL_TRIANGLES, 0, 36);
			}

		}
		views::GameStats::EndChronoData("Rendering", "Game");

		//DRAW IMGUI
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		//SWAP BUFFER
		glfwSwapBuffers(Window::glfwWindow);

		views::GameStats::EndChronoData("Game");
	}

	//Shutdown
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();

	ImGui::DestroyContext();

	Window::Destroy();
}
