#include "core/rendering/Renderer.h"
#include "core/rendering/shapes/defaultShapes.h"
#include "ecs/Scene.h"
#include <glm/gtx/matrix_decompose.hpp>

#include "resources/assets/Shaders.h"
#include "resources/assets/Materials.h"

#include "core/rendering/CoreComponents/pointLight.h"

namespace illusion {
	void MeshInstance::OnEntityDuplicate(ecs::entity_id id){
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
		RemoveData(index, meshId, materialId);
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

	Renderer::Renderer(ecs::Scene* _scene) :
		scene(_scene),
		camera(scene->GetComponent<ecs::core::Camera>()),
		transform(scene->GetComponent<ecs::core::Transform>()),
		meshInstance(scene->GetComponent<MeshInstance>()),
		instanceRenderingThreshold(3)
	{
		//Default Shader
		AddShader(Shader::defaultShader, 0);
		AddMaterial({ 0, 0, "default material", "", "", {
			{ "ambient", {1.0f,1.0f,1.0f,1.0f} },
			{ "diffuse", {1.0f,1.0f,1.0f,1.0f} },
			{ "specular", {1.0f,1.0f,1.0f,1.0f} },
			{ "shininess", 32.0 }
		} }, 0);

		//Default Mesh
		AddMesh(illusion::defaultshape::Cube(), 0);
		AddMesh(illusion::defaultshape::IcoSphere(), 1);

		if (illusion::resources::CurrentProject().path == "") return;

		//All Assets
		auto shaders = illusion::resources::assets::LoadAllShaders();
		auto materials = illusion::resources::assets::LoadAllMaterials();

		for (auto const& shader : shaders) {
			AddShader(Shader(shader), shader.id);
		}

		for (auto const& material : materials) {
			AddMaterial(material, material.id);
		}
	}

	void Renderer::AddMeshMaterial(size_t idMaterial, size_t idMesh, ecs::entity_id entity) {
		if (!ContainsMaterial(idMaterial) || !ContainsMesh(idMesh) || !scene->entities.IsAlive(entity)) return;
		AddMeshShader(materials[idMaterial].shaderId, idMesh, entity);
	}

	void Renderer::RemoveMeshMaterial(size_t idMaterial, size_t idMesh, ecs::entity_id entity) {
		if (!ContainsMaterial(idMaterial) || !ContainsMesh(idMesh)) return;
		RemoveMeshShader(materials[idMaterial].shaderId, idMesh, entity);
	}

	void Renderer::AddMeshShader(size_t idShader, size_t idMesh, ecs::entity_id entity) {
		if (!ContainsShader(idShader) || !ContainsMesh(idMesh) || !scene->entities.IsAlive(entity)) return;

		if (!MeshExistInShader(idShader, idMesh)) {
			//Création de l'espace pour le mesh
			instancesByMeshByShader[idShader][idMesh] = util::Array<ecs::entity_id>();
		}

		instancesByMeshByShader[idShader][idMesh].push_back(entity);
	}
	void Renderer::RemoveMeshShader(size_t idShader, size_t idMesh, ecs::entity_id entity) {
		if (!ContainsShader(idShader) || !ContainsMesh(idMesh)) return;
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
		if (camera->size() < 1) {
			//INTERNAL_ERR("No Camera, the scene can't be rendered");
			return;
		}
		//for each shader
		for (auto const& [shaderKey, meshMap] : instancesByMeshByShader) {
			Shader& shader = shaders[shaderKey];
			shader.use();

			//@Todo change projection only if one of these values are changed
			float aspect = (float)Window::width / (float)Window::height;
			projection = glm::perspective(camera->fov[0], aspect, camera->near[0], camera->far[0]);
			view = glm::lookAt(transform->position[camera->ToEntity[0]], transform->position[camera->ToEntity[0]] + camera->front[0], camera->up[0]);

			Mat4x4 modelCamera = transform->ComputeModel(transform->getIndex(camera->getId(0)));
			Vec3 cameraWorldPos(modelCamera[3][0], modelCamera[3][1], modelCamera[3][2]);

			//set view and projection matrices
			shader.setMat4("view", view);
			shader.setMat4("projection", projection);

			core::rendering::DirectionalLight* lights = scene->GetComponent<core::rendering::DirectionalLight>();
			core::rendering::PointLight* pointLights = scene->GetComponent<core::rendering::PointLight>();

			if (lights->size() > 0) {
				ecs::entity_id idLight = lights->getId(0);

				glm::vec3 scale; glm::quat rotation; glm::vec3 translation;
				glm::vec3 skew; glm::vec4 perspective;
				glm::decompose(transform->ComputeModel(transform->getIndex(idLight)), scale, rotation, translation, skew, perspective);

				shader.setVec3("dirLight.specular", lights->specular[0]);
				shader.setVec3("dirLight.diffuse", lights->diffuse[0]);
				shader.setVec3("dirLight.ambient", lights->ambient[0]);
				shader.setVec3("dirLight.direction", Vec3(glm::toMat4(glm::conjugate(rotation)) * Vec4(0.0,1.0,0.0,1.0)) );
			}
			else {
				shader.setVec3("dirLight.specular", 1.0f, 1.0f, 1.0f);
				shader.setVec3("dirLight.diffuse", 0.5f, 0.5f, 0.5f);
				shader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
				shader.setVec3("dirLight.ambient", 0.2f, 0.2f, 0.2f);
			}

			size_t pointNumbers = pointLights->size();
			shader.setFloat("nbPointLights", pointNumbers);

			for (u32 i = 0; i < pointNumbers; i++) {
				ecs::entity_id idLight = pointLights->getId(i);

				glm::vec3 scale; glm::quat rotation; glm::vec3 translation;
				glm::vec3 skew; glm::vec4 perspective;
				glm::decompose(transform->ComputeModel(transform->getIndex(idLight)), scale, rotation, translation, skew, perspective);

				std::string start = "pointLights[" + std::to_string(i) + "].";
				shader.setVec3(start + "position", translation);
				shader.setFloat(start + "constant", pointLights->constant[i]);
				shader.setFloat(start + "linear", pointLights->linear[i]);
				shader.setFloat(start + "quadratic", pointLights->quadratic[i]);

				shader.setVec3(start + "ambient", pointLights->ambient[i]);
				shader.setVec3(start + "diffuse", pointLights->diffuse[i]);
				shader.setVec3(start + "specular", pointLights->specular[i]);
			}

			shader.setVec3("viewPos", cameraWorldPos);

			//for each Mesh using this shader
			for (auto const& [meshKey, entitiesArray] : meshMap) {
				Mesh& mesh = meshes[meshKey];
				if (!mesh.isSetup) continue;
				mesh.Bind();

				//for each instance
				size_t numInstances = entitiesArray.size();
				for (size_t i = 0; i < numInstances; i++) {
					ecs::entity_id instance_id = entitiesArray[i];
					ecs::component_id idTransform = transform->getIndex(instance_id);
					ecs::component_id idMesh = meshInstance->getIndex(instance_id);
					Mat4x4 modelMatrix = transform->ComputeModel(idTransform);//@Todo Compute model en dehors du rendu pour toutes les entités ?

					// @Todo get Material
					Material& material = materials[meshInstance->materialId[idMesh]];

					for (json::iterator it = shader.resource.uniforms.begin(); it != shader.resource.uniforms.end(); ++it) {
						json value = material.uniforms[it.key()];
						const std::string typeV = it.value()["type"];
						if (value.is_null()) value = it.value()["default"];
						std::string index = "material." + it.key();
						if (typeV == "f32") { shader.setFloat(index, value); }
						else if (typeV == "Vec2") { shader.setVec2(index, value[0], value[1]); }
						else if (typeV == "Vec3" || typeV == "Color3") { shader.setVec3(index, value[0], value[1], value[2]); }
						else if (typeV == "Vec4" || typeV == "Color4") { shader.setVec4(index, value[0], value[1], value[2], value[3]); }
					}

					// @Todo make instance rendering if instances >treshold
					shader.setMat4("model", modelMatrix);
					//Render
					glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
				}
			}
		}
	}
}