#include "ecs/Scene.h"
#include "ecs/System.h"

namespace illusion::ecs {
	entity_id Scene::CreateEntity() {
		entity_id id = entities.Create({});
		for (auto const& [key, val] : components) {
			val->CreateEntity(id);
		}
		for (auto const& val : systems) {
			val->OnEntityCreate(id);
		}
		return id;
	}
	void Scene::DestroyEntity(entity_id id) {
		for (auto const& val : systems) {
			val->OnEntityDestroy(id);
		}
		for (auto const& [key, val] : components) {
			val->DestroyEntity(id);
		}
		entities.Destroy(id);
	}

	void Scene::Update() {
		for (auto const& val : systems) {
			val->Update();
		}
	}
}