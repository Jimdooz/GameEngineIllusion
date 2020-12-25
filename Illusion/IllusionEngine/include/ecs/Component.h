#pragma once

#include "ecs/Entity.h"

/**
* Macro permettant d'abstraire la cr�ation d'un type de donn�e d'un component
*/
#define COMPONENT_DATA(TYPE, NAME) \
	util::Array<TYPE> NAME

#define COMPONENT_REGISTER(CLASS) \
	virtual Component* generate(illusion::ecs::Scene* scene) override { return new CLASS(scene); }

#define COMPONENT_NAME(NAME)\
	inline static const std::string CNAME = NAME;\
	virtual std::string getName() { return CNAME; }

#define COMPONENT_PUBLIC(NAME) \
	this->publicDatas.push_back(illusion::ecs::PublicComponentDatas::GenerateData(this->NAME, #NAME));

#define COMPONENT_PROTECTED(NAME) \
	this->publicDatas.push_back(illusion::ecs::PublicComponentDatas::GenerateData(this->NAME, #NAME, false));

namespace illusion::ecs {
	// On dיclare Scene sans inclure ses headers par question de double dיpendances
	struct Scene;

	// Permet de diffיrencier l'id d'un component d'un autre id ( Voir Documentation Macro )
	DEFINE_TYPED_ID(component_id);

	struct PublicComponentDatas {
		template<typename T> static PublicComponentDatas GenerateData(T& data, std::string name, bool visible = true) {
			PublicComponentDatas newData = { (void*)&data, typeid(T).hash_code(), name, visible };
			return newData;
		}

		void* data;
		size_t type;
		std::string name;
		bool visible;
	};

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

		COMPONENT_NAME("DEFAULT COMPONENT");
		virtual Component* generate(Scene* scene) { return new Component(scene); }

		// Tableau avec pour index l'id des components, et pour valeur les entity id
		util::Array<entity_id> ToEntity;

		// Tableau avec pour index l'id des entitיs, et pour valeur la position des donnיes
		// Elle permet de pointer vers la liste des donnיes qui est compactיe
		util::Array<component_id> ToData;

		// Tableau des variables publiques du component, elle permet principalement d'afficher des �l�ments d'UI
		util::Array<PublicComponentDatas> publicDatas;

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
		 * @brief Fonction appel� apr�s RemoveComponent
		 *
		 * @param	id l'identifiant de l'entit�
		 */
		virtual void AfterRemoveComponent(entity_id id);

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
		virtual void AddDatas(entity_id id){}

		/**
		 * @brief	Fonction appelי lors de la suppression du component א une entitי.
		 *			-> En gיnיral appelי lors de la destruction d'une entitי
		 *			-> Ou lors de la suppression manuelle du component via RemoveComponent()
		 *			[!] Doit supprimer les donnיes rajoutיes lors de l'hיritage
		 * @param	id l'identifiant de l'entitי
		 */
		virtual void RemoveDatas(component_id index, entity_id id){}

		/**
		 * Wrapper pour ajouter une donnיe א un tableau de donnיes ( rajout du component א une entitי )
		 */
		template<typename T> inline void AddData(illusion::util::Array<T>& array, T data) {
			array.push_back(data);
		}

		/**
		 * Wrapper pour supprimer une donnיe א un tableau de donnיes depuis un index ( ducoup une entitי )
		 */
		template<typename T> inline void RemoveData(illusion::util::Array<T>& array, component_id index) {
			util::EraseUnordered(array, index);
		}

	public:

		static util::Map<size_t, Component*> AllComponents;

		static void AppendCoreComponents();

		template<typename T>static void AppendComponents() {
			Scene fakeScene;
			Component::AllComponents[typeid(T).hash_code()] = new T(&fakeScene);
		}
	};

}
