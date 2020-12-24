#pragma once

#include "ecs/Ecs.h"

namespace illusion::ecs {
	DEFINE_TYPED_ID(entity_id);

	struct EntityInfo {
        //Position
        //Rotation
        //Scale
	};
	struct Entities {
		// Tableau 
		illusion::util::Array<id::generation_type> m_entities;
		illusion::util::Deque<entity_id> m_freeIds;

		//Variable représentant l'id la plus grande vue pendant le déroulement de l'application
		//Celà sera utile pour avoir une information sur la positions des générations jamais vues
		// [-----|               ]
		//		maxId
		u32 m_maxId = 0; //Proposition de meilleur nom ?

		entity_id Create(EntityInfo info); //
		entity_id GetId(u32 index);
		void Destroy(entity_id id); // Va mettre le premier bit à 0 de generation (entities)
		void DestroyAtIndex(entity_id id);
		// IsAlive ? IsValid ? Exists ? AreCurrentlyNotDead ?
		bool IsAlive(entity_id id); // Va retourner le premier bit de generation
		bool IsAliveAtIndex(entity_id id); // Va retourner le premier bit de generation
	};
}