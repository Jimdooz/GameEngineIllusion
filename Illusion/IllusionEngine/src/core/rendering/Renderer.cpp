#include "core/rendering/Renderer.h"
#include "ecs/Scene.h"
#include <glm/gtx/matrix_decompose.hpp>

#include "resources/assets/Shaders.h"
#include "resources/assets/Materials.h"
#include "resources/assets/Meshes.h"

#include "core/rendering/Importer.h"


namespace illusion {
	void MeshInstance::OnEntityDuplicate(ecs::entity_id id) {
		ecs::component_id index = getIndex(id);
		SetMesh(getIndex(id), meshId[index]);
	}

	void MeshInstance::OnEntityLoaded(ecs::entity_id id) {
		ecs::component_id index = getIndex(id);
		SetMesh(getIndex(id), meshId[index]);
	}

	void MeshInstance::OnComponentAddInspector(ecs::entity_id id) {
		ecs::component_id index = getIndex(id);
		SetMesh(getIndex(id), meshId[index]);
	}

	void MeshInstance::AddDatas(ecs::entity_id id) {
		AddData(meshId, size_t(0));
		AddData(materialId, size_t(0));
		AddData<boolean>(initialized, false);
	}

	void MeshInstance::RemoveDatas(ecs::component_id index, ecs::entity_id id) {
		scene->renderer->RemoveMeshMaterial(materialId[index], meshId[index], id);
		RemoveData(meshId, index);
		RemoveData(materialId, index);
		RemoveData(initialized, index);
	}

	void MeshInstance::SetMesh(ecs::component_id index, size_t newMeshId) {
		ecs::entity_id id = getId(index);
		scene->renderer->RemoveMeshMaterial(materialId[index], meshId[index], id);
		meshId[index] = newMeshId;
		scene->renderer->AddMeshMaterial(materialId[index], meshId[index], id);
	}

	void MeshInstance::SetMaterial(ecs::component_id index, size_t newMaterialId) {
		ecs::entity_id id = getId(index);
		scene->renderer->RemoveMeshMaterial(materialId[index], meshId[index], id);
		materialId[index] = newMaterialId;
		scene->renderer->AddMeshMaterial(materialId[index], meshId[index], id);
	}

	void MeshInstance::SetMeshMaterial(ecs::component_id index, size_t newMeshId, size_t newMaterialId) {
		ecs::entity_id id = getId(index);
		scene->renderer->RemoveMeshMaterial(materialId[index], meshId[index], id);
		meshId[index] = newMeshId;
		materialId[index] = newMaterialId;
		scene->renderer->AddMeshMaterial(materialId[index], meshId[index], id);
	}

	/**
	 * Renderer Part
	 */

	RenderEngine RenderEngine::main;

	// STATIC PART
	QualitySettings Renderer::qualitySettings;
	bool Renderer::frameBufferInitialized = false;

	int Renderer::widthSaved, Renderer::heightSaved;

	FrameBuffer Renderer::FBAA;
	FrameBuffer Renderer::FBFeature;

	FrameBuffer Renderer::FBDirectShadow;

	FrameBuffer Renderer::FBBloomPingPong[2];

	void Renderer::InitializeBuffers() {
		if (Renderer::frameBufferInitialized) return; //Already Initialized

		Renderer::FBAA.Reserve();
		Renderer::FBAA.GenerateMSTexture(1, 4);
		Renderer::FBAA.GenerateDepthStencil(4);
		Renderer::FBAA.Complete();

		Renderer::FBFeature.Reserve();
		Renderer::FBFeature.GenerateTexture(2);
		Renderer::FBFeature.Complete();

		Renderer::FBDirectShadow.Reserve();
		Renderer::FBDirectShadow.SetBufferDimensions(4096, 4096);
		Renderer::FBDirectShadow.GenerateDepthTexture();
		Renderer::FBDirectShadow.DisableColorBuffer();
		Renderer::FBDirectShadow.Complete();

		//Bloom Frame Buffers
		for (u32 i = 0; i < 2; i++) {
			Renderer::FBBloomPingPong[i].Reserve();
			Renderer::FBBloomPingPong[i].GenerateTexture(1);
			Renderer::FBBloomPingPong[i].Complete();
		}

		Renderer::frameBufferInitialized = true;

		Renderer::widthSaved = Window::width;
		Renderer::heightSaved = Window::height;
	}

	void Renderer::ReloadBuffers() {
		if (!Renderer::frameBufferInitialized) {
			Renderer::InitializeBuffers();
			return; //Already Initialized
		}

		Renderer::FBAA.Reload();
		Renderer::FBFeature.Reload();

		//Bloom Frame Buffers
		for (u32 i = 0; i < 2; i++) {
			Renderer::FBBloomPingPong[i].Reload();
		}

		Renderer::widthSaved = Window::width;
		Renderer::heightSaved = Window::height;
	}

	// OBJECT PART
	Renderer::Renderer(ecs::Scene* _scene) :
		scene(_scene),
		camera(scene->GetComponent<ecs::core::Camera>()),
		transform(scene->GetComponent<ecs::core::Transform>()),
		meshInstance(scene->GetComponent<MeshInstance>()),
		skeleton(scene->GetComponent<animation::Skeleton>()),
		instanceRenderingThreshold(6)
	{
		Renderer::InitializeBuffers();

		auto shaders = GetRenderEngine().shaders;
		for (auto const& [idShader, val] : shaders) {
			instancesByMeshByShader[idShader] = util::UnorderedMap<size_t, util::Array<ecs::entity_id>>();
		}
		/*GenerateShaders();
		GenerateMaterials();
		GenerateMeshes();*/
	}

	Renderer::~Renderer() {}

	void Renderer::AddMeshMaterial(size_t idMaterial, size_t idMesh, ecs::entity_id entity) {
		if (!GetRenderEngine().ContainsMaterial(idMaterial) || !GetRenderEngine().ContainsMesh(idMesh) || !scene->entities.IsAlive(entity)) return;
		AddMeshShader(GetRenderEngine().materials[idMaterial].shaderId, idMesh, entity);
	}

	void Renderer::RemoveMeshMaterial(size_t idMaterial, size_t idMesh, ecs::entity_id entity) {
		if (!GetRenderEngine().ContainsMaterial(idMaterial) || !GetRenderEngine().ContainsMesh(idMesh)) return;
		RemoveMeshShader(GetRenderEngine().materials[idMaterial].shaderId, idMesh, entity);
	}

	void Renderer::AddMeshShader(size_t idShader, size_t idMesh, ecs::entity_id entity) {
		if (!GetRenderEngine().ContainsShader(idShader) || !GetRenderEngine().ContainsMesh(idMesh) || !scene->entities.IsAlive(entity)) return;

		if (!MeshExistInShader(idShader, idMesh)) {
			//Cr�ation de l'espace pour le mesh
			instancesByMeshByShader[idShader][idMesh] = util::Array<ecs::entity_id>();
		}

		instancesByMeshByShader[idShader][idMesh].push_back(entity);
	}
	void Renderer::RemoveMeshShader(size_t idShader, size_t idMesh, ecs::entity_id entity) {
		if (!GetRenderEngine().ContainsShader(idShader) || !GetRenderEngine().ContainsMesh(idMesh)) return;
		if (!MeshExistInShader(idShader, idMesh)) {
			WARN("Mesh doesn't exist in [instancesByMeshByShader] but entity try to be removed from him");
			return;
		}

		for (u32 i = 0; i < instancesByMeshByShader[idShader][idMesh].size(); i++) {
			if (instancesByMeshByShader[idShader][idMesh][i] == entity || !scene->entities.IsAlive(instancesByMeshByShader[idShader][idMesh][i])) {
				instancesByMeshByShader[idShader][idMesh].erase(
					instancesByMeshByShader[idShader][idMesh].begin() + i
				);
				i--;
			}
		}

		//INFO("GET : ", instancesByMeshByShader[idShader][idMesh].size());

		if (instancesByMeshByShader[idShader][idMesh].size() == 0) instancesByMeshByShader[idShader].erase(instancesByMeshByShader[idShader].find(idMesh));

	}

	void Renderer::Render() {//@Todo register draw calls and num entities rendered per frame
		if (Window::width==0 || Window::height == 0) {
			//INTERNAL_ERR("Window size is 0, the scene can't be rendered");
			return;
		}
		if (camera->size() < 1) {
			//INTERNAL_ERR("No Camera, the scene can't be rendered");
			return;
		}

		/**
		 * Frame Buffer
		 * FBAA : Frame buffer anti aliasing
		 * FBFeature : Frame buffer to extract features & create main texture
		 *	- Texture 0 : Main rendering
		 *  - Texture 1 : Bloom
		 * { Frame Buffer Post Process } : Depend on effects created
		 * FBMain = 0 : Render the all system with effects etc etc...
		 *	- Texture 0 : Main rendering
		 *  - Texture 1 : Bloom
		 *  - Texture 2 : Shadow Map
		 *	- etc...
		 */
		if (Renderer::widthSaved != Window::width || Renderer::heightSaved != Window::height) {
			INTERNAL_INFO("RELOAD BUFFER");
			Renderer::ReloadBuffers();
		}

		core::rendering::PostProcess& postProcess = *scene->GetComponent<core::rendering::PostProcess>();
		qualitySettings.usePostProcess = postProcess.size() > 0;

		qualitySettings.Reset();

		if (qualitySettings.usePostProcess) {
			qualitySettings.useHdr = postProcess.hdrEffect[0];
			qualitySettings.useShadow = postProcess.shadowEffect[0];
			qualitySettings.useBloom = postProcess.bloomEffect[0];
		}

		if (qualitySettings.useHdr) {
			qualitySettings.hdrIntensity = 1.0f;
			qualitySettings.gamma = postProcess.gamma[0];
			qualitySettings.gamma = qualitySettings.gamma <= 0 ? 0.001 : qualitySettings.gamma;
			qualitySettings.exposure = postProcess.exposure[0];
		}

		if (qualitySettings.useShadow) {
			qualitySettings.shadowIntensity = postProcess.shadowIntensity[0];
			qualitySettings.shadowIntensity = qualitySettings.shadowIntensity > 1.0f ? 1.0f : qualitySettings.shadowIntensity < 0.0f ? 0.0f : qualitySettings.shadowIntensity;
			qualitySettings.shadowSmooth = postProcess.shadowSmooth[0];
			if(qualitySettings.shadowSmooth < 0) qualitySettings.shadowSmooth;

			qualitySettings.shadowWidth = postProcess.shadowWidth[0];
			qualitySettings.shadowHeight = postProcess.shadowHeight[0];

			if (qualitySettings.shadowQuality != postProcess.shadowQuality[0]) {
				if (postProcess.shadowQuality[0] > 10000) postProcess.shadowQuality[0] = 10000;
				qualitySettings.shadowQuality = postProcess.shadowQuality[0];
				Renderer::FBDirectShadow.Delete();
				Renderer::FBDirectShadow.Reserve();
				Renderer::FBDirectShadow.SetBufferDimensions(qualitySettings.shadowQuality, qualitySettings.shadowQuality);
				Renderer::FBDirectShadow.GenerateDepthTexture();
				Renderer::FBDirectShadow.DisableColorBuffer();
				Renderer::FBDirectShadow.Complete();
			}
		}

		if (qualitySettings.useBloom) {
			qualitySettings.bloomThreshold = postProcess.bloomThreshold[0];
			qualitySettings.bloomThreshold = qualitySettings.bloomThreshold < 0 ? 0 : qualitySettings.bloomThreshold;
			qualitySettings.bloomIntensity = postProcess.bloomIntensity[0];
			qualitySettings.bloomIntensity = qualitySettings.bloomIntensity < 0 ? 0 : qualitySettings.bloomIntensity;
		}

		// AA first pass & Render Main Scene
		glEnable(GL_DEPTH_TEST);

		if (qualitySettings.useShadow && qualitySettings.shadowIntensity > 0.0f) {
			Renderer::FBDirectShadow.ApplyViewPort();
			Renderer::FBDirectShadow.Bind();
			Renderer::FBDirectShadow.Clear();
			Shader::shadowShader.use();
			glDisable(GL_CULL_FACE);
			Renderer::RenderScene(&Shader::shadowShader);
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
		}

		Renderer::FBAA.ApplyViewPort();
		Renderer::FBAA.Bind();
		Renderer::FBAA.Clear();

		Renderer::FBDirectShadow.BindDepthTexture(0); //Bind Depth direct shadow
		Renderer::RenderScene();

		// 1. Post Processing Pass
		glDisable(GL_DEPTH_TEST);
		GetRenderEngine().GetQuad().Bind();

		// 2. Feature extractions
		Shader::featureShader.use();
		Shader::featureShader.setVec2("screenSize", Vec2(Window::width, Window::height));
		Shader::featureShader.setFloat("bloomThreshold", qualitySettings.bloomThreshold);

		Renderer::FBFeature.Bind();
		Renderer::FBFeature.Clear();

		Renderer::FBAA.BindMSTexture(0);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); //DRAW QUAD

		// POST PROCESS EFFECT
		u32 pingPongValue = 0;
		if(qualitySettings.useBloom) pingPongValue = BloomEffect(postProcess);

		// FINAL RENDER
		glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default
		glClear(GL_COLOR_BUFFER_BIT);

		Shader::screenShader.use();
		//Shader Screen : Textures datas
		Renderer::FBFeature.BindTexture(0, 0);						//Main Scene
		if (qualitySettings.useBloom) Renderer::FBBloomPingPong[pingPongValue].BindTexture(0, 1); //Bloom result

		//Shader Screen : Post process datas
		Shader::screenShader.setFloat("hdrIntensity", qualitySettings.hdrIntensity);
		Shader::screenShader.setFloat("gamma", qualitySettings.gamma);
		Shader::screenShader.setFloat("exposure", qualitySettings.exposure);
		Shader::screenShader.setFloat("bloomIntensity", qualitySettings.bloomIntensity);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);		//DRAW QUAD

		glEnable(GL_DEPTH_TEST);
	}

	void Renderer::RenderScene(Shader *overrideShader) {
		//@Todo change projection only if one of these values are changed
		float aspect = (float)Window::width / (float)Window::height;

		//Compute Camera Position
		ecs::entity_id cameraId = camera->ToEntity[0];
		ecs::component_id cameraIdTransform = transform->getIndex(cameraId);
		Vec3 cameraWorldPos = camera->GetPosition();

		projection = camera->GetProjection();// glm::perspective(camera->fov[0], aspect, camera->near[0], camera->far[0]);
		view = camera->GetView();

		core::rendering::DirectionalLight* lights = scene->GetComponent<core::rendering::DirectionalLight>();
		core::rendering::PointLight* pointLights = scene->GetComponent<core::rendering::PointLight>();

		glm::mat4 lightSpaceMatrix;
		Vec3 lightDirection;
		Vec3 lightTranslation;
		if (lights->size() > 0) {
			ecs::entity_id idLight = lights->getId(0);

			glm::vec3 scale; glm::quat rotation;
			glm::vec3 skew; glm::vec4 perspective;
			glm::decompose(transform->ComputeModel(transform->getIndex(idLight)), scale, rotation, lightTranslation, skew, perspective);
			lightDirection = Vec3(glm::toMat4(glm::conjugate(rotation)) * Vec4(0.0, 1.0, 0.0, 1.0));

			float near_plane = 0.01f, far_plane = 100.0f;
			glm::mat4 lightProjection = glm::ortho(-qualitySettings.shadowWidth, qualitySettings.shadowWidth, -qualitySettings.shadowHeight, qualitySettings.shadowHeight, near_plane, far_plane);
			glm::mat4 lightView = glm::lookAt(lightTranslation, lightTranslation + lightDirection, glm::vec3(0.0f, 1.0f, 0.0f));
			lightSpaceMatrix = lightProjection * lightView;
		}


		//for each shader
		for (auto const& [shaderKey, meshMap] : instancesByMeshByShader) {
			Shader* shader = &GetRenderEngine().shaders[shaderKey];
			if (overrideShader != nullptr) shader = overrideShader;
			else shader->use();

			//set view and projection matrices
			shader->setMat4("view", view);
			shader->setMat4("projection", projection);
			shader->setFloat("SkeletonActive", 0.0f);

			shader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
			if (lights->size() > 0) {
				shader->setVec3("dirLight.specular", lights->specular[0]);
				shader->setVec3("dirLight.diffuse", lights->diffuse[0]);
				shader->setVec3("dirLight.ambient", lights->ambient[0]);
				shader->setVec3("dirLight.direction", lightDirection);
				shader->setVec3("dirLight.position", lightTranslation);
			}
			else {
				shader->setVec3("dirLight.specular", 1.0f, 1.0f, 1.0f);
				shader->setVec3("dirLight.diffuse", 0.5f, 0.5f, 0.5f);
				shader->setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
				shader->setVec3("dirLight.ambient", 0.2f, 0.2f, 0.2f);
			}

			size_t pointNumbers = pointLights->size();
			shader->setFloat("nbPointLights", pointNumbers);

			for (u32 i = 0; i < pointNumbers; i++) {
				ecs::entity_id idLight = pointLights->getId(i);

				glm::vec3 scale; glm::quat rotation; glm::vec3 translation;
				glm::vec3 skew; glm::vec4 perspective;
				glm::decompose(transform->ComputeModel(transform->getIndex(idLight)), scale, rotation, translation, skew, perspective);

				std::string start = "pointLights[" + std::to_string(i) + "].";
				shader->setVec3(start + "position", translation);
				shader->setFloat(start + "constant", pointLights->constant[i]);
				shader->setFloat(start + "linear", pointLights->linear[i]);
				shader->setFloat(start + "quadratic", pointLights->quadratic[i]);

				shader->setVec3(start + "ambient", pointLights->ambient[i]);
				shader->setVec3(start + "diffuse", pointLights->diffuse[i]);
				shader->setVec3(start + "specular", pointLights->specular[i]);
			}

			shader->setVec3("viewPos", cameraWorldPos);

			//Shadow option
			shader->setFloat("shadowIntensity", qualitySettings.shadowIntensity);
			shader->setInt("shadowSmooth", qualitySettings.shadowSmooth);

			//for each Mesh using this shader
			for (auto const& [meshKey, entitiesArray] : meshMap) {
				Mesh& mesh = GetRenderEngine().meshes[meshKey];
				// @Todo : que faire quand le mesh n'est pas setup sur le GPU ? le setup ou juste ignorer ? afficher une erreur  ?
				if (!mesh.isSetupOnGPU) continue;
				mesh.Bind();

				//for each instance
				size_t numInstances = entitiesArray.size();

				for (size_t i = 0; i < numInstances; i++) {
					ecs::entity_id instance_id = entitiesArray[i];
					ecs::component_id idTransform = transform->getIndex(instance_id);
					ecs::component_id idMesh = meshInstance->getIndex(instance_id);
					Mat4x4 modelMatrix = transform->ComputeModel(idTransform);//@Todo Compute model en dehors du rendu pour toutes les entit�s ?

					Material& material = GetRenderEngine().materials[meshInstance->materialId[idMesh]];
					//set material uniforms
					for (json::iterator it = shader->resource.uniforms.begin(); it != shader->resource.uniforms.end(); ++it) {
						json value = material.uniforms[it.key()];
						const std::string typeV = it.value()["type"];
						if (value.is_null()) value = it.value()["default"];
						std::string index = "material." + it.key();
						if (typeV == "f32") { shader->setFloat(index, value); }
						else if (typeV == "Vec2") { shader->setVec2(index, value[0], value[1]); }
						else if (typeV == "Vec3" || typeV == "Color3") { shader->setVec3(index, value[0], value[1], value[2]); }
						else if (typeV == "Vec4" || typeV == "Color4") { shader->setVec4(index, value[0], value[1], value[2], value[3]); }
					}
					//set Skelletons uniforms if it has one
					//@Todo get skeletonComponent
					shader->setFloat("SkeletonActive", 0.0f);
					if(mesh.HasSkeleton()){
						ecs::component_id skeletonId = skeleton->getIndex(instance_id);
						if (ecs::id::IsValid(skeletonId)) { //@Todo isvalid
							shader->setFloat("SkeletonActive", 1.0f);
							if (!skeleton->idsComputed[skeletonId]) {
								//compute ids
								//parentid
								skeleton->parentId[skeletonId]=transform->FindByName(instance_id,skeleton->parentRelativePath[skeletonId]);
								//bone ids
								for (animation::Bone& bone : skeleton->bones[skeletonId]) {
									bone.id = transform->FindByName(instance_id,bone.relativePath);
								}
								skeleton->idsComputed[skeletonId]= true;
							}

							//get transformation of the entity representing the bone
							Mat4x4 bonesMatrices[NUM_BONES_PER_MESH];
							// @TODO Error on import when bones > NUM_BONES_PER_MESH
							if (skeleton->bones[skeletonId].size() > NUM_BONES_PER_MESH) INTERNAL_ERR("MAX BONES ACHEIVED : ", skeleton->bones[skeletonId].size(), " / ", NUM_BONES_PER_MESH);
							for (size_t j = 0,size = skeleton->bones[skeletonId].size(); j < size;j++) {
								animation::Bone& bone = skeleton->bones[skeletonId][j];
								ecs::component_id idTransform = transform->getIndex(bone.id);
								ecs::component_id parentIdTransform = transform->getIndex(skeleton->parentId[skeletonId]);
								bonesMatrices[j]= glm::inverse(transform->ComputeModel(parentIdTransform)) * transform->ComputeModel(idTransform) * bone.offset; //
							}
							//set bone transformation uniforms
							shader->setMat4("Bones", bonesMatrices[0], NUM_BONES_PER_MESH);
						}
					}

					//get all model matrices and put them in a BufferObject
					//set the Buffer object to instance
					// @Todo make instance rendering if instances >treshold
					shader->setMat4("model", modelMatrix);
					//Render
					glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
				}
			}
		}
	}

	u32 Renderer::BloomEffect(core::rendering::PostProcess& postProcess) {
		bool horizontal = true, first_iteration = true;
		unsigned int amount = postProcess.bloomDiffusion[0] < 1 ? 1 : postProcess.bloomDiffusion[0];
		if (amount > 200) amount = 200;

		float bloomExpansion = postProcess.bloomExpansion[0];
		bloomExpansion = bloomExpansion <= 1 ? 1 : bloomExpansion > 100 ? 100 : bloomExpansion;

		Shader::blurShader.use();

		Shader::blurShader.setFloat("expansion", bloomExpansion);

		for (unsigned int i = 0; i < amount; i++) {
			Renderer::FBBloomPingPong[horizontal].Bind();
			Shader::blurShader.setInt("horizontal", horizontal);

			if(first_iteration) Renderer::FBFeature.BindTexture(1, 0);		// Set Bloom Feature
			else Renderer::FBBloomPingPong[!horizontal].BindTexture(0, 0);	// Set PingPong Bloom

			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); //DRAW QUAD
			horizontal = !horizontal;
			if (first_iteration) first_iteration = false;
		}

		return horizontal ? 0 : 1;
	}
}
