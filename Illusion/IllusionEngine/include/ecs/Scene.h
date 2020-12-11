#pragma once

#include "ecs/Entity.h"
#include "ecs/Component.h"

// Core Components
#include "ecs/CoreComponents/Transform.h"

namespace illusion::ecs {

	// On d�clare System sans inclure ses headers par question de double d�pendances
	struct System;

	/**
	 * Scene
	 *
	 * Une sc�ne regroupe tout ce qu'il faut pour la cr�ation d'un niveau dans le jeux
	 * Elle poss�de le System ECS ( Entit�, Component, System ) pour pouvoir donner des comportements aux jeux
	 * Ainsi que des fonctions permettant de rafraichir l'�tat du jeux
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
		 * Permet de cr�er une entit�
		 * @return	l'id de l'entit� g�n�r�
		 */
		entity_id CreateEntity();

		/**
		 * Permet de supprimer une entit�
		 */
		void DestroyEntity(entity_id id);

		/**
		 * >>> Component Part
		 */

		/**
		 * Fonction signalant � la sc�ne qu'il doit utiliser un type de Component donn�
		 */
		template<typename C> void UseComponent() {
			if (ComponentExist<C>()) return;
			components[typeid(C).hash_code()] = new C(this);
		}

		/**
		 * Permet de savoir si la sc�ne inclus d�j� un Component donn�
		 */
		template<typename C> bool ComponentExist() {
			return components.find(typeid(C).hash_code()) != components.end();
		}

		/**
		 * Permet de r�cup�rer un Component
		 */
		template<typename C> Component* GetComponentSystem() {
			return components[typeid(C).hash_code()];
		}

		/**
		 * Permet de r�cup�rer un Component typ�
		 */
		template<typename C> C* GetComponent() {
			return static_cast<C*>(GetComponentSystem<C>());
		}

		/**
		 * Permet de signaler qu'une Entit� va utiliser un Component donn�
		 * @param	id l'id de l'entit�
		 */
		template<typename C> void EntityAddComponent(entity_id id) {
			GetComponentSystem<C>()->UseComponent(id);
			for (auto const& [key, val] : systems) {
				val->OnComponentAdd<C>(id);
			}
		}

		/**
		 * Permet de signaler qu'une Entit� ne va plus utiliser un Component donn�
		 * @param	id l'id de l'entit�
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
		 * Fonction signalant � la sc�ne qu'il doit utiliser un type de System donn�
		 */
		template<typename C> void UseSystem() {
			systems[typeid(C).hash_code()] = new C();
			systems[typeid(C).hash_code()]->Initialize(*this);
		}

		/**
		 * Fonction permettant de r�cup�rer un System typ�
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
