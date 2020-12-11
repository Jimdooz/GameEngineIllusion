#pragma once

#include "ecs/Component.h"

namespace illusion::ecs::core {

	struct Transform : public Component {
		Transform(Scene* scene) : Component(scene) {}

		util::Array<Vec3> position;
		util::Array<Quaternion> rotation;
		util::Array<Vec3> scale;

		util::Array<ecs::entity_id> parent;
		util::Array<util::Array<ecs::entity_id>> childs;

		/**
		 * Permet de d�finir le parent d'une entit�
		 * @param	id l'id de l'entit� 
		 * @param	parentId l'id du parent
		 */
		void SetParent(ecs::entity_id id, ecs::entity_id parentId);

		/**
		 * Permet d'ajouter un enfant � la liste d'un id
		 * @param	id l'id de l'entit�
		 * @param	childId l'id de l'enfant � ajouter
		 */
		void SetChild(ecs::entity_id id, ecs::entity_id childId);

		/**
		 * Permet de supprimer un enfant � la liste d'un id
		 * @param	id l'id de l'entit�
		 * @param	childId l'id de l'enfant � supprimer
		 */
		void RemoveChild(ecs::entity_id id, ecs::entity_id childId);

		virtual void AddComponentDatas(ecs::entity_id id) override;

		virtual void RemoveComponentDatas(ecs::component_id index, ecs::entity_id id);
	};

}