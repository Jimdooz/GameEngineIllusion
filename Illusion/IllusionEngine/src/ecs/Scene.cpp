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
}