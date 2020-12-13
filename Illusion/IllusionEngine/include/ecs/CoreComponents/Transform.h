#pragma once

#include "ecs/Component.h"

namespace illusion::ecs::core {

	struct Transform : public Component {
		Transform(Scene* scene) : Component(scene) {}

		COMPONENT_DATA(Vec3, position);
		COMPONENT_DATA(Quaternion, rotation);
		COMPONENT_DATA(Vec3, scale);

		COMPONENT_DATA(ecs::entity_id, parent);
		COMPONENT_DATA(util::Array<ecs::entity_id>, childs);

		/**
		 * Permet de définir le parent d'une entité
		 * @param	id l'id de l'entité 
		 * @param	parentId l'id du parent
		 */
		void SetParent(ecs::entity_id id, ecs::entity_id parentId);

		/**
		 * Permet de supprimer le parent d'un element
		 * @param	id l'id de l'entité
		 */
		void RemoveParent(ecs::entity_id id);

		/**
		 * Permet d'ajouter un enfant à la liste d'un id
		 * @param	id l'id de l'entité
		 * @param	childId l'id de l'enfant à ajouter
		 */
		void SetChild(ecs::entity_id id, ecs::entity_id childId);

		/**
		 * Permet de supprimer un enfant à la liste d'un id
		 * @param	id l'id de l'entité
		 * @param	childId l'id de l'enfant à supprimer
		 */
		void RemoveChild(ecs::entity_id id, ecs::entity_id childId);

		/**
		 * Permet de savoir si une entité à pour parent une autre entité recursivement
		 * Retourne faux si les deux entités sont égales ou l'une d'entre elle est invalide
		 * @param	id l'id de l'entité 
		 * @param	parentId l'id du parent
		 * @return	vrai si le parent existe, faux sinon
		 */
		bool HaveParentRecursive(ecs::entity_id id, ecs::entity_id parentId);

		virtual void AddComponentDatas(ecs::entity_id id) override;

		virtual void RemoveComponentDatas(ecs::component_id index, ecs::entity_id id);
	};

}