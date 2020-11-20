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
		illusion::util::Array<id::generation_type> entities;
		illusion::util::Deque<entity_id> freeIds;

		//Variable représentant l'id la plus grande vue pendant le déroulement de l'application
		//Celà sera utile pour avoir une information sur la positions des générations jamais vues
		// [-----|               ]
		//		maxId
		u32 maxId = 0; //Proposition de meilleur nom ?

		// TODO : Réaliser un système qui permet de check si le premier bit de la génération
		//			1 -> en vie
		//			0 -> mort

        entity_id Create(EntityInfo& info); //
        void Destroy(entity_id id); // Va mettre le premier bit à 0 de generation (entities)
		// IsAlive ? IsValid ? Exists ? AreCurrentlyNotDead ?
        bool IsAlive(entity_id id); // Va retourner le premier bit de generation
	};
}