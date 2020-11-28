#pragma once

#include "ecs/Entity.h"
#include "ecs/Component.h"

namespace illusion::ecs {

	struct System;

	struct Scene {
		Entities entities;
		util::Map<size_t, Component*> components;
		util::Map<size_t, System*> systems;

		entity_id CreateEntity();
		void DestroyEntity(entity_id id);

		void Update();

		/**
		 * System Part
		 */
		template<typename C> void AddSystem() {
			systems[typeid(C).hash_code()] = new C();
			systems[typeid(C).hash_code()]->Initialize(*this);
		}
		template<typename C> C& GetSystem() {
			return static_cast<C&>(*systems[typeid(C).hash_code()]);
		}

		/**
		 * Component Part
		 */
		template<typename C> void AddComponent() {
			components[typeid(C).hash_code()] = new C();
		}
		template<typename C> bool ComponentExist() {
			return components.find(typeid(C).hash_code()) != components.end();
		}
		template<typename C> Component* GetComponentSystem() {
			return components[typeid(C).hash_code()];
		}
		template<typename C> C* GetComponent() {
			return static_cast<C*>(GetComponentSystem<C>());
		}

		template<typename C> void AddComponentEntity(entity_id id) {
			GetComponentSystem<C>()->AddComponent(id);
			for (auto const& [key, val] : systems) {
				val->OnComponentAdd<C>(id);
			}
		}
		template<typename C> void RemoveComponentEntity(entity_id id) {
			GetComponentSystem<C>()->RemoveComponent(id);
			for (auto const& [key, val] : systems) {
				val->OnComponentRemove<C>(id);
			}
		}
	};
}
