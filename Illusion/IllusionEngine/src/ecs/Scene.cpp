#include "ecs/Scene.h"
#include "ecs/System.h"

namespace illusion::ecs {
	entity_id Scene::CreateEntity() {
		entity_id id = entities.Create({});

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

}