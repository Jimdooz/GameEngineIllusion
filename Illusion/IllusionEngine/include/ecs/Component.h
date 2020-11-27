#pragma once

#include "ecs/Entity.h"

namespace illusion::ecs {
	DEFINE_TYPED_ID(component_id);

	// 1 - les data components	 [CUSTOM]

	// 2 - data to Entity Id     [STATIC]
	// 3 - entity id to data     [STATIC]
	struct Component {
		// Tableau avec pour index l'id des components, et pour valeur les entity id
		util::Array<entity_id> ToEntity;

		// Tableau avec pour index l'id des entités, et pour valeur la position des données
		// Elle permet de pointer vers la liste des données qui est compactée
		util::Array<component_id> ToData;

		void AddComponent(entity_id id);
		void RemoveComponent(entity_id id);

		void CreateEntity(entity_id id);
		void DestroyEntity(entity_id id);

		component_id getIndex(entity_id id);

	protected:
		template<typename T> void RemoveComponentData(illusion::util::Array<T>& array, entity_id index) {
			util::EraseUnordered(array, index);
		}

		template<typename T> void AddComponentData(illusion::util::Array<T>& array, T& data) {
			array.push_back(data);
		}

		// /!\ to override
		virtual void AddComponentDatas(entity_id id) {}
		virtual void RemoveComponentDatas(entity_id index) {}
	};
}