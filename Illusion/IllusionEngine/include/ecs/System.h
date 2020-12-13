#pragma once

#include "ecs/Entity.h"
#include "ecs/Component.h"

/**
 * Macro permettant de décorer la fonction Update à override
 * Il permet d'automatiser la boucle sur toutes les entités ciblé par le System
 * Il est conseillé d'utiliser la macro SYSTEM_USE_DATA pour automatiser la récupération de datas des components
 */
#define SYSTEM_UPDATE_LOOP(BEHAVIOUR) \
	virtual void Update() override { \
	u32 size = ToEntity.size(); \
	for (currIndex = 0; currIndex < size; currIndex++) { BEHAVIOUR }}

 /**
  * Macro permettant d'automatiser la déclaration de fonctions inline pour obtenir des données de components
  * Cette macro pourra être utilisé pour permettre d'obtenir facilement les donnée de l'entité actuelle dans les différentes boucles [Update, FixedUpdate, LateUpdate]
  *
  * - NAME : le nom de la fonction à appeler
  * - COMPONENT : le [Component] ciblé (doit appartenir aux dépendances du System)
  * - DATA : la donnée à récupérer, par exemple la [position] d'un [Component] transform
  * - TYPE : le type de la donnée, par exemple pour [position] -> [Vec3]
  */
#define SYSTEM_USE_DATA(NAME, COMPONENT, DATA, TYPE) \
	inline TYPE& NAME() {		\
		return COMPONENT->DATA[COMPONENT->getIndex(id())]; \
	}

#define SYSTEM_NAME(NAME)\
	inline static const std::string SNAME = ##NAME;\
	virtual std::string getName() { return SNAME; }

namespace illusion::ecs {

	// On déclare Scene sans inclure ses headers par question de double dépendances
	struct Scene;

	/**
	 * System
	 *
	 * Un Système est une structure qui possèdes les mechaniques et comportements
	 * L'entité en soit ne fait rien, c'est les Systèmes qui auront pour but de modifier les données des components
	 * des entités ciblés
	 *
	 *	+---------------------------+
	 *	| ToEntity ( compact )		|
	 *	| ^	   v					|
	 *	| ToData ( non-compact )	|
	 *	+---------------------[FIXED]
	 */
	struct System {
		SYSTEM_NAME("DEFAULT SYSTEM")

		virtual void Update();
		virtual void LateUpdate();
		virtual void FixedUpdate();

		util::Array<entity_id> ToEntity;		// Liste compacte des entités ciblés par le System
		util::Array<component_id> ToData;		// Liste non compacte de pointeurs vers les entités ciblés par le System ( id::invalid si non )
		util::Array<Component*> componentsDeps;	// Liste des Components dont dépend le System

		inline entity_id id() {
			return ToEntity[currIndex];
		}

		// Lors de l'itération d'un [Update, LateUpdate, FixedUpdate] c'est cette variable qui sera incrémenté
		// Elle permet de faciliter la récupération des données des components lors des différentes boucles
		// Voir Macro SYSTEM_USE_DATA
		u32 currIndex = 0;
		
		/**
		 * Evénement appelé lorsqu'une entité à été créé par la scène
		 * @param	id l'id de l'entité
		 */
		void OnEntityCreate(entity_id id);

		/**
		 * Evénement appelé lorsqu'une entité à été détruite par la scène
		 * @param	id l'id de l'entité
		 */
		void OnEntityDestroy(entity_id id);

		/**
		 * Evénement appelé lorsqu'une entité a ajouté un nouveau component
		 * Elle permet d'ajouter l'entité au System si celle-ci corresponds au critères
		 * @param	id l'id de l'entité
		 */
		inline void OnComponentAdd(entity_id id) {
			if (!AcceptedEntity(id) && CanAcceptEntity(id)) {
				ToData[id::Index(id)] = component_id{ static_cast<u32>(ToEntity.size()) };
				AddData(ToEntity, id);
			}
		}

		/**
		 * Evénement appelé lorsqu'une entité a supprimé un component qu'elle posssédais
		 * Elle permet de supprimer l'entité au System si celle-ci ne corresponds plus au critères
		 * @param	id l'id de l'entité
		 */
		inline void OnComponentRemove(entity_id id) {
			if (AcceptedEntity(id) && !CanAcceptEntity(id)) {
				RemoveEntity(id);
			}
		}

		/**
		 * Fonction permettant de savoir si une entité est possédé par le System
		 * @param	id l'id de l'entité
		 * @return	vrai si l'entité est possédé par le System, faux sinon
		 */
		inline bool AcceptedEntity(ecs::entity_id id) {
			return ToData[id::Index(id)] != id::invalid_id;
		}

		/**
		 * Fonction d'Initialisation du System selon la scène qui le possède
		 * Celle-ci aura pour but de récupérer les components dont le System dépends provenant de la scène via scene.getComponent<Component>()
		 * Et d'ajouter ces même components au dépendances de celui-ci via la méthode SetDependencies()
		 * @param	scene la scène possédant le System
		 */
		virtual void Initialize(Scene& scene);

	protected:
		/**
		 * Permet de supprimer l'entité du System
		 * @param	id l'id de l'entité
		 */
		void RemoveEntity(entity_id id);

		/**
		 * Wrapper pour supprimer d'un Array l'élement à la position index
		 */
		template<typename T> inline void RemoveData(illusion::util::Array<T>& array, entity_id index) {
			util::EraseUnordered(array, index);
		}

		/**
		 * Wrapper pour ajouter un element à un Array donné
		 */
		template<typename T> inline void AddData(illusion::util::Array<T>& array, T data) {
			array.push_back(data);
		}

		/**
		 * Fonction pour savoir si une entité est conforme aux dépendances du System
		 * Pour qu'une entité soit conforme elle doit utiliser tous les components dont le System dépend
		 * @param	id l'id de l'entité
		 * @return	vrai si l'entité est acceptable, faux sinon
		 */
		inline bool CanAcceptEntity(ecs::entity_id id) {
			for (auto const& val : componentsDeps) {
				if (val->getIndex(id) == ecs::id::invalid_id) return false;
			}
			return true;
		}

		/**
		 * Fonction permettant d'ajouter les Components dont dépend le System
		 * Cette fonction doit être appelé lors de la fonction d'Initialisation du System
		 */
		constexpr void SetDependencies() {}
		template<typename Arg1, class... Args> inline void SetDependencies(Arg1 c1, Args... cNext) {
			componentsDeps.push_back(c1);
			SetDependencies(cNext...);
		}
	};
}