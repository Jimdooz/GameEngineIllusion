#include "core/rendering/Renderer.h"
#include "core/rendering/shapes/defaultShapes.h"
#include "ecs/Scene.h"

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
		scene->renderer->RemoveMeshShader(0, meshId[index], id);
		RemoveData(index, meshId, materialId);
	}

	void MeshInstance::SetMesh(ecs::component_id index, size_t newMeshId) {
		ecs::entity_id id = getId(index);
		scene->renderer->RemoveMeshShader(0, meshId[index], id);
		meshId[index] = newMeshId;
		scene->renderer->AddMeshShader(0, meshId[index], id);
	}

	Renderer::Renderer(ecs::Scene* _scene) :
		scene(_scene),
		camera(scene->GetComponent<ecs::core::Camera>()),
		transform(scene->GetComponent<ecs::core::Transform>()),
		meshInstance(scene->GetComponent<MeshInstance>()),
		instanceRenderingThreshold(3)
	{
		AddMesh(illusion::defaultshape::Cube(), 0);
		AddMesh(illusion::defaultshape::IcoSphere(), 1);
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

		INFO("GET : ", instancesByMeshByShader[idShader][idMesh].size());

		if (instancesByMeshByShader[idShader][idMesh].size() == 0) instancesByMeshByShader[idShader].erase(instancesByMeshByShader[idShader].find(idMesh));

	}
}