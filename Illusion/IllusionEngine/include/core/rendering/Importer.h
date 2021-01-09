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
#include "core/animation/CoreComponents/Skeleton.h"
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
	static std::string GetParentRelativePath(aiNode* ai_parent, aiNode* ai_node) {
		if (ai_node == nullptr) {
			INTERNAL_ERR("NULL NODE ");
			return ".";
		}
		if (ai_node == ai_parent) {
			return ".";
		}
		return GetParentRelativePath(ai_parent, ai_node->mParent) + "/..";
	}
	static std::string GetNodeRelativePath(aiNode* ai_parent, aiNode* ai_node) {
		if (ai_node == nullptr) {
			INTERNAL_ERR("NULL NODE ");
			return ".";
		}
		if (ai_node == ai_parent) {
			return ".";
		}
		return GetNodeRelativePath(ai_parent, ai_node->mParent) + "/" + ai_node->mName.C_Str();
	}

	static animation::Key<Vec3> ConvertToKey(const aiVectorKey& ai_key, double ticksPerSeconds) {
		animation::Key<Vec3> key;
		key.value = vec3_convert(ai_key.mValue);
		key.time = ai_key.mTime / ticksPerSeconds;
		return key;

	}
	static animation::Key<Quaternion> ConvertToKey(const aiQuatKey& ai_key, double ticksPerSeconds) {
		animation::Key<Quaternion> key;
		key.value = quat_convert(ai_key.mValue);
		key.time = ai_key.mTime / ticksPerSeconds;
		return key;
	}

	static animation::Bone ConvertToBone(const aiScene* ai_scene, aiNode* ai_node, aiBone* ai_bone) {
		animation::Bone bone;
		bone.offset = mat4_convert(ai_bone->mOffsetMatrix);
		aiNode* ai_bonenode = ai_scene->mRootNode->FindNode(ai_bone->mName);
		std::string nodePath = GetParentRelativePath(ai_scene->mRootNode, ai_node);
		std::string bonePath = GetNodeRelativePath(ai_scene->mRootNode, ai_bonenode);
		bone.relativePath = nodePath +"/"+ bonePath;
		return bone;
	}

	static inline animation::Channel ConvertToChannel(const aiScene* ai_scene, aiNodeAnim* ai_channel, double ticksPerSeconds) {
		animation::Channel channel;
		//Get RelativePath
		aiNode* ai_node = ai_scene->mRootNode->FindNode(ai_channel->mNodeName);
		channel.relativePath = GetNodeRelativePath(ai_scene->mRootNode, ai_node);
		INTERNAL_INFO("nodeName : ", ai_channel->mNodeName.C_Str());
		INTERNAL_INFO("relativePath : ", channel.relativePath);
		INTERNAL_INFO("positionKeys : ", ai_channel->mNumPositionKeys);
		INTERNAL_INFO("rotationKeys : ", ai_channel->mNumRotationKeys);
		INTERNAL_INFO("scaleKeys : ", ai_channel->mNumScalingKeys);
		aiVectorKey* ai_positionKeys = ai_channel->mPositionKeys;
		for (int i = 0; i < ai_channel->mNumPositionKeys; i++) {
			animation::Key<Vec3> key = ConvertToKey(ai_positionKeys[i], ticksPerSeconds);
			key.value *= 0.01f;
			channel.positionKeys.push_back(key);
		}
		aiQuatKey* ai_rotationKeys = ai_channel->mRotationKeys;
		for (int i = 0; i < ai_channel->mNumRotationKeys; i++) {
			channel.rotationKeys.push_back(ConvertToKey(ai_rotationKeys[i], ticksPerSeconds));
		}
		aiVectorKey* ai_scaleKeys = ai_channel->mScalingKeys;
		for (int i = 0; i < ai_channel->mNumScalingKeys; i++) {
			channel.scaleKeys.push_back(ConvertToKey(ai_scaleKeys[i], ticksPerSeconds));
		}
		return channel;
	}
	static animation::Animation ConvertToAnimation(const aiScene* ai_scene, aiAnimation* ai_animation) {
		animation::Animation animation;
		//animation.
		animation.name = ai_animation->mName.C_Str();
		animation.duration = ai_animation->mDuration / ai_animation->mTicksPerSecond;
		INTERNAL_INFO("Animation : ", animation.name);
		INTERNAL_INFO("mDuration : ", ai_animation->mDuration);
		INTERNAL_INFO("mTicksPerSecond : ", ai_animation->mTicksPerSecond);
		INTERNAL_INFO("convert duration : ", animation.duration);
		INTERNAL_INFO("Channels : ", ai_animation->mNumChannels);
		aiNodeAnim** ai_channels = ai_animation->mChannels;
		for (int i = 0; i < ai_animation->mNumChannels; i++) {
			animation.channels.push_back(ConvertToChannel(ai_scene, ai_channels[i], ai_animation->mTicksPerSecond));
		}
		return animation;
	}
	static animation::AnimatorElement GetAnimatorElement(const aiScene* ai_scene) {
		animation::AnimatorElement animator;
		aiAnimation** ai_animations = ai_scene->mAnimations;
		INTERNAL_INFO("Nombre d'animations importées : ", ai_scene->mNumAnimations);
		for (int i = 0; i < ai_scene->mNumAnimations; i++) {
			animator.animations.push_back(ConvertToAnimation(ai_scene, ai_animations[0]));
		}
		return animator;
	}

	static Mesh ConvertToMesh(const aiMesh* ai_mesh, std::string& name, std::string& group) {
		INTERNAL_INFO("Try to convert to mesh");
		Mesh mesh;
		//if (ai_mesh->HasNormals()) {
		//	INFO("HasNormals");
		//}
		//if (ai_mesh->HasTextureCoords(0)) {
		//	INFO("HasUvs");
		//}
		for (int i = 0;i < ai_mesh->mNumVertices; i++) {
			auto current_Position = ai_mesh->mVertices[i];
			current_Position *= 0.01f;
			auto current_Normal = ai_mesh->mNormals[i];
			aiVector3D current_Uv;
			if (ai_mesh->HasTextureCoords(0)) {
				current_Uv = ai_mesh->mTextureCoords[0][i];
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
		for (int i = 0;i < ai_mesh->mNumFaces; i++) {
			auto current = ai_mesh->mFaces[i];
			//only triangles supported
			mesh.indices.push_back(current.mIndices[0]);
			mesh.indices.push_back(current.mIndices[1]);
			mesh.indices.push_back(current.mIndices[2]);
		}
		if (ai_mesh->HasBones()) {
			INTERNAL_INFO("HasBones");
			//get Weights and bone ids
			mesh.verticesBoneData.resize(ai_mesh->mNumVertices);
			INTERNAL_INFO("Num Bones :", ai_mesh->mNumBones);
			for (int i = 0;i < ai_mesh->mNumBones; i++) {
				aiBone* ai_bone = ai_mesh->mBones[i];
				INTERNAL_INFO("Num Weights :", ai_bone->mNumWeights);
				for (int j = 0; j < ai_bone->mNumWeights; j++) {
					aiVertexWeight& vertexWeight = ai_bone->mWeights[j];
					for (int k = 0; k < NUM_BONES_PER_VERTEX; k++) {
						if (mesh.verticesBoneData[vertexWeight.mVertexId].weights[k] == 0.0f) {
							mesh.verticesBoneData[vertexWeight.mVertexId].weights[k] = vertexWeight.mWeight;
							mesh.verticesBoneData[vertexWeight.mVertexId].ids[k] = i;
						}
					}
				}
			}
			INTERNAL_INFO("End Vertex Bone Weights");


		}
		mesh.name = name;
		mesh.group = group;

		INTERNAL_INFO("END LOAD BONES ", mesh.name, " ", mesh.group);

		return mesh;
	}
	//Generate ECS hierarchy without adding Meshes to renderer
	static ecs::entity_id processNode(const char* path, aiNode* ai_node, const aiScene* ai_scene, illusion::ecs::Scene& scene, ecs::entity_id parentId) {
		illusion::ecs::entity_id id = scene.CreateEntity();
		ecs::core::Transform* transform = scene.GetComponent<ecs::core::Transform>();
		ecs::component_id transform_id = transform->getIndex(id);
		transform->SetParent(id, parentId);
		transform->name[transform_id] = ai_node->mName.C_Str();
		Mat4x4 transformation = mat4_convert(ai_node->mTransformation);
		Vec3 position, scale, skew;
		Quaternion rotation;
		Vec4 perspective;
		glm::decompose(transformation, scale, rotation, position, skew, perspective);
		transform->position[transform_id] = position * 0.01f;
		transform->rotation[transform_id] = glm::conjugate(rotation); // Conjugate rotation to apply the correct quaternion with decompose
		transform->scale[transform_id] = scale;
		// @Todo : support multiples meshes on the same ai_node		
		if (ai_node->mNumMeshes > 0) {
			MeshInstance& meshInstance = *(scene.GetComponent<MeshInstance>());
			scene.EntityAddComponent<MeshInstance>(id);
			INFO("num meshes : ", ai_node->mNumMeshes);
			unsigned int ai_meshid = ai_node->mMeshes[0];
			std::string relativePath = fs::relative(path, resources::CurrentProject().path + "/Assets/").string();
			size_t mesh_id = std::hash<std::string>{}(relativePath + std::to_string(ai_meshid));
			//@Todo support Material on models
			//Set mesh reference
			meshInstance.SetMesh(meshInstance.getIndex(id), mesh_id);
			//AddMeshInstance to Renderer
			scene.renderer->AddMeshShader(0, mesh_id, id);
			//Skeleton 
			aiMesh* ai_mesh = ai_scene->mMeshes[ai_meshid];
			if (ai_mesh->HasBones()) {
				animation::Skeleton& skeletonComponent = *(scene.GetComponent<animation::Skeleton>());
				scene.EntityAddComponent<animation::Skeleton>(id);
				illusion::ecs::component_id skeleton_id = skeletonComponent.getIndex(id);
				//Add Bones
				util::Array<animation::Bone>& bones = skeletonComponent.bones[skeleton_id];
				INTERNAL_INFO("NUM BONES : ", ai_mesh->mNumBones);
				if (ai_mesh->mNumBones > NUM_BONES_PER_MESH) {
					INTERNAL_ERR("too much bones - max : ", NUM_BONES_PER_MESH);
				}
				for (int i = 0, size = ai_mesh->mNumBones; i < size;i++) {
					bones.push_back(ConvertToBone(ai_scene, ai_node, ai_mesh->mBones[i]));
				}
			}
		}

		// then do the same for each of its children
		for (unsigned int i = 0; i < ai_node->mNumChildren; i++)
		{
			processNode(path, ai_node->mChildren[i], ai_scene, scene, id);
		}
		return id;
	}
	//Load meshes in the renderer
	static void loadNode(const char* path, aiNode* node, const aiScene* ai_scene, illusion::Renderer& renderer) {
		INTERNAL_INFO("LOAD NODE ");
		// @Todo : support multiples meshes on the same node		
		if (node->mNumMeshes > 0) {
			unsigned int ai_meshid = node->mMeshes[0];
			std::string relativePath = fs::relative(path, resources::CurrentProject().path + "/Assets/").string();
			size_t mesh_id = std::hash<std::string>{}(relativePath + std::to_string(ai_meshid));
			std::string group = fs::path(relativePath).filename().string();
			//@Todo support Material on models
			//get materialId hash 
			//Set MeshInstance materialId
			//if don't contains 
				//Get Matrial from Assimp
				//Add Material to Renderer
			if (!renderer.ContainsMesh(mesh_id)) {
				std::string meshName = node->mName.C_Str();
				//Add Mesh to renderer
				Mesh mesh = ConvertToMesh(ai_scene->mMeshes[ai_meshid], meshName, group);
				renderer.AddMesh(mesh, mesh_id);
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
		loadNode(path, ai_scene->mRootNode, ai_scene, *(scene.renderer));
		ecs::entity_id rootEntity_id = processNode(path, ai_scene->mRootNode, ai_scene, scene, (ecs::entity_id)illusion::ecs::id::invalid_id);
		ecs::core::Transform& transform = *scene.GetComponent<ecs::core::Transform>();
		std::string relativePath = fs::relative(path, resources::CurrentProject().path + "/Assets/").string();
		std::stringstream filename = std::stringstream(fs::path(relativePath).filename().string());
		std::string entityname;
		std::getline(filename, entityname, '.');
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
		loadNode(path, ai_scene->mRootNode, ai_scene, renderer);
		aiReleaseImport(ai_scene);
	}

}