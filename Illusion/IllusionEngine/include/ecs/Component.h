#pragma once

#include "ecs/Entity.h"

namespace illusion::ecs {
	// On dיclare Scene sans inclure ses headers par question de double dיpendances
	struct Scene;

	// Permet de diffיrencier l'id d'un component d'un autre id ( Voir Documentation Macro )
	DEFINE_TYPED_ID(component_id);

	/**
	 * Component
	 *
	 * Un Component est une structure qui possטdes des donnיes qu'une entity peut possיder
	 * L'entitי ne possטde pas les donnיes, mais sיmantiquement les "utilises"
	 *	
	 *	+---------------------------+
	 *  | Donnיes  					|
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

		// Tableau avec pour index l'id des entitיs, et pour valeur la position des donnיes
		// Elle permet de pointer vers la liste des donnיes qui est compactיe
		util::Array<component_id> ToData;

		/**
		 * @brief	Permet d'annoncer qu'une entitי va utiliser le component
		 * 
		 * @param	id l'identifiant de l'entitי
		 */
		void UseComponent(entity_id id);

		/**
		 * @brief Permet d'annoncer qu'une entitי n'utilise plus le component
		 * 
		 * @param	id l'identifiant de l'entitי
		 */
		void RemoveComponent(entity_id id);

		/**
		 * Evenements
		 */

		//Fonction appelי lorsqu'une entitי de la scטne a יtי crיי
		//Elle a pour but d'augmenter la taille du tableau des entitיs (ToData) possible
		void OnEntityCreate(entity_id id);

		//Fonction appelי lorsqu'une entitי de la scטne a יtי dיtruite
		//Elle a pour but de supprimer les donnיes si elles existent de l'entitי et de crיer un espace vide
		void OnEntityDestroy(entity_id id);

		/**
		 * @brief	Permet de rיcupיrer l'index d'une entitי dans le component
		 * 
		 * @param	id l'identifiant de l'entitי
		 * @return	l'index des datas de l'entitי dans le component
		 */
		inline component_id getIndex(entity_id id) {
			return ToData[id::Index(id)];
		}

	protected:
		/**
		 * @brief	Fonction appelי lors de l'ajout du component א une entitי via UseComponent()
		 *			[!] Doit יtendre les donnיes rajoutיes lors de l'hיritage
		 * @param	id l'identifiant de l'entitי
		 */
		virtual void AddComponentDatas(entity_id id){}

		/**
		 * @brief	Fonction appelי lors de la suppression du component א une entitי.
		 *			-> En gיnיral appelי lors de la destruction d'une entitי
		 *			-> Ou lors de la suppression manuelle du component via RemoveComponent()
		 *			[!] Doit supprimer les donnיes rajoutיes lors de l'hיritage
		 * @param	id l'identifiant de l'entitי
		 */
		virtual void RemoveComponentDatas(component_id index, entity_id id){}

		/**
		 * Wrapper pour ajouter une donnיe א un tableau de donnיes ( rajout du component א une entitי )
		 */
		template<typename T> inline void AddComponentData(illusion::util::Array<T>& array, T data) {
			array.push_back(data);
		}

		/**
		 * Wrapper pour supprimer une donnיe א un tableau de donnיes depuis un index ( ducoup une entitי )
		 */
		template<typename T> inline void RemoveComponentData(illusion::util::Array<T>& array, component_id index) {
			util::EraseUnordered(array, index);
		}
	};
}