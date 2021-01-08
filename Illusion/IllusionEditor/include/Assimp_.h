#include "IllusionEngine.h"
#include "ecs/Scene.h"
#include "ecs/id.h"

#include "assimp/cimport.h"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "glm/gtx/matrix_decompose.hpp"

namespace illusion{
	// For converting between ASSIMP and glm
	static inline glm::vec3 vec3_convert(const aiVector3D& v) { return glm::vec3(v.x, v.y, v.z); }
	static inline glm::vec2 vec2_convert(const aiVector3D& v) { return glm::vec2(v.x, v.y); }
	static inline glm::quat quat_convert(const aiQuaternion& q) { return glm::quat(q.w, q.x, q.y, q.z); }
	static inline glm::mat4 mat4_convert(const aiMatrix4x4& m) { return glm::transpose(glm::make_mat4(&m.a1)); }
	//mat3
	static inline glm::mat4 mat4_convert(const aiMatrix3x3& m) { return glm::transpose(glm::make_mat3(&m.a1)); }
	
	struct Vertex{
		Vec3 position;
		Vec3 normal;
		Vec2 uv;
	};
	struct Mesh{
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		GLuint VAO,VBO,EBO;
		bool isSetup;
		Mesh(){
			vertices=std::vector<Vertex>();
			indices=std::vector<unsigned int>();
			VAO=0;
			VBO=0;
			EBO=0;
			isSetup=false;
		}
	};
	struct MeshRenderer : public ecs::Component {
		// Declare component name
		COMPONENT_NAME("Mesh Renderer");
		COMPONENT_REGISTER(MeshRenderer);
		// Declare constructor
		MeshRenderer(ecs::Scene* scene) : Component(scene) {}

		// Declare datas
		COMPONENT_DATA(Mesh, mesh);

		// On Data added
		virtual void AddDatas(ecs::entity_id id) override {
			AddData(mesh, Mesh());
		}

		// On Data removed
		virtual void RemoveDatas(ecs::component_id index, ecs::entity_id id) override {
			RemoveData(mesh, index);
		}
	};
	void SetupMesh(Mesh& mesh){    
		INFO("Setup Mesh");
		glGenVertexArrays(1, &(mesh.VAO));
		glGenBuffers(1, &(mesh.VBO));
		glGenBuffers(1, &(mesh.EBO));
		// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
		glBindVertexArray(mesh.VAO);

		glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
		glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(Vertex), &(mesh.vertices[0]), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(unsigned int), &(mesh.indices[0]), GL_STATIC_DRAW);

		u32 offset=0;
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offset);
		offset+=sizeof(Vec3);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offset);
		offset+=sizeof(Vec3);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offset);
		offset+=sizeof(Vec2);
		// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
		glBindBuffer(GL_ARRAY_BUFFER, 0); 

		// remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
		// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
		glBindVertexArray(0); 
		mesh.isSetup=true;
		INFO("End Setup Mesh");
	}

	Mesh ConvertToMesh(const aiMesh *aimesh){
		Mesh mesh;
		if(aimesh->HasNormals()){
			INFO("HasNormals");
		}
		if(aimesh->HasTextureCoords(0)){
			INFO("HasUvs");
		}
		for(unsigned int i=0; i < aimesh->mNumVertices; i++){
			auto current_Position =aimesh->mVertices[i];
			auto current_Normal =aimesh->mNormals[i];
			auto current_Uv =aimesh->mTextureCoords[0][i];
			Vertex vertex={
				Vec3(current_Position.x,current_Position.y,current_Position.z)
				,Vec3(current_Normal.x,current_Normal.y,current_Normal.z)
				,Vec2(current_Uv.x,current_Uv.y)
			};
			INFO(vertex.position.x,",",vertex.position.y,",",vertex.position.z);
			mesh.vertices.push_back(vertex);
		}
		for(unsigned int i=0; i<aimesh->mNumFaces; i++){
			auto current =aimesh->mFaces[i];
			INFO(aimesh->mFaces[i].mNumIndices);
			//only triangles supported
			mesh.indices.push_back(current.mIndices[0]);
			mesh.indices.push_back(current.mIndices[1]);
			mesh.indices.push_back(current.mIndices[2]);
		}
		return mesh;
	}
	void processNode(aiNode *node,const aiScene *ai_scene,illusion::ecs::Scene *scene,ecs::entity_id parentId){
		illusion::ecs::entity_id id=scene->CreateEntity();
		ecs::core::Transform *transform=scene->GetComponent<ecs::core::Transform>();
		ecs::component_id transform_id=transform->getIndex(id);
		transform->SetParent(id,parentId);
		transform->name[transform_id]=node->mName.C_Str();
		Mat4x4 transformation = mat4_convert(node->mTransformation);
		Vec3 position, scale, skew;
		Quaternion rotation;
		Vec4 perspective;
		glm::decompose(transformation, scale, rotation, position, skew, perspective);
		transform->position[transform_id] = position;
		transform->rotation[transform_id] = glm::conjugate(rotation);
		transform->scale[transform_id] = scale;
		// @Todo : support multiples meshes on the same node		
		if(node->mNumMeshes>0){
			MeshRenderer *meshRenderer = scene->GetComponent<MeshRenderer>();
			scene->EntityAddComponent<MeshRenderer>(id);
			INFO("meshRendererid : ", meshRenderer->getIndex(id));
			Mesh& mesh=meshRenderer->mesh[meshRenderer->getIndex(id)];
			INFO("num meshes : ", node->mNumMeshes);
			unsigned int ai_meshid = node->mMeshes[0];
			mesh=ConvertToMesh(ai_scene->mMeshes[ai_meshid]);
			SetupMesh(mesh);
		}
		// then do the same for each of its children
		for(unsigned int i = 0; i < node->mNumChildren; i++)
		{
			processNode(node->mChildren[i], ai_scene,scene,id);
		}
	}
	void import3DModel(const char * path,illusion::ecs::Scene *scene)
	{
		const aiScene* ai_scene= aiImportFile(path, 
		aiProcess_CalcTangentSpace		 |
		aiProcess_Triangulate            |
		aiProcess_JoinIdenticalVertices  |
		aiProcess_SortByPType);	
		
		if(!ai_scene || ai_scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !ai_scene->mRootNode) 
		{
			ERR("ASSIMP : ", aiGetErrorString());
			return;
		}
		processNode(ai_scene->mRootNode, ai_scene,scene,(ecs::entity_id)illusion::ecs::id::invalid_id);
		aiReleaseImport(ai_scene);
	}
	inline void RenderMesh(Mesh & m){
		//INFO("Render Mesh");
		glBindVertexArray(m.VAO);
		glDrawElements(GL_TRIANGLES, m.indices.size(), GL_UNSIGNED_INT, 0);
		//glBindVertexArray(0); // no need to unbind it every time 
		//INFO("End Render Mesh");
	}
}