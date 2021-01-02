#pragma once

#include "ecs/Entity.h"

/**
* Macro permettant d'abstraire la création d'un type de donnée d'un component
*/
#define COMPONENT_DATA(TYPE, NAME) \
	util::Array<TYPE> NAME

//Permet de générer un component
#define COMPONENT_REGISTER(CLASS) \
	virtual Component* generate(illusion::ecs::Scene* scene) override { return new CLASS(scene); }

//Permet de définir le nom d'un component
#define COMPONENT_NAME(NAME)\
	inline static const std::string CNAME = NAME;\
	virtual std::string getName() { return CNAME; }

//Permet de définir une donnée du component visible dans l'editeur et sauvegardable
#define COMPONENT_PUBLIC(NAME) \
	this->publicDatas.push_back(illusion::ecs::PublicComponentDatas::GenerateData(this->NAME, #NAME));

//Permet de définir une donnée du component non visible dans l'editeur et sauvegardable
#define COMPONENT_PROTECTED(NAME) \
	this->publicDatas.push_back(illusion::ecs::PublicComponentDatas::GenerateData(this->NAME, #NAME, false));

namespace illusion::ecs {
	// On déclare Scene sans inclure ses headers par question de double dépendances
	struct Scene;

	// Permet de différencier l'id d'un component d'un autre id ( Voir Documentation Macro )
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
	 * Un Component est une structure qui possטdes des données qu'une entity peut posséder
	 * L'entité ne possטde pas les données, mais sémantiquement les "utilises"
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

		COMPONENT_NAME("DEFAULT COMPONENT");
		virtual Component* generate(Scene* scene) { return new Component(scene); }

		// Tableau avec pour index l'id des components, et pour valeur les entity id
		util::Array<entity_id> ToEntity;

		// Tableau avec pour index l'id des entités, et pour valeur la position des données
		// Elle permet de pointer vers la liste des données qui est compactée
		util::Array<component_id> ToData;

		// Tableau des variables publiques du component, elle permet principalement d'afficher des �l�ments d'UI
		util::Array<PublicComponentDatas> publicDatas;

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
		 * @brief Fonction appel� apr�s RemoveComponent
		 *
		 * @param	id l'identifiant de l'entit�
		 */
		virtual void AfterRemoveComponent(entity_id id);

		/**
		 * Evenements
		 */

		//Fonction appelé lorsqu'une entité de la scטne a été créé
		//Elle a pour but d'augmenter la taille du tableau des entités (ToData) possible
		void OnEntityCreate(entity_id id);

		//Fonction appelé lorsqu'une entité de la scטne a été détruite
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

		/**
		 * @brief	Permet de récupérer l'id d'index donné
		 *
		 * @param	id l'identifiant de l'entité
		 * @return	l'index des datas de l'entité dans le component
		 */
		inline entity_id getId(id::id_type index) {
			return ToEntity[index];
		}

		/**
		 * @brief	Permet de savoir si un id a un component associé
		 *
		 * @param	id l'identifiant de l'entité
		 * @return	l'index des datas de l'entité dans le component
		 */
		inline bool exist(entity_id id) {
			return id::IsValid(ToData[id::Index(id)]);
		}

		/**
		 * @brief	Permet de connaître le nombre d'entité qui ont le component
		 *
		 * @return	le nombre d'éléments que compose le component
		 */
		inline i32 size() {
			return ToEntity.size();
		}

	protected:
		/**
		 * @brief	Fonction appelé lors de l'ajout du component à une entité via UseComponent()
		 *			[!] Doit étendre les données rajoutées lors de l'héritage
		 * @param	id l'identifiant de l'entité
		 */
		virtual void AddDatas(entity_id id){}

		/**
		 * @brief	Fonction appelé lors de la suppression du component à une entité.
		 *			-> En général appelé lors de la destruction d'une entité
		 *			-> Ou lors de la suppression manuelle du component via RemoveComponent()
		 *			[!] Doit supprimer les données rajoutées lors de l'héritage
		 * @param	id l'identifiant de l'entité
		 */
		virtual void RemoveDatas(component_id index, entity_id id){}

		/**
		 * Wrapper pour ajouter une donnée à un tableau de données ( rajout du component à une entité )
		 */
		template<typename T> inline void AddData(illusion::util::Array<T>& array, T data) {
			array.push_back(data);
		}

		/**
		 * Wrapper pour supprimer une donnée à un tableau de données depuis un index ( ducoup une entité )
		 */
		template<typename T> inline void RemoveData(illusion::util::Array<T>& array, component_id index) {
			util::EraseUnordered(array, index);
		}

		/**
		 * Fonction permettant d'ajouter les Components dont dépend le System
		 * Cette fonction doit être appelé lors de la fonction d'Initialisation du System
		 */
		constexpr void RemoveData(component_id index) {}
		template<typename Arg1, class... Args> inline void RemoveData(component_id index, illusion::util::Array<Arg1>& c1, Args... cNext) {
			util::EraseUnordered(c1, index);
			RemoveData(index, cNext...);
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
