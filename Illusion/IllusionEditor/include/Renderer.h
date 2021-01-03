#pragma once

#include "Shader.h"
#include "Window.h"
#include "utilities/Utilities.h"
#include "ecs/id.h"
#include "ecs/CoreComponents/Transform.h"
#include "ecs/CoreComponents/Camera.h"
#include "ecs/Scene.h"

namespace illusion {

	struct Vertex {
		Vec3 position;
		Vec3 normal;
		Vec2 uv;
	};
	struct Mesh {
		size_t hash;
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		GLuint VAO, VBO, EBO;
		bool isSetup;
		Mesh() {
			vertices = std::vector<Vertex>();
			indices = std::vector<unsigned int>();
			VAO = 0;
			VBO = 0;
			EBO = 0;
			isSetup = false;
			hash = 0;
		}
		void Setup() {
			if (isSetup) return;
			INFO("Setup Mesh");
			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &VBO);
			glGenBuffers(1, &EBO);
			// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
			glBindVertexArray(VAO);

			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &(vertices[0]), GL_STATIC_DRAW);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &(indices[0]), GL_STATIC_DRAW);

			u32 offset = 0;
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offset);
			offset += sizeof(Vec3);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offset);
			offset += sizeof(Vec3);
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offset);
			offset += sizeof(Vec2);
			// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			// remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
			//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

			// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
			// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
			glBindVertexArray(0);
			isSetup = true;
			INFO("End Setup Mesh");
		}
		void Bind() {
			glBindVertexArray(VAO);
		}
	};
	struct Material {

	};
	struct MeshInstance : public ecs::Component {
		// Declare component name
		COMPONENT_NAME("Mesh Instance");
		COMPONENT_REGISTER(MeshInstance);
		// Declare constructor
		MeshInstance(ecs::Scene* scene) : Component(scene) {}

		// Declare datas
		COMPONENT_DATA(size_t, meshId);
		COMPONENT_DATA(size_t, materialId);

		// On Data added
		virtual void AddDatas(ecs::entity_id id) override {
			AddData(meshId, size_t(0));
			AddData(materialId, size_t(0));
		}

		// On Data removed
		virtual void RemoveDatas(ecs::component_id index, ecs::entity_id id) override {
			RemoveData(index, meshId, materialId);
		}
	};
	struct Renderer
	{
		ecs::Scene& scene;
		ecs::core::Camera& camera;
		ecs::core::Transform& transform;
		MeshInstance& meshInstance;

		util::Array<Mesh> meshes;

		util::Array<Shader> shaders;
		util::Array<util::Array<util::Array<ecs::entity_id>>> instancesByMeshByShader;
		util::Array<Material> materials;
		size_t instanceRenderingThreshold;

		Renderer(ecs::Scene &_scene) : 
			scene(_scene) , 
			camera(*(scene.GetComponent<ecs::core::Camera>())), 
			transform(*(scene.GetComponent<ecs::core::Transform>())), 
			meshInstance(*(scene.GetComponent<MeshInstance>())),
			instanceRenderingThreshold(10)
		{			
		}

		size_t AddMesh(Mesh mesh) {
			meshes.push_back(mesh);
			mesh.Setup();
			return meshes.size() - 1;
		}
		bool ContainsMesh(size_t hash) {
			std::vector<Mesh>::iterator it =std::find_if(meshes.begin(), meshes.end(), [&](const auto& val) { return val.hash == hash; });
			return true;
		}

		size_t AddShader(Shader shader) {
			shaders.push_back(shader);
			return shaders.size() - 1;
		}
		bool ContainsMesh(size_t hash) {
			std::vector<Shader>::iterator it = std::find_if(shaders.begin(), shaders.end(), [&](const auto& val) { return val.hash == hash; });
			return true;
		}
		void Render() {//@Todo register draw calls and num entities rendered per frame
			if (camera.size() < 1) {
				INTERNAL_ERR("No Camera, the scene can't be rendered");
				return;
			}
			//for each shader
			size_t numShaders = instancesByMeshByShader.size();
			for (size_t i = 0;i < numShaders;i++) {
				Shader& shader = shaders[i];
				shader.use();

				//@Todo change projection only if one of these values are changed
				float aspect = (float)Window::width / (float)Window::height;
				Mat4x4 projection = glm::perspective(camera.fov[0], aspect, camera.near[0], camera.far[0]);

				Mat4x4 view = glm::lookAt(transform.position[camera.ToEntity[0]], transform.position[camera.ToEntity[0]] + camera.front[0], camera.up[0]);


				//set view and projection matrices
				shader.setMat4("view", view);
				shader.setMat4("projection", projection);

				//for each Mesh using this shader
				size_t numMeshes = instancesByMeshByShader[i].size();
				for (size_t j = 0;j < numMeshes;j++) {
					Mesh& mesh = meshes[j];
					if (!mesh.isSetup) continue;
					mesh.Bind();

					//for each instance
					size_t numInstances = instancesByMeshByShader[i][j].size();
					for (size_t k = 0;k<instancesByMeshByShader[i][j].size();k++) {
						ecs::entity_id instance_id = instancesByMeshByShader[i][j][k];
						ecs::component_id idTransform = transform.getIndex(instance_id);
						transform.ComputeModel(idTransform);//@Todo Compute model en dehors du rendu pour toutes les entités ?
						Mat4x4 modelMatrix = transform.modelTransform[instance_id];
						// @Todo get Material

						// @Todo make instance rendering if instances >treshold
						shader.setMat4("model",modelMatrix);
						//Render
						glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
					}
				}
			}
		}
	};
}