#include "ecs/Scene.h"
#include "ecs/System.h"

namespace illusion::ecs {

	void Scene::Reset() {
		for (u32 i = 0; i < entities.m_entities.size(); i++) {
			if (entities.IsAlive(entities.GetId(i))) {
				DestroyEntity(entities.GetId(i));
			}
		}
		entities.m_entities.clear();
		entities.m_freeIds.clear();

		components.clear();
		systems.clear();
	}

	entity_id Scene::CreateEntity() {
		entity_id id = entities.Create();

		for (auto const& [key, val] : components) {
			val->OnEntityCreate(id);
		}
		for (auto const& [key, val] : systems) {
			val->OnEntityCreate(id);
		}

		// Add Default Core Components
		EntityAddComponent<core::Transform>(id);

		return id;
	}
	entity_id Scene::CreateEntity(u32 index) {
		entity_id id = entities.Create(index);

		for (auto const& [key, val] : components) {
			val->OnEntityCreate(id);
		}
		for (auto const& [key, val] : systems) {
			val->OnEntityCreate(id);
		}

		// Add Default Core Components
		EntityAddComponent<core::Transform>(id);

		return id;
	}
	void Scene::DestroyEntity(entity_id id) {
		for (auto const& [key, val] : systems) {
			val->OnEntityDestroy(id);
		}
		for (auto const& [key, val] : components) {
			val->OnEntityDestroy(id);
		}
		entities.Destroy(id);
	}

	void Scene::Update() {
		for (auto const& [key, val] : systems) {
			val->Update();
		}
	}
	void Scene::EntityAddComponent(entity_id id, size_t componentHash) {
		if (!entities.IsAlive(id)) return;
		components[componentHash]->UseComponent(id);
		for (auto const& [key, val] : systems) {
			val->OnComponentAdd(id);
		}
	}

	void Scene::EntityRemoveComponent(entity_id id, size_t componentHash) {
		if (!entities.IsAlive(id)) return;
		components[componentHash]->RemoveComponent(id);
		for (auto const& [key, val] : systems) {
			val->OnComponentRemove(id);
		}
	}

	void Scene::UseSystem(size_t systemHash) {
		systems[systemHash] = System::AllSystems[systemHash]->generate();
		systems[systemHash]->Initialize(*this);
	}
}