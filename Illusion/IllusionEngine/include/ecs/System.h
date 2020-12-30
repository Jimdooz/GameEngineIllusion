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
  * Macro permettant d'automatiser la d�claration de fonctions inline pour obtenir des donn�es de components
  * Cette macro pourra �tre utilis� pour permettre d'obtenir facilement les donn�e de l'entit� actuelle dans les diff�rentes boucles [Update, FixedUpdate, LateUpdate]
  *
  * - NAME : le nom de la fonction � appeler
  * - COMPONENT : le [Component] cibl� (doit appartenir aux d�pendances du System)
  * - DATA : la donn�e � r�cup�rer, par exemple la [position] d'un [Component] transform
  * - TYPE : le type de la donn�e, par exemple pour [position] -> [Vec3]
  */
#define SYSTEM_USE_DATA(NAME, COMPONENT, DATA, TYPE) \
	inline TYPE& NAME() {		\
		return COMPONENT->DATA[COMPONENT->getIndex(id())]; \
	}

#define SYSTEM_NAME(NAME)\
	inline static const std::string SNAME = NAME;\
	virtual std::string getName() { return SNAME; }

#define SYSTEM_REGISTER(CLASS) \
	virtual System* generate() override { return new CLASS(); }

namespace illusion::ecs {

	// On d�clare Scene sans inclure ses headers par question de double d�pendances
	struct Scene;

	/**
	 * System
	 *
	 * Un Système est une structure qui poss�des les mechaniques et comportements
	 * L'entit� en soit ne fait rien, c'est les Syst�mes qui auront pour but de modifier les donn�es des components
	 * des entit�s cibl�s
	 *
	 *	+---------------------------+
	 *	| ToEntity ( compact )		|
	 *	| ^	   v					|
	 *	| ToData ( non-compact )	|
	 *	+---------------------[FIXED]
	 */
	struct System {
		SYSTEM_NAME("DEFAULT SYSTEM")
		virtual System* generate() { return new System(); }

		virtual void Update();
		virtual void LateUpdate();
		virtual void FixedUpdate();

		util::Array<entity_id> ToEntity;		// Liste compacte des entit�s cibl�s par le System
		util::Array<component_id> ToData;		// Liste non compacte de pointeurs vers les entit�s cibl�s par le System ( id::invalid si non )
		util::Array<Component*> componentsDeps;	// Liste des Components dont d�pend le System

		// Permet de r�cup�rer l'id de l'entit� actuellement dans la boucle
		inline entity_id id() {
			return ToEntity[currIndex];
		}

		// Lors de l'it�ration d'un [Update, LateUpdate, FixedUpdate] c'est cette variable qui sera incr�ment�
		// Elle permet de faciliter la r�cup�ration des donn�es des components lors des diff�rentes boucles
		// Voir Macro SYSTEM_USE_DATA
		u32 currIndex = 0;
		
		/**
		 * Ev�nement appel� lorsqu'une entit� � �t� cr�� par la sc�ne
		 * @param	id l'id de l'entit�
		 */
		void OnEntityCreate(entity_id id);

		/**
		 * Ev�nement appel� lorsqu'une entit� � �t� d�truite par la sc�ne
		 * @param	id l'id de l'entit�
		 */
		void OnEntityDestroy(entity_id id);

		/**
		 * Ev�nement appel� lorsqu'une entit� a ajout� un nouveau component
		 * Elle permet d'ajouter l'entit� au System si celle-ci corresponds au crit�res
		 * @param	id l'id de l'entit�
		 */
		inline void OnComponentAdd(entity_id id) {
			if (!AcceptedEntity(id) && CanAcceptEntity(id)) {
				ToData[id::Index(id)] = component_id{ static_cast<u32>(ToEntity.size()) };
				AddData(ToEntity, id);
			}
		}

		/**
		 * Ev�nement appel� lorsqu'une entit� a supprim� un component qu'elle posss�dais
		 * Elle permet de supprimer l'entit� au System si celle-ci ne corresponds plus au crit�res
		 * @param	id l'id de l'entit�
		 */
		inline void OnComponentRemove(entity_id id) {
			if (AcceptedEntity(id) && !CanAcceptEntity(id)) {
				RemoveEntity(id);
			}
		}

		/**
		 * Fonction permettant de savoir si une entit� est poss�d� par le System
		 * @param	id l'id de l'entit�
		 * @return	vrai si l'entit� est poss�d� par le System, faux sinon
		 */
		inline bool AcceptedEntity(ecs::entity_id id) {
			return ToData[id::Index(id)] != id::invalid_id;
		}

		/**
		 * Fonction d'Initialisation du System selon la sc�ne qui le poss�de
		 * Celle-ci aura pour but de r�cup�rer les components dont le System d�pends provenant de la sc�ne via scene.getComponent<Component>()
		 * Et d'ajouter ces m�me components au d�pendances de celui-ci via la m�thode SetDependencies()
		 * @param	scene la sc�ne poss�dant le System
		 */
		virtual void Initialize(Scene& scene);

	protected:
		/**
		 * Permet de supprimer l'entit� du System
		 * @param	id l'id de l'entit�
		 */
		void RemoveEntity(entity_id id);

		/**
		 * Wrapper pour supprimer d'un Array l'�lement � la position index
		 */
		template<typename T> inline void RemoveData(illusion::util::Array<T>& array, entity_id index) {
			util::EraseUnordered(array, index);
		}

		/**
		 * Wrapper pour ajouter un element � un Array donn�
		 */
		template<typename T> inline void AddData(illusion::util::Array<T>& array, T data) {
			array.push_back(data);
		}

		/**
		 * Fonction pour savoir si une entit� est conforme aux d�pendances du System
		 * Pour qu'une entit� soit conforme elle doit utiliser tous les components dont le System d�pend
		 * @param	id l'id de l'entit�
		 * @return	vrai si l'entit� est acceptable, faux sinon
		 */
		inline bool CanAcceptEntity(ecs::entity_id id) {
			for (auto const& val : componentsDeps) {
				if (val->getIndex(id) == ecs::id::invalid_id) return false;
			}
			return true;
		}

		/**
		 * Fonction permettant d'ajouter les Components dont d�pend le System
		 * Cette fonction doit �tre appel� lors de la fonction d'Initialisation du System
		 */
		constexpr void SetDependencies() {}
		template<typename Arg1, class... Args> inline void SetDependencies(Arg1 c1, Args... cNext) {
			componentsDeps.push_back(c1);
			SetDependencies(cNext...);
		}

	public:
		static util::Map<size_t, System*> AllSystems;

		static void AppendCoreSystems();

		template<typename T>static void AppendSystems() {
			System::AllSystems[typeid(T).hash_code()] = new T();
		}
	};
}