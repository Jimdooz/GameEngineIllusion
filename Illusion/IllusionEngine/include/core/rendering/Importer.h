#pragma once
#include "core/rendering/Renderer.h"
#include "assimp/cimport.h"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "glm/gtx/matrix_decompose.hpp"

#include <filesystem>
namespace fs = std::filesystem;


#include "resources/Project.h"
#include "core/animation/Animation.h"
#include "core/animation/CoreComponents/Animator.h"
namespace animation = illusion::core::animation;


namespace illusion {
	// For converting between ASSIMP and glm
	static inline glm::vec3 vec3_convert(const aiVector3D& v) { return glm::vec3(v.x, v.y, v.z); }
	static inline glm::vec2 vec2_convert(const aiVector3D& v) { return glm::vec2(v.x, v.y); }
	static inline glm::quat quat_convert(const aiQuaternion& q) { return glm::quat(q.w, q.x, q.y, q.z); }
	static inline glm::mat4 mat4_convert(const aiMatrix4x4& m) { return glm::transpose(glm::make_mat4(&m.a1)); }
	//mat3
	static inline glm::mat4 mat4_convert(const aiMatrix3x3& m) { return glm::transpose(glm::make_mat3(&m.a1)); }

	static animation::Key<Vec3> ConvertToKey(const aiVectorKey& ai_key,double ticksPerSeconds){
		animation::Key<Vec3> key;
		key.value = vec3_convert(ai_key.mValue);
		key.time = ai_key.mTime/ticksPerSeconds;
		return key;

	}
	static animation::Key<Quaternion> ConvertToKey(const aiQuatKey& ai_key,double ticksPerSeconds){
		animation::Key<Quaternion> key;
		key.value = quat_convert(ai_key.mValue);
		key.time = ai_key.mTime/ticksPerSeconds;
		return key;
	}

	static std::string GetNodeRelativePathToRoot(aiNode* ai_node){
		std::string name=ai_node->mName.C_Str();
		if(ai_node->mParent==nullptr){
			return ".";
		}
		return GetNodeRelativePathToRoot(ai_node->mParent)+"/"+name;
	}
	static inline animation::Channel ConvertToChannel(const aiScene* ai_scene,aiNodeAnim* ai_channel,double ticksPerSeconds){
		animation::Channel channel;
		//Get RelativePath
		aiNode* ai_node = ai_scene->mRootNode->FindNode(ai_channel->mNodeName);
		channel.relativePath=GetNodeRelativePathToRoot(ai_node);
		INTERNAL_INFO("nodeName : ",ai_channel->mNodeName.C_Str());
		INTERNAL_INFO("relativePath : ",channel.relativePath);
		INTERNAL_INFO("positionKeys : ", ai_channel->mNumPositionKeys);
		INTERNAL_INFO("rotationKeys : ",ai_channel->mNumRotationKeys);
		INTERNAL_INFO("scaleKeys : ",ai_channel->mNumScalingKeys);
		aiVectorKey *ai_positionKeys=ai_channel->mPositionKeys;
		for (int i = 0; i < ai_channel->mNumPositionKeys; i++) {
			animation::Key<Vec3> key = ConvertToKey(ai_positionKeys[i], ticksPerSeconds);
			key.value *= 0.01f;
			channel.positionKeys.push_back(key);
		}
		aiQuatKey *ai_rotationKeys=ai_channel->mRotationKeys;
		for (int i = 0; i < ai_channel->mNumRotationKeys; i++) {
			channel.rotationKeys.push_back(ConvertToKey(ai_rotationKeys[i],ticksPerSeconds));
		}
		aiVectorKey *ai_scaleKeys=ai_channel->mScalingKeys;
		for (int i = 0; i < ai_channel->mNumScalingKeys; i++) {
			channel.scaleKeys.push_back(ConvertToKey(ai_scaleKeys[i],ticksPerSeconds));
		}
		return channel;
	}
	static animation::Animation ConvertToAnimation(const aiScene* ai_scene,aiAnimation* ai_animation){
		animation::Animation animation;
		//animation.
		animation.name = ai_animation->mName.C_Str();
		INTERNAL_INFO("Animation : ",animation.name);
		animation.duration = ai_animation->mDuration / ai_animation->mTicksPerSecond;
		INTERNAL_INFO("mDuration : ",ai_animation->mDuration);
		INTERNAL_INFO("mTicksPerSecond : ",ai_animation->mTicksPerSecond);
		INTERNAL_INFO("convert duration : ",animation.duration);
		INTERNAL_INFO("Channels : ",ai_animation->mNumChannels);
		aiNodeAnim** ai_channels = ai_animation->mChannels;
		for(int i = 0; i < ai_animation->mNumChannels; i++){
			animation.channels.push_back(ConvertToChannel(ai_scene,ai_channels[i],ai_animation->mTicksPerSecond));
		}
		return animation;
	}
	static animation::AnimatorElement GetAnimatorElement(const aiScene* ai_scene){
		animation::AnimatorElement animator;
		aiAnimation** ai_animations = ai_scene->mAnimations;
		INTERNAL_INFO("Nombre d'animations importées : ",ai_scene->mNumAnimations);
		for (int i = 0; i < ai_scene->mNumAnimations; i++) {
			animator.animations.push_back(ConvertToAnimation(ai_scene,ai_animations[0]));
		}
		return animator;
	}

	static Mesh ConvertToMesh(const aiMesh* aimesh, std::string& name, std::string& group) {
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
	static ecs::entity_id processNode(const char* path, aiNode* node, const aiScene* ai_scene, illusion::ecs::Scene& scene, ecs::entity_id parentId) {
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
		transform->position[transform_id] = position * 0.01f;
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
			//@Todo support Material on models
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
		return id;
	}
	//Load meshes in the renderer
	static void loadNode(const char* path, aiNode* node, const aiScene* ai_scene, illusion::Renderer& renderer) {
		// @Todo : support multiples meshes on the same node		
		if (node->mNumMeshes > 0) {
			unsigned int ai_meshid = node->mMeshes[0];
			std::string relativePath = fs::relative(path, resources::CurrentProject().path + "/Assets/").string();
			size_t mesh_id = std::hash<std::string>{}(relativePath + std::to_string(ai_meshid));
			std::string group =fs::path(relativePath).filename().string();
			//@Todo support Material on models
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
	static void import3DModel(const char* path, illusion::ecs::Scene& scene)
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
		ecs::entity_id rootEntity_id = processNode(path,ai_scene->mRootNode, ai_scene, scene, (ecs::entity_id)illusion::ecs::id::invalid_id);
		ecs::core::Transform& transform = *scene.GetComponent<ecs::core::Transform>(); 
		std::string relativePath = fs::relative(path, resources::CurrentProject().path + "/Assets/").string();
		std::stringstream filename = std::stringstream(fs::path(relativePath).filename().string());
		std::string entityname;
		std::getline(filename,entityname,'.');
		transform.name[transform.getIndex(rootEntity_id)] = entityname;
		if (ai_scene->HasAnimations()) {
			core::animation::AnimatorElement animatorElement = GetAnimatorElement(ai_scene);

			animation::Animator* animators = scene.GetComponent<animation::Animator>();
			scene.EntityAddComponent<animation::Animator>(rootEntity_id);
			animators->animator[animators->getIndex(rootEntity_id)] = animatorElement;
		}

		//Ajouter à l'entity root du model
		aiReleaseImport(ai_scene);
	}
	static void load3DModel(const char* path, illusion::Renderer& renderer)
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