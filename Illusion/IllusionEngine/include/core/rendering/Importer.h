#pragma once
#include "core/rendering/Renderer.h"
#include "assimp/cimport.h"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "glm/gtx/matrix_decompose.hpp"

#include <filesystem>
namespace fs = std::filesystem;

#include "resources/Project.h"



namespace illusion {
	// For converting between ASSIMP and glm
	static inline glm::vec3 vec3_convert(const aiVector3D& v) { return glm::vec3(v.x, v.y, v.z); }
	static inline glm::vec2 vec2_convert(const aiVector3D& v) { return glm::vec2(v.x, v.y); }
	static inline glm::quat quat_convert(const aiQuaternion& q) { return glm::quat(q.w, q.x, q.y, q.z); }
	static inline glm::mat4 mat4_convert(const aiMatrix4x4& m) { return glm::transpose(glm::make_mat4(&m.a1)); }
	//mat3
	static inline glm::mat4 mat4_convert(const aiMatrix3x3& m) { return glm::transpose(glm::make_mat3(&m.a1)); }


	Mesh ConvertToMesh(const aiMesh* aimesh, std::string& name, std::string& group) {
		Mesh mesh;
		//if (aimesh->HasNormals()) {
		//	INFO("HasNormals");
		//}
		//if (aimesh->HasTextureCoords(0)) {
		//	INFO("HasUvs");
		//}
		for (int i = 0;i < aimesh->mNumVertices; i++) {
			auto current_Position = aimesh->mVertices[i];
			current_Position *= 0.01f;
			auto current_Normal = aimesh->mNormals[i];
			aiVector3D current_Uv;
			if (aimesh->HasTextureCoords(0)) {
				current_Uv = aimesh->mTextureCoords[0][i];
			}
			else {
				current_Uv = aiVector3D(0, 0, 0);
			}
			Vertex vertex = {
				Vec3(current_Position.x,current_Position.y,current_Position.z)
				,Vec3(current_Normal.x,current_Normal.y,current_Normal.z)
				,Vec2(current_Uv.x,current_Uv.y)
			};
			mesh.vertices.push_back(vertex);
		}
		for (int i = 0;i < aimesh->mNumFaces; i++) {
			auto current = aimesh->mFaces[i];
			//only triangles supported
			mesh.indices.push_back(current.mIndices[0]);
			mesh.indices.push_back(current.mIndices[1]);
			mesh.indices.push_back(current.mIndices[2]);
		}

		mesh.name = name;
		mesh.group = group;

		return mesh;
	}
	//Generate ECS hierarchy without adding Meshes to renderer
	void processNode(const char* path, aiNode* node, const aiScene* ai_scene, illusion::ecs::Scene& scene, ecs::entity_id parentId) {
		illusion::ecs::entity_id id = scene.CreateEntity();
		ecs::core::Transform* transform = scene.GetComponent<ecs::core::Transform>();
		ecs::component_id transform_id = transform->getIndex(id);
		transform->SetParent(id, parentId);
		transform->name[transform_id] = node->mName.C_Str();
		Mat4x4 transformation = mat4_convert(node->mTransformation);
		Vec3 position, scale, skew;
		Quaternion rotation;
		Vec4 perspective;
		glm::decompose(transformation, scale, rotation, position, skew, perspective);
		transform->position[transform_id] = position;
		transform->rotation[transform_id] = glm::conjugate(rotation); // Conjugate rotation to apply the correct quaternion with decompose
		transform->scale[transform_id] = scale;
		// @Todo : support multiples meshes on the same node		
		if (node->mNumMeshes > 0) {
			MeshInstance& meshInstance = *(scene.GetComponent<MeshInstance>());
			scene.EntityAddComponent<MeshInstance>(id);
			INFO("num meshes : ", node->mNumMeshes);
			unsigned int ai_meshid = node->mMeshes[0];
			std::string relativePath = fs::relative(path, resources::CurrentProject().path + "/Assets/").string();
			size_t mesh_id = std::hash<std::string>{}(relativePath + std::to_string(ai_meshid));
			//Set mesh reference
			meshInstance.SetMesh(meshInstance.getIndex(id), mesh_id);				
			//AddMeshInstance to Renderer
			scene.renderer->AddMeshShader(0, mesh_id, id);
		}
		// then do the same for each of its children
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			processNode(path, node->mChildren[i], ai_scene, scene, id);
		}
	}
	//Load meshes in the renderer
	void loadNode(const char* path, aiNode* node, const aiScene* ai_scene, illusion::Renderer& renderer) {
		// @Todo : support multiples meshes on the same node		
		if (node->mNumMeshes > 0) {
			unsigned int ai_meshid = node->mMeshes[0];
			std::string relativePath = fs::relative(path, resources::CurrentProject().path + "/Assets/").string();
			size_t mesh_id = std::hash<std::string>{}(relativePath + std::to_string(ai_meshid));
			std::string group =fs::path(relativePath).filename().string();
			//get materialId hash 
			//Set MeshInstance materialId
			//if don't contains 
				//Get Matrial from Assimp
				//Add Material to Renderer
			if(!renderer.ContainsMesh(mesh_id)){
				std::string meshName = node->mName.C_Str();
				Mesh mesh = ConvertToMesh(ai_scene->mMeshes[ai_meshid],meshName,group);
				//Add Mesh to renderer
				renderer.AddMesh(mesh,mesh_id);
			}
		}
		// then do the same for each of its children
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			loadNode(path, node->mChildren[i], ai_scene, renderer);
		}
	}
	void import3DModel(const char* path, illusion::ecs::Scene& scene)
	{
		illusion::Renderer* renderer = scene.renderer;
		const aiScene* ai_scene = aiImportFile(path,
			aiProcess_CalcTangentSpace |
			aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices |
			aiProcess_SortByPType);

		if (!ai_scene || ai_scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !ai_scene->mRootNode)
		{
			ERR("ASSIMP : ", aiGetErrorString());
			return;
		}
		loadNode(path,ai_scene->mRootNode, ai_scene, *(scene.renderer));
		processNode(path,ai_scene->mRootNode, ai_scene, scene, (ecs::entity_id)illusion::ecs::id::invalid_id);
		aiReleaseImport(ai_scene);
	}
	void load3DModel(const char* path, illusion::Renderer& renderer)
	{
		const aiScene* ai_scene = aiImportFile(path,
			aiProcess_CalcTangentSpace |
			aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices |
			aiProcess_SortByPType);

		if (!ai_scene || ai_scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !ai_scene->mRootNode)
		{
			ERR("ASSIMP : ", aiGetErrorString());
			return;
		}
		loadNode(path,ai_scene->mRootNode, ai_scene, renderer);
		aiReleaseImport(ai_scene);
	}
}