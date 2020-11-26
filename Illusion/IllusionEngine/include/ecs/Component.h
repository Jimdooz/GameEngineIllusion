#pragma once

#include "ecs/Entity.h"

namespace illusion::ecs {
	DEFINE_TYPED_ID(component_id);

	// 1 - les data components	 [CUSTOM]

	// 2 - data to Entity Id     [STATIC]
	// 3 - entity id to data     [STATIC]
	struct Component {
		// Tableau avec pour index l'id des components, et pour valeur les entity id
		illusion::util::Array<entity_id> ToEntity;

		// Tableau avec pour index l'id des entit�s, et pour valeur la position des donn�es
		// Elle permet de pointer vers la liste des donn�es qui est compact�e
		illusion::util::Array<component_id> ToData;

		inline void AddComponent(entity_id id);
		inline void RemoveComponent(entity_id id);
	};

}