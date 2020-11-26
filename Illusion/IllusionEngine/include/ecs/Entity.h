#pragma once

#include "ecs/Ecs.h"

namespace illusion::ecs {
	DEFINE_TYPED_ID(entity_id);

	struct EntityInfo {
        //Position
        //Rotation
        //Scale
	};
	class Entities {
		// Tableau 
		illusion::util::Array<id::generation_type> m_entities;
		illusion::util::Deque<entity_id> m_freeIds;

		//Variable repr�sentant l'id la plus grande vue pendant le d�roulement de l'application
		//Cel� sera utile pour avoir une information sur la positions des g�n�rations jamais vues
		// [-----|               ]
		//		maxId
		u32 m_maxId = 0; //Proposition de meilleur nom ?

		// TODO : R�aliser un syst�me qui permet de check si le premier bit de la g�n�ration
		//			1 -> en vie
		//			0 -> mort
		public:
			entity_id Create(EntityInfo info); //
			void Destroy(entity_id id); // Va mettre le premier bit � 0 de generation (entities)
			void DestroyAtIndex(entity_id id);
			// IsAlive ? IsValid ? Exists ? AreCurrentlyNotDead ?
			bool IsAlive(entity_id id); // Va retourner le premier bit de generation
	};
}