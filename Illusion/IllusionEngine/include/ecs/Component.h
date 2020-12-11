#pragma once

#include "ecs/Entity.h"

namespace illusion::ecs {
	// On déclare Scene sans inclure ses headers par question de double dépendances
	struct Scene;

	// Permet de différencier l'id d'un component d'un autre id ( Voir Documentation Macro )
	DEFINE_TYPED_ID(component_id);

	/**
	 * Component
	 *
	 * Un Component est une structure qui possèdes des données qu'une entity peut posséder
	 * L'entité ne possède pas les données, mais sémantiquement les "utilises"
	 *	
	 *	+---------------------------+
	 *  | Données  					|
	 *	| ( type util::Array )		|
	 *	|							|
	 *	+--------------------[CUSTOM]
	 *	+---------------------------+
	 *	| ToEntity					|
	 *	| ToData					|
	 *	+---------------------[FIXED]
	 */
	struct Component {
		Component(Scene* scene);

		Scene *scene;

		// Tableau avec pour index l'id des components, et pour valeur les entity id
		util::Array<entity_id> ToEntity;

		// Tableau avec pour index l'id des entités, et pour valeur la position des données
		// Elle permet de pointer vers la liste des données qui est compactée
		util::Array<component_id> ToData;

		/**
		 * @brief	Permet d'annoncer qu'une entité va utiliser le component
		 * 
		 * @param	id l'identifiant de l'entité
		 */
		void UseComponent(entity_id id);

		/**
		 * @brief Permet d'annoncer qu'une entité n'utilise plus le component
		 * 
		 * @param	id l'identifiant de l'entité
		 */
		void RemoveComponent(entity_id id);

		/**
		 * Evenements
		 */

		//Fonction appelé lorsqu'une entité de la scène a été créé
		//Elle a pour but d'augmenter la taille du tableau des entités (ToData) possible
		void OnEntityCreate(entity_id id);

		//Fonction appelé lorsqu'une entité de la scène a été détruite
		//Elle a pour but de supprimer les données si elles existent de l'entité et de créer un espace vide
		void OnEntityDestroy(entity_id id);

		/**
		 * @brief	Permet de récupérer l'index d'une entité dans le component
		 * 
		 * @param	id l'identifiant de l'entité
		 * @return	l'index des datas de l'entité dans le component
		 */
		inline component_id getIndex(entity_id id) {
			return ToData[id::Index(id)];
		}

	protected:
		/**
		 * @brief	Fonction appelé lors de l'ajout du component à une entité via UseComponent()
		 *			[!] Doit étendre les données rajoutées lors de l'héritage
		 * @param	id l'identifiant de l'entité
		 */
		virtual void AddComponentDatas(entity_id id){}

		/**
		 * @brief	Fonction appelé lors de la suppression du component à une entité.
		 *			-> En général appelé lors de la destruction d'une entité
		 *			-> Ou lors de la suppression manuelle du component via RemoveComponent()
		 *			[!] Doit supprimer les données rajoutées lors de l'héritage
		 * @param	id l'identifiant de l'entité
		 */
		virtual void RemoveComponentDatas(component_id index, entity_id id){}

		/**
		 * Wrapper pour ajouter une donnée à un tableau de données ( rajout du component à une entité )
		 */
		template<typename T> inline void AddComponentData(illusion::util::Array<T>& array, T data) {
			array.push_back(data);
		}

		/**
		 * Wrapper pour supprimer une donnée à un tableau de données depuis un index ( ducoup une entité )
		 */
		template<typename T> inline void RemoveComponentData(illusion::util::Array<T>& array, component_id index) {
			util::EraseUnordered(array, index);
		}
	};
}