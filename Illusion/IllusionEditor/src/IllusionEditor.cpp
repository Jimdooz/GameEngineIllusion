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

#include <iostream>
#include <vector>
#include <bitset>
#include <map>
#include <string>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

//TEMP RENDERING
#include "Shader.h"
#include "Renderer.h"
#include "Importer.h"

using namespace std::chrono;
using namespace illusion;
using namespace illusioneditor;
using namespace illusion::core::physics;

struct JumpBigCube : public ecs::Component {
	COMPONENT_NAME("Jump My Big Cube");
	COMPONENT_REGISTER(JumpBigCube);

	JumpBigCube(ecs::Scene* scene) : Component(scene) {
		COMPONENT_PUBLIC(direction);
		COMPONENT_PUBLIC(powerJump);
	}

	COMPONENT_DATA(Vec3, direction);
	COMPONENT_DATA(f32, powerJump);
	COMPONENT_DATA(boolean, activeJump);
	COMPONENT_DATA(f32, timerJump);

	// On Data added
	virtual void AddDatas(ecs::entity_id id) override {
		AddData(direction, Vec3(0,1,1));
		AddData(powerJump, 1.f);
		AddData<boolean>(activeJump, false);
		AddData(timerJump, 0.0f);
	}

	// On Data removed
	virtual void RemoveDatas(ecs::component_id index, ecs::entity_id id) {
		RemoveData(index, direction, powerJump, activeJump, timerJump);
	}
};

struct JumpBigCubeSystem : public ecs::System {
	SYSTEM_NAME("JumpBigCube");
	SYSTEM_REGISTER(JumpBigCubeSystem);

	ecs::core::Transform* transform;
	JumpBigCube* jumpBigCube;
	core::physics::RigidBody* rigidbody;

	/* la fonction Update */
	SYSTEM_UPDATE_LOOP(
		if (collisions().size() > 0 && timerJump() > 0.1f) activeJump() = true;
		timerJump() += Time::deltaTime;

		if (Input::isKeyDown(GLFW_KEY_SPACE) && activeJump()) {
			if (collisions().size() > 0) {
				velocity() = glm::normalize(direction()) * powerJump();
			}
			else velocity() = glm::normalize(direction()) * powerJump();
			activeJump() = false;
			timerJump() = 0.0f;
		}

		if (Input::isKey(GLFW_KEY_LEFT)) velocity() = Vec3(-5, velocity().y, 0);
		if (Input::isKey(GLFW_KEY_RIGHT)) velocity() = Vec3(5, velocity().y, 0);

		if(!Input::isKey(GLFW_KEY_LEFT) && !Input::isKey(GLFW_KEY_RIGHT)) velocity() = Vec3(0, velocity().y, 0);
		position().z = 0;
	)

	/* Definition des variables utiles */
	SYSTEM_USE_DATA(direction, jumpBigCube, direction, Vec3);
	SYSTEM_USE_DATA(powerJump, jumpBigCube, powerJump, f32);
	SYSTEM_USE_DATA(activeJump, jumpBigCube, activeJump, boolean);
	SYSTEM_USE_DATA(timerJump, jumpBigCube, timerJump, f32);
	SYSTEM_USE_DATA(velocity, rigidbody, velocity, Vec3);
	SYSTEM_USE_DATA(collisions, rigidbody, collisions, util::Array<core::physics::CollisionRigidBody>);
	SYSTEM_USE_DATA(rotation, transform, rotation, Quaternion);
	SYSTEM_USE_DATA(position, transform, position, Vec3);

	/* Initialisation relative a la scene parente */
	virtual void Initialize(ecs::Scene& scene) override {
		transform = scene.GetComponent<ecs::core::Transform>();
		jumpBigCube = scene.GetComponent<JumpBigCube>();
		rigidbody = scene.GetComponent<core::physics::RigidBody>();
		SetDependencies(transform, jumpBigCube, rigidbody);
	}
};

struct CubeRenderer : public ecs::Component {
	// Declare component name
	COMPONENT_NAME("Cube Renderer");
	COMPONENT_REGISTER(CubeRenderer);
	// Declare constructor
	CubeRenderer(ecs::Scene* scene) : Component(scene) {}
};



struct RigidBodyComponent : public ecs::Component {
	// Declare component name
	COMPONENT_NAME("ParticleRigidbody");
	COMPONENT_REGISTER(RigidBodyComponent);

	// Declare constructor
	RigidBodyComponent(ecs::Scene* scene) : Component(scene) {
		// Display on inspector
	}

	// Declare datas
	COMPONENT_DATA(Vec3, oldPosition);

	COMPONENT_DATA(Vec3, forces);
	COMPONENT_DATA(f32, mass);
	COMPONENT_DATA(f32, bounce);

	COMPONENT_DATA(f32, friction);
	COMPONENT_DATA(Vec3, gravity);

	void ApplyForces(ecs::entity_id id) {
		ecs::component_id index = getIndex(id);

		forces[index] = gravity[index];
	}

	void Update(ecs::entity_id id) {
		//Get Index
		ecs::component_id index = getIndex(id);
		//Get Position
		ecs::core::Transform* transform = scene->GetComponent<ecs::core::Transform>();
		Vec3& position = transform->position[transform->getIndex(id)];

		//Compute Physics
		Vec3 velocity = position - oldPosition[index];
		oldPosition[index] = position;

		f32 deltaSquare = Time::fixedDeltaTime * Time::fixedDeltaTime;
		position = position + (velocity * friction[index] + forces[index] * deltaSquare);
	}

	void SolveConstraints(ecs::entity_id id, const util::Array<primitives::OBB>& constraints) {
		//Get Index
		ecs::component_id index = getIndex(id);
		//Get Position
		ecs::core::Transform* transform = scene->GetComponent<ecs::core::Transform>();
		Vec3& position = transform->position[transform->getIndex(id)];

		int size = constraints.size();
		for (int i = 0; i < size; ++i) {
			primitives::Line traveled(oldPosition[index], position);
			if (collisions::Linetest(constraints[i], traveled)) {
				Vec3 velocity = position - oldPosition[index];
				Vec3 direction = glm::normalize(velocity);
				primitives::Ray ray(oldPosition[index], direction);
				collisions::RaycastResult result;
				if (Raycast(constraints[i], ray, &result)) {
					position = result.point + result.normal * 0.003f;
					Vec3 vn = result.normal * glm::dot(result.normal, velocity);
					Vec3 vt = velocity - vn;
					oldPosition[index] = position - (vt - vn * bounce[index]);
					break;
				}
			}
		}
	}

	// On Data added
	virtual void AddDatas(ecs::entity_id id) override {

		//Get Position
		ecs::core::Transform* transform = scene->GetComponent<ecs::core::Transform>();
		Vec3& position = transform->position[transform->getIndex(id)];

		AddData(oldPosition, position);
		AddData(forces, Vec3(0, 0, 0));

		AddData(mass, 1.0f);
		AddData(bounce, 0.7f);

		AddData(friction, 0.95f);
		AddData(gravity, Vec3(0, -9.82f, 0));
	}

	// On Data removed
	virtual void RemoveDatas(ecs::component_id index, ecs::entity_id id) {
		RemoveData(index, oldPosition, forces, mass, bounce, friction, gravity);
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
		RemoveData(index, speed, evolution);
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

int main(int argc, char* argv[]) {
	// Create Window
	//--------
	Window::Create(1280,720,"MyGame");

	// Test [Romain Saclier]
	//--------

	// Setup IMGUI
	//--------
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui_ImplGlfw_InitForOpenGL(Window::glfwWindow, true);
	ImGui_ImplOpenGL3_Init((char*)glGetString(GL_NUM_SHADING_LANGUAGE_VERSIONS));
	views::theme::InitTheme();

	// Init Engine
	//----------
	illusion::resources::JsonConvertor::Initialize();
	illusion::Time::Init();

	// Init Editor
	//----------
	illusioneditor::scene::editor::Initialize();

	// Init ECS
	//----------
	illusion::ecs::Component::AppendCoreComponents();
	illusion::ecs::Component::AppendComponents<RigidBodyComponent>();
	illusion::ecs::Component::AppendComponents<PlanetComponent>();
	illusion::ecs::Component::AppendComponents<JumpBigCube>();

	//Systems
	illusion::ecs::System::AppendSystems<PlanetSystem>();
	illusion::ecs::System::AppendSystems<JumpBigCubeSystem>();

	// Init Scene
	//----------
	ecs::Scene scene;

	scene.UseComponent<RigidBodyComponent>();
	scene.UseComponent<PlanetComponent>();
	scene.UseSystem<PlanetSystem>();

	//Create Camera
	ecs::entity_id entity = scene.CreateEntity();
	scene.GetComponent<ecs::core::Transform>()->name[entity] = "Camera";
	scene.EntityAddComponent<ecs::core::Camera>(entity);

	Renderer renderer(scene);

	illusion::import3DModel("..\\..\\GameProjects\\Optimulus\\Assets\\Meshes\\basicCharacter_anim.fbx",&renderer,&scene);

	std::vector<float> fpsMesure;

	std::filesystem::directory_entry currentPath("..\\GameProjects\\Optimulus");
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

	Shader ourShader("..\\..\\..\\GameEngineIllusion\\GameProjects\\Optimulus\\Assets\\Shader\\vertexShader.glsl",
					"..\\..\\..\\GameEngineIllusion\\GameProjects\\Optimulus\\Assets\\Shader\\fragmentShader.glsl");
	ourShader.use();
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glEnable(GL_DEPTH_TEST);

	f32 physicsTime = 0.0f;

	bool stepMode = false;

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

		{
			ImGui::Begin("Scene Options###SceneOptions");
			if (scene.pause) {
				if (ImGui::Button("Play")) {
					stepMode = false;
					scene.pause = false;
				}
			} else {
				if (ImGui::Button("Pause")) {
					stepMode = false;
					scene.pause = true;
				}
			}
			ImGui::SameLine();
			if (ImGui::Button("Reload")) {
				illusioneditor::views::GameProject::LoadScene(illusioneditor::project::config::projectPath + "/" + illusioneditor::project::config::currentScenePath);
				stepMode = false;
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

		//RENDER
		//--------
		ImGui::Render();
		int display_w, display_h;
		glfwGetFramebufferSize(Window::glfwWindow, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(0.25f, 0.25f, 0.5f, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ecs::core::Transform& transform = *scene.GetComponent<ecs::core::Transform>();
		RigidBodyComponent& rigibodies = *scene.GetComponent<RigidBodyComponent>();
		ecs::core::Camera& camera = *scene.GetComponent<ecs::core::Camera>();
		CubeRenderer& renderer = *scene.GetComponent<CubeRenderer>();
		MeshInstance& meshInstance = *scene.GetComponent<MeshInstance>();

		//UPDATE
		if(views::GameStats::StartChronoData("Update Loop", "Game")) {
			scene.Update();
		}
		views::GameStats::EndChronoData("Update Loop", "Game");

		//COMPUTE MODELS POSITION
		if (views::GameStats::StartChronoData("Compute Models", "Game")) {
			for (u32 i = 0; i < transform.ToEntity.size(); i++) {
				// calculate the model matrix for each object and pass it to shader before drawing
				transform.ComputeModel((ecs::component_id)i);
			}

		}
		views::GameStats::EndChronoData("Compute Models", "Game");

		ourShader.setMat4("model", glm::mat4(1.0f));

		//PHYSICS
		if (views::GameStats::StartChronoData("Physics", "Game")) {
			physicsTime += Time::unscaledDeltaTime;
			while (physicsTime >= Time::unscaledFixedDeltaTime) {
				physicsTime -= Time::unscaledFixedDeltaTime;

				if (scene.pause) continue;
				
				ComputePhysics(scene);
				if (stepMode) scene.pause = true;
			}
		}
		views::GameStats::EndChronoData("Physics", "Game");

		//CAMERA MOVEMENT
		{
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

		//RENDERING
		if (views::GameStats::StartChronoData("Rendering", "Game")) {
			//renderer.
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
