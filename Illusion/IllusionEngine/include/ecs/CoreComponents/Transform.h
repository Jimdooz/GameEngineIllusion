#pragma once

#include "ecs/Component.h"

namespace illusion::ecs::core {

	struct Transform : public Component {
		Transform(Scene* scene) : Component(scene) {
			COMPONENT_PUBLIC(name);

			COMPONENT_PUBLIC(position);
			COMPONENT_PUBLIC(rotation);
			COMPONENT_PUBLIC(scale);

			COMPONENT_PROTECTED(parent);
			COMPONENT_PROTECTED(childs);
		}

		COMPONENT_NAME("Transform");
		COMPONENT_REGISTER(Transform);

		COMPONENT_DATA(u8, currentTick);			//Optimisation pour ne pas recalculer la matrice du model
		COMPONENT_DATA(std::string, name);

		COMPONENT_DATA(Vec3, position);
		COMPONENT_DATA(Quaternion, rotation);
		COMPONENT_DATA(Vec3, scale);

		//Copy Datas for optimisation
		COMPONENT_DATA(Vec3, s_position);
		COMPONENT_DATA(Quaternion, s_rotation);
		COMPONENT_DATA(Vec3, s_scale);

		COMPONENT_DATA(Mat4x4, modelTransform);		//Matrice de transformation total du model
		COMPONENT_DATA(Mat4x4, elementTransform);	//Matrice de transformation de l'objet

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
		 * @param	id l'id de l'entit�
		 */
		void RemoveParent(ecs::entity_id id);

		/**
		 * Permet d'ajouter un enfant � la liste d'un id
		 * @param	id l'id de l'entit�
		 * @param	childId l'id de l'enfant � ajouter
		 */
		void SetChild(ecs::entity_id id, ecs::entity_id childId);

		/**
		 * Permet de supprimer un enfant à la liste d'un id
		 * @param	id l'id de l'entité
		 * @param	childId l'id de l'enfant à supprimer
		 */
		void RemoveChild(ecs::entity_id id, ecs::entity_id childId);

		/**
		 * Permet de savoir si une entit� � pour parent une autre entit� recursivement
		 * Retourne faux si les deux entit�s sont �gales ou l'une d'entre elle est invalide
		 * @param	id l'id de l'entit�
		 * @param	parentId l'id du parent
		 * @return	vrai si le parent existe, faux sinon
		 */
		bool HaveParentRecursive(ecs::entity_id id, ecs::entity_id parentId);

		Mat4x4 &ComputeModel(ecs::component_id component);

		virtual void AddDatas(ecs::entity_id id) override;

		virtual void RemoveDatas(ecs::component_id index, ecs::entity_id id) override;

		virtual void AfterRemoveComponent(entity_id id);
	};

}
