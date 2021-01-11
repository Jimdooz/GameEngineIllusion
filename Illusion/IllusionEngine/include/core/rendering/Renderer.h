#pragma once

#include "Shader.h"
#include "Window.h"
#include "utilities/Utilities.h"
#include "ecs/id.h"
#include "ecs/CoreComponents/Transform.h"
#include "ecs/CoreComponents/Camera.h"

#include "core/rendering/CoreComponents/directionalLight.h"
#include "core/rendering/CoreComponents/PostProcess.h"
#include "core/rendering/CoreComponents/pointLight.h"
#include "resources/assets/Materials.h"

#include "core/animation/Bone.h"
#include "core/animation/Animation.h"
#include "core/animation/CoreComponents/Skeleton.h"

using Material = illusion::resources::assets::MaterialResource;
namespace animation = illusion::core::animation;

#define VERTEX_POSITION_LOCATION	0
#define VERTEX_NORMAL_LOCATION		1
#define VERTEX_UV_LOCATION			2
#define BONE_ID_LOCATION			3
#define BONE_WEIGHT_LOCATION		4

namespace illusion::ecs {
	struct Scene;
}

namespace illusion {

	struct FrameBuffer {
		u32 id;
		bool generated = false;
		bool depthStencilGenerated = false;
		bool textureGenerated = false;
		bool msTextureGenerated = false;

		u32 idDepthStencil;
		u32* idTexture; //Id Texture
		u32* idMSTexture; //Id Multi Sample Texture

		u32 sampleDepthStencil = 0;
		u32 nbTextures = 0;
		u32 nbMSTextures = 0;
		u32 sampleMSTextures = 0;

		~FrameBuffer() {
			Delete();
		}

		inline void Bind() {
			if (!generated) return;
			glBindFramebuffer(GL_FRAMEBUFFER, id);
		}

		inline void UnBind() {
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		inline void Clear() {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		}

		inline void BindTexture(u32 texture, u32 position = 0) {
			if (texture >= nbTextures) return;
			glActiveTexture(GL_TEXTURE0 + position);
			glBindTexture(GL_TEXTURE_2D, idTexture[texture]);
		}

		inline void BindMSTexture(u32 texture, u32 position = 0) {
			if (texture >= nbMSTextures) return;
			glActiveTexture(GL_TEXTURE0 + position);
			glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, idMSTexture[texture]);
		}

		void Delete(bool resetValues = true) {
			if (!generated) return;
			glDeleteFramebuffers(1, &id);
			if (depthStencilGenerated) glDeleteRenderbuffers(1, &idDepthStencil);
			if (textureGenerated) {
				glDeleteTextures(nbTextures, idTexture);
				delete idTexture;
			}
			if (msTextureGenerated) {
				glDeleteTextures(nbMSTextures, idMSTexture);
				delete idMSTexture;
			}
			if (resetValues) {
				depthStencilGenerated = false;
				textureGenerated = false;
				msTextureGenerated = false;
			}
			generated = false;
		}

		void Reload() {
			Delete(false);
			Reserve();
			if (depthStencilGenerated) {
				depthStencilGenerated = false;
				GenerateDepthStencil(sampleDepthStencil);
			}
			if (textureGenerated) {
				textureGenerated = false;
				GenerateTexture(nbTextures);
			}
			if (msTextureGenerated) {
				msTextureGenerated = false;
				GenerateMSTexture(nbMSTextures, sampleMSTextures);
			}
			Complete();
		}

		void Reserve(){
			if (generated) return;
			glGenFramebuffers(1, &id);
			glBindFramebuffer(GL_FRAMEBUFFER, id);
		}

		void GenerateDepthStencil(u32 sample = 1) {
			if (generated) return;
			INFO("GenerateDepthStencil");
			sampleDepthStencil = sample;
			glGenRenderbuffers(1, &idDepthStencil);
			glBindRenderbuffer(GL_RENDERBUFFER, idDepthStencil);
			glRenderbufferStorageMultisample(GL_RENDERBUFFER, sample, GL_DEPTH24_STENCIL8, Window::width, Window::height);
			glBindRenderbuffer(GL_RENDERBUFFER, 0);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, idDepthStencil);
			depthStencilGenerated = true;
		}

		void GenerateTexture(u32 quantity = 1) {
			if (generated || textureGenerated) return;
			nbTextures = quantity;
			u32* attachments = new u32[quantity];
			idTexture = new u32[quantity];
			glGenTextures(quantity, idTexture);

			for (unsigned int i = 0; i < quantity; i++) {
				glBindTexture(GL_TEXTURE_2D, idTexture[i]);
				glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA16F, Window::width, Window::height, 0, GL_RGBA, GL_FLOAT, NULL );
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glFramebufferTexture2D(
					GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, idTexture[i], 0
				);
				attachments[i] = GL_COLOR_ATTACHMENT0 + i;
			}
			glDrawBuffers(quantity, attachments);
			textureGenerated = true;
		}
		
		void GenerateMSTexture(u32 quantity = 1, u32 sample = 1){
			if (generated || msTextureGenerated) return;
			nbMSTextures = quantity;
			sampleMSTextures = sample;
			idMSTexture = new u32[quantity];
			u32* attachments = new u32[quantity];
			glGenTextures(quantity, idMSTexture);

			for (unsigned int i = 0; i < quantity; i++) {
				glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, idMSTexture[i]);
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, sample, GL_RGB, Window::width, Window::height, GL_TRUE);
				glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, i);
				glFramebufferTexture2D(
					GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D_MULTISAMPLE, idMSTexture[i], 0
				);
				attachments[i] = GL_COLOR_ATTACHMENT0 + i;
			}

			glDrawBuffers(quantity, attachments);
			msTextureGenerated = true;
		}
		
		bool Complete() {
			if (generated) return true;
			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
				ERR("ERROR::FRAMEBUFFER:: Framebuffer is not complete -> ID : ", id);
				return false;
			}
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			generated = true;
			return true;
		}
	};

	struct Vertex {
		Vec3 position;
		Vec3 normal;
		Vec2 uv;
	};

	struct Mesh {
		std::vector<Vertex> vertices;
		std::vector<animation::VertexBoneData> verticesBoneData;
		std::vector<unsigned int> indices;
		GLuint VAO, VBO, EBO, bonesVBO;
		bool isSetupOnGPU;
		std::string name;
		std::string group = "";

		Mesh(std::string name = "unamed mesh") {
			vertices = std::vector<Vertex>();
			indices = std::vector<unsigned int>();
			VAO = 0;
			VBO = 0;
			EBO = 0;
			isSetupOnGPU = false;
			this->name = name;
		}
		inline bool HasSkeleton() {
			return verticesBoneData.size() > 0;
		}
		void SetupOnGPU() {
			if (isSetupOnGPU) return;
			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &VBO);
			glGenBuffers(1, &EBO);
			if (HasSkeleton())
				glGenBuffers(1, &bonesVBO);
			// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
			glBindVertexArray(VAO);

			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &(vertices[0]), GL_STATIC_DRAW);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &(indices[0]), GL_STATIC_DRAW);

			//@Todo format and buffer separation (openGL 4.5)
			u32 offset = 0;
			glEnableVertexAttribArray(VERTEX_POSITION_LOCATION);
			glVertexAttribPointer(VERTEX_POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offset);
			offset += sizeof(Vec3);
			glEnableVertexAttribArray(VERTEX_NORMAL_LOCATION);
			glVertexAttribPointer(VERTEX_NORMAL_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offset);
			offset += sizeof(Vec3);
			glEnableVertexAttribArray(VERTEX_UV_LOCATION);
			glVertexAttribPointer(VERTEX_UV_LOCATION, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offset);
			offset += sizeof(Vec2);

			if (HasSkeleton())
			{
				offset = 0;
				glBindBuffer(GL_ARRAY_BUFFER, bonesVBO);
				glBufferData(GL_ARRAY_BUFFER, verticesBoneData.size() * sizeof(animation::VertexBoneData), &verticesBoneData[0], GL_STATIC_DRAW);
				glEnableVertexAttribArray(BONE_ID_LOCATION);
				glVertexAttribIPointer(BONE_ID_LOCATION, NUM_BONES_PER_VERTEX, GL_UNSIGNED_INT, sizeof(animation::VertexBoneData),(void*)offset);
				offset += sizeof(GLuint) * NUM_BONES_PER_VERTEX;
				glEnableVertexAttribArray(BONE_WEIGHT_LOCATION);
				glVertexAttribPointer(BONE_WEIGHT_LOCATION, NUM_BONES_PER_VERTEX, GL_FLOAT, GL_FALSE, sizeof(animation::VertexBoneData), (void*)offset);
				offset += sizeof(GLfloat) * NUM_BONES_PER_VERTEX;
			}
			// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			// remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
			//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

			// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
			// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
			glBindVertexArray(0);
			isSetupOnGPU = true;
		}
		inline void Bind() {
			glBindVertexArray(VAO);
		}
		void Render() {
			Bind();
			glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		}
		inline void ClearOnGPU() {
			if (!isSetupOnGPU) return;
			glBindVertexArray(0);
			glDeleteVertexArrays(1, &VAO);
			glDeleteBuffers(1, &VBO);
			glDeleteBuffers(1, &EBO);
			isSetupOnGPU = false;

		}
	};

	struct MeshInstance : public ecs::Component {
		// Declare component name
		COMPONENT_NAME("Mesh Instance");
		COMPONENT_REGISTER(MeshInstance);
		// Declare constructor
		MeshInstance(ecs::Scene* scene) : Component(scene) {
			COMPONENT_PUBLIC(meshId);
			COMPONENT_PUBLIC(materialId);
		}

		// Declare datas
		COMPONENT_DATA(size_t, meshId);
		COMPONENT_DATA(size_t, materialId);
		COMPONENT_DATA(boolean, initialized);

		void SetMesh(ecs::component_id index, size_t meshId);
		void SetMaterial(ecs::component_id index, size_t materialId);
		void SetMeshMaterial(ecs::component_id index, size_t newMeshId, size_t newMaterialId);

		virtual void OnEntityDuplicate(ecs::entity_id id) override;
		virtual void OnEntityLoaded(ecs::entity_id id) override;
		virtual void OnComponentAddInspector(ecs::entity_id id) override;

		// On Data added
		virtual void AddDatas(ecs::entity_id id) override;

		// On Data removed
		virtual void RemoveDatas(ecs::component_id index, ecs::entity_id id) override;
	};

	struct Renderer {
		ecs::Scene* scene;
		ecs::core::Camera* camera;
		ecs::core::Transform* transform;
		MeshInstance* meshInstance;
		animation::Skeleton* skeleton;

		Mat4x4 projection;
		Mat4x4 view;

		util::UnorderedMap<size_t, Shader> shaders;
		util::UnorderedMap<size_t, Mesh> meshes;
		util::UnorderedMap<size_t, Material> materials;

		util::UnorderedMap<size_t, util::UnorderedMap<size_t, util::Array<ecs::entity_id>>> instancesByMeshByShader;
		//@Todo organiser par material aussi pour optimiser si plusieurs instances du mesh ont le même material ?

		size_t instanceRenderingThreshold;

		static int widthSaved, heightSaved;

		Renderer(ecs::Scene* _scene);
		~Renderer();

		/**
		 * Permet d'ajouter un mesh par son id
		 */
		inline void AddMesh(Mesh mesh, size_t idMesh) {
			meshes[idMesh] = mesh;
			meshes[idMesh].SetupOnGPU();
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
			instancesByMeshByShader[idShader] = util::UnorderedMap<size_t, util::Array<ecs::entity_id>>();
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
			materials[idMaterial] = material;
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

		inline bool MeshExistInShader(size_t idShader, size_t idMesh) {
			return instancesByMeshByShader[idShader].find(idMesh) != instancesByMeshByShader[idShader].end();
		}

		void AddMeshMaterial(size_t idMaterial, size_t idMesh, ecs::entity_id entity);
		void RemoveMeshMaterial(size_t idMaterial, size_t idMesh, ecs::entity_id entity);

		void AddMeshShader(size_t idShader, size_t idMesh, ecs::entity_id entity);
		void RemoveMeshShader(size_t idShader, size_t idMesh, ecs::entity_id entity);

		void ReGenerateMeshByShader() {
			instancesByMeshByShader.clear();
			for (size_t i = 0, size = meshInstance->size(); i < size; i++) {
				AddMeshMaterial(meshInstance->materialId[i], meshInstance->meshId[i], meshInstance->getId(i));
			}
		}

		void GenerateMaterials();
		void GenerateMeshes();
		void GenerateShaders();

		Shader& defaultShader() {
			return shaders[0];
		}

		/**
		 * Permet de rendre la scène
		 */
		void Render();//@Todo register draw calls and num entities rendered per frame
		void RenderScene();

		u32 BloomEffect(core::rendering::PostProcess& postProcess); //Return the ping pong value FB

		/*********************************************/
		/* STATIC SYSTEM							 */
		/*********************************************/

		static bool frameBufferInitialized;

		static FrameBuffer FBAA; //Frame buffer anti aliasing
		static FrameBuffer FBFeature; //Frame buffer Features

		// Post Process Buffer
		static FrameBuffer FBBloomPingPong[];

		static void InitializeBuffers();
		static void ReloadBuffers();
	};
}