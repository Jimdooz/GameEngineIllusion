#pragma once

#include "ecs/Entity.h"

namespace illusion::ecs {
	// On d�clare Scene sans inclure ses headers par question de double d�pendances
	struct Scene;

	// Permet de diff�rencier l'id d'un component d'un autre id ( Voir Documentation Macro )
	DEFINE_TYPED_ID(component_id);

	/**
	 * Component
	 *
	 * Un Component est une structure qui poss�des des donn�es qu'une entity peut poss�der
	 * L'entit� ne poss�de pas les donn�es, mais s�mantiquement les "utilises"
	 *	
	 *	+---------------------------+
	 *  | Donn�es  					|
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

		// Tableau avec pour index l'id des entit�s, et pour valeur la position des donn�es
		// Elle permet de pointer vers la liste des donn�es qui est compact�e
		util::Array<component_id> ToData;

		/**
		 * @brief	Permet d'annoncer qu'une entit� va utiliser le component
		 * 
		 * @param	id l'identifiant de l'entit�
		 */
		void UseComponent(entity_id id);

		/**
		 * @brief Permet d'annoncer qu'une entit� n'utilise plus le component
		 * 
		 * @param	id l'identifiant de l'entit�
		 */
		void RemoveComponent(entity_id id);

		/**
		 * Evenements
		 */

		//Fonction appel� lorsqu'une entit� de la sc�ne a �t� cr��
		//Elle a pour but d'augmenter la taille du tableau des entit�s (ToData) possible
		void OnEntityCreate(entity_id id);

		//Fonction appel� lorsqu'une entit� de la sc�ne a �t� d�truite
		//Elle a pour but de supprimer les donn�es si elles existent de l'entit� et de cr�er un espace vide
		void OnEntityDestroy(entity_id id);

		/**
		 * @brief	Permet de r�cup�rer l'index d'une entit� dans le component
		 * 
		 * @param	id l'identifiant de l'entit�
		 * @return	l'index des datas de l'entit� dans le component
		 */
		inline component_id getIndex(entity_id id) {
			return ToData[id::Index(id)];
		}

	protected:
		/**
		 * @brief	Fonction appel� lors de l'ajout du component � une entit� via UseComponent()
		 *			[!] Doit �tendre les donn�es rajout�es lors de l'h�ritage
		 * @param	id l'identifiant de l'entit�
		 */
		virtual void AddComponentDatas(entity_id id){}

		/**
		 * @brief	Fonction appel� lors de la suppression du component � une entit�.
		 *			-> En g�n�ral appel� lors de la destruction d'une entit�
		 *			-> Ou lors de la suppression manuelle du component via RemoveComponent()
		 *			[!] Doit supprimer les donn�es rajout�es lors de l'h�ritage
		 * @param	id l'identifiant de l'entit�
		 */
		virtual void RemoveComponentDatas(component_id index, entity_id id){}

		/**
		 * Wrapper pour ajouter une donn�e � un tableau de donn�es ( rajout du component � une entit� )
		 */
		template<typename T> inline void AddComponentData(illusion::util::Array<T>& array, T data) {
			array.push_back(data);
		}

		/**
		 * Wrapper pour supprimer une donn�e � un tableau de donn�es depuis un index ( ducoup une entit� )
		 */
		template<typename T> inline void RemoveComponentData(illusion::util::Array<T>& array, component_id index) {
			util::EraseUnordered(array, index);
		}
	};
}