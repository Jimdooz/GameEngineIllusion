#pragma once

#include "ecs/Entity.h"
#include "ecs/Component.h"

// Core Components
#include "ecs/CoreComponents/Transform.h"

namespace illusion::ecs {

	// On déclare System sans inclure ses headers par question de double dépendances
	struct System;

	/**
	 * Scene
	 *
	 * Une scène regroupe tout ce qu'il faut pour la création d'un niveau dans le jeux
	 * Elle possède le System ECS ( Entité, Component, System ) pour pouvoir donner des comportements aux jeux
	 * Ainsi que des fonctions permettant de rafraichir l'état du jeux
	 * 
	 * +----------------+
	 * | ECS			|
	 * +----------------+
	 */
	struct Scene {

		Scene() {
			this->UseComponent<core::Transform>();
		}

		// ECS System
		Entities entities;
		util::Map<size_t, Component*> components;
		util::Map<size_t, System*> systems;

		/**
		 * >>> Entity Part
		 */

		/**
		 * Permet de créer une entité
		 * @return	l'id de l'entité généré
		 */
		entity_id CreateEntity();

		/**
		 * Permet de supprimer une entité
		 */
		void DestroyEntity(entity_id id);

		/**
		 * >>> Component Part
		 */

		/**
		 * Fonction signalant à la scène qu'il doit utiliser un type de Component donné
		 */
		template<typename C> void UseComponent() {
			if (ComponentExist<C>()) return;
			components[typeid(C).hash_code()] = new C(this);
		}

		/**
		 * Permet de savoir si la scène inclus déjà un Component donné
		 */
		template<typename C> bool ComponentExist() {
			return components.find(typeid(C).hash_code()) != components.end();
		}

		/**
		 * Permet de récupérer un Component
		 */
		template<typename C> Component* GetComponentSystem() {
			return components[typeid(C).hash_code()];
		}

		/**
		 * Permet de récupérer un Component typé
		 */
		template<typename C> C* GetComponent() {
			return static_cast<C*>(GetComponentSystem<C>());
		}

		/**
		 * Permet de signaler qu'une Entité va utiliser un Component donné
		 * @param	id l'id de l'entité
		 */
		template<typename C> void EntityAddComponent(entity_id id) {
			GetComponentSystem<C>()->UseComponent(id);
			for (auto const& [key, val] : systems) {
				val->OnComponentAdd<C>(id);
			}
		}

		/**
		 * Permet de signaler qu'une Entité ne va plus utiliser un Component donné
		 * @param	id l'id de l'entité
		 */
		template<typename C> void EntityRemoveComponent(entity_id id) {
			GetComponentSystem<C>()->RemoveComponent(id);
			for (auto const& [key, val] : systems) {
				val->OnComponentRemove<C>(id);
			}
		}

		/**
		 * >>> System Part
		 */

		/**
		 * Fonction signalant à la scène qu'il doit utiliser un type de System donné
		 */
		template<typename C> void UseSystem() {
			systems[typeid(C).hash_code()] = new C();
			systems[typeid(C).hash_code()]->Initialize(*this);
		}

		/**
		 * Fonction permettant de récupérer un System typé
		 */
		template<typename C> C& GetSystem() {
			return static_cast<C&>(*systems[typeid(C).hash_code()]);
		}

		/**
		 * >>> Behaviour Part
		 */
		void Update();
	};
}
