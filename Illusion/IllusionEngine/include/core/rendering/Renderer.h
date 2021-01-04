#pragma once

#include "Shader.h"
#include "Window.h"
#include "utilities/Utilities.h"
#include "ecs/id.h"
#include "ecs/CoreComponents/Transform.h"
#include "ecs/CoreComponents/Camera.h"

namespace illusion::ecs {
	struct Scene;
}

namespace illusion {

	struct Vertex {
		Vec3 position;
		Vec3 normal;
		Vec2 uv;
	};
	struct Mesh {
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

			//@Todo format and buffer separation (openGL 4.5)
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
		size_t shaderId;
	};
	struct MeshInstance : public ecs::Component {
		// Declare component name
		COMPONENT_NAME("Mesh Instance");
		COMPONENT_REGISTER(MeshInstance);
		// Declare constructor
		MeshInstance(ecs::Scene* scene) : Component(scene) {
			COMPONENT_PROTECTED(meshId);
			COMPONENT_PROTECTED(materialId);
		}

		// Declare datas
		COMPONENT_DATA(size_t, meshId);
		COMPONENT_DATA(size_t, materialId);
		COMPONENT_DATA(boolean, initialized);

		virtual void OnEntityDuplicate(ecs::entity_id id) override;

		// On Data added
		virtual void AddDatas(ecs::entity_id id) override {
			AddData(meshId, size_t(0));
			AddData(materialId, size_t(0));
			AddData<boolean>(initialized, false);
		}

		// On Data removed
		virtual void RemoveDatas(ecs::component_id index, ecs::entity_id id) override {
			RemoveData(index, meshId, materialId);
		}
	};
	struct Renderer
	{
		ecs::Scene* scene;
		ecs::core::Camera* camera;
		ecs::core::Transform* transform;
		MeshInstance* meshInstance;

		util::UnorderedMap<size_t,Shader> shaders;
		util::UnorderedMap<size_t, Mesh> meshes;
		util::UnorderedMap<size_t, Material> materials;

		util::UnorderedMap<size_t, util::UnorderedMap<size_t, util::Array<ecs::entity_id>>> instancesByMeshByShader;
				
		size_t instanceRenderingThreshold;

		Renderer(ecs::Scene* _scene);

		/**
		 * Permet d'ajouter un mesh par son id
		 */
		inline void AddMesh(Mesh mesh, size_t idMesh) {
			meshes[idMesh] = mesh;
		}

		/**
		 * Permet de savoir si un mesh est déjà existant par son Id
		 */
		inline bool ContainsMesh(size_t idMesh) {
			return meshes.find(idMesh) != meshes.end();
		}

		/**
		 * Permet d'ajouter un shader
		 */
		inline void AddShader(Shader shader, size_t idShader) {
			shaders[idShader] = shader;
			//On créé automatiquement un espace pour la suite
			instancesByMeshByShader[idShader] = util::UnorderedMap<size_t, util::Array<ecs::entity_id>>() ;
		}

		/**
		 * Permet de savoir si un shader existe déjà par son id
		 */
		inline bool ContainsShader(size_t idShader) {
			return shaders.find(idShader) != shaders.end();
		}

		/**
		 * Permet d'ajouter un material
		 */
		inline void AddMaterial(Material material, size_t idMaterial) {
			materials[idMaterial]=material;
		}

		/**
		 * Permet de savoir si un material existe déjà par son id
		 */
		inline bool ContainsMaterial(size_t idMaterial) {
			return materials.find(idMaterial) != materials.end();
		}

		/*********************************************/
		/* Ajout & Suppression par [Shader / Mesh] */
		/*********************************************/

		inline bool MeshExistInShader(size_t idShader, size_t idMesh){
			return instancesByMeshByShader[idShader].find(idMesh) != instancesByMeshByShader[idShader].end();
		}

		void AddMeshShader(size_t idShader, size_t idMesh, ecs::entity_id entity);

		void RemoveMeshShader(size_t idShader, size_t idMesh, ecs::entity_id entity);

		/**
		 * Permet de rendre la scène
		 */
		void Render() {//@Todo register draw calls and num entities rendered per frame
			if (camera->size() < 1) {
				INTERNAL_ERR("No Camera, the scene can't be rendered");
				return;
			}
			//for each shader
			for (auto const& [shaderKey, meshMap] : instancesByMeshByShader)
			{
				Shader& shader = shaders[shaderKey];
				shader.use();

				//@Todo change projection only if one of these values are changed
				float aspect = (float)Window::width / (float)Window::height;
				Mat4x4 projection = glm::perspective(camera->fov[0], aspect, camera->near[0], camera->far[0]);

				Mat4x4 view = glm::lookAt(transform->position[camera->ToEntity[0]], transform->position[camera->ToEntity[0]] + camera->front[0], camera->up[0]);


				//set view and projection matrices
				shader.setMat4("view", view);
				shader.setMat4("projection", projection);

				//for each Mesh using this shader
				for (auto const& [meshKey, entitiesArray] : meshMap)
				{
					Mesh& mesh = meshes[meshKey];
					if (!mesh.isSetup) continue;
					mesh.Bind();

					//for each instance
					size_t numInstances = entitiesArray.size();
					for (size_t i = 0;i< numInstances;i++) {
						ecs::entity_id instance_id = entitiesArray[i];
						ecs::component_id idTransform = transform->getIndex(instance_id);
						transform->ComputeModel(idTransform);//@Todo Compute model en dehors du rendu pour toutes les entités ?
						Mat4x4 modelMatrix = transform->modelTransform[instance_id];
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