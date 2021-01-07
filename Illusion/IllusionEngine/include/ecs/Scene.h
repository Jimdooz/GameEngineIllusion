#pragma once

#include "ecs/Entity.h"
#include "ecs/Component.h"
#include "ecs/System.h"

// Core Components
#include "ecs/CoreComponents/Transform.h"

#include "core/rendering/Renderer.h"

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
			for (auto const& [key, val] : Component::AllComponents) this->UseComponent(key);
			renderer = new Renderer(this);
			renderer->AddShader(Shader::defaultShader, 0);
		}

		~Scene() {
			delete renderer;
		}

		// ECS System
		Entities entities;

		// @Ask : peut être directement stoquer des Components ou System à la place de pointeurs ?
		// @Todo : mettre un pointeur direct vers les core Components comme Camera et Transform
		util::Map<size_t, Component*> components;
		util::Map<size_t, System*> systems;
		bool pause = false;

		Renderer *renderer;

		/**
		 * Permet de réinitialiser une scène
		 */
		void Reset();

		void ReloadRenderer() {
			if(renderer != nullptr) delete renderer;
			renderer = new Renderer(this);
			renderer->AddShader(Shader::defaultShader, 0);
		}

		/**
		 * >>> Entity Part
		 */

		/**
		 * Permet de créer une entité
		 * @return	l'id de l'entité généré
		 */
		entity_id CreateEntity();
		entity_id CreateEntity(u32 index);

		/**
		 * Permet de supprimer une entité
		 */
		void DestroyEntity(entity_id id);

		/**
		 * >>> Component Part
		 */

		// @Ask : peut être assert si le type C hérite bien de Component en Debug ?
		// car rien n'oblige avec le C++ il me semble
		/**
		 * Fonction signalant à la scène qu'il doit utiliser un type de Component donné
		 */
		template<typename C> void UseComponent() {
			if (ComponentExist<C>()) return;
			components[typeid(C).hash_code()] = new C(this);
		}

		void UseComponent(size_t componentHash) {
			if (ComponentExist(componentHash)) return;
			components[componentHash] = Component::AllComponents[componentHash]->generate(this);
		}

		/**
		 * Permet de savoir si la scène inclus déjà un Component donné
		 */
		template<typename C> bool ComponentExist() {
			return ComponentExist(typeid(C).hash_code());
		}

		bool ComponentExist(size_t hash) {
			return components.find(hash) != components.end();
		}

		/**
		 * Permet de récupérer un Component
		 */
		template<typename C> Component* GetComponentSystem() {
			//@Todo : Créer un système pour checker si le component existe en debug
			return components[typeid(C).hash_code()];
		}

		Component* GetComponentSystem(size_t hash) {
			return components[hash];
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
			return EntityAddComponent(id, typeid(C).hash_code());
		}

		/**
		 * Permet de signaler qu'une Entit� va utiliser un Component donn�
		 * @param	id l'id de l'entit�
		 * @param	componentHash le hash du component
		 */
		void EntityAddComponent(entity_id id, size_t componentHash);

		/**
		 * Permet d'annoncer l'ajout d'un component de la part de l'inspecteur au component
		 */
		void EmitComponentAdded(entity_id id, size_t componentHash);

		/**
		 * Permet de signaler qu'une Entit� ne va plus utiliser un Component donn�
		 * @param	id l'id de l'entit�
		 */
		template<typename C> void EntityRemoveComponent(entity_id id) {
			return EntityRemoveComponent(id, typeid(C).hash_code());
		}

		/**
		 * Permet de signaler qu'une Entit� ne va plus utiliser un Component donn�
		 * @param	id l'id de l'entit�
		 * @param	componentHash le hash du component
		 */
		void EntityRemoveComponent(entity_id id, size_t componentHash);

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

		void UseSystem(size_t systemHash);

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
