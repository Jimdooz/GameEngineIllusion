#include "ecs/Entity.h"

#include <bitset>

namespace illusion::ecs {

	entity_id Entities::Create(){
		u32 nbEntities = m_entities.size();
		entity_id id;

		if (m_freeIds.size() > 0) {
			id = m_freeIds.front();
			m_freeIds.pop_front();
			id = entity_id{ id::NewGeneration(id) };
			m_entities[id::Index(id)] = id::Generation(id);
		}
		else {
			id = entity_id{ static_cast<id::id_type>(nbEntities) };
			m_entities.push_back(0);
		}

		//Set entity alive
		m_entities[id::Index(id)] = id::SetGenerationAlive(m_entities[id::Index(id)], true);

		//INTERNAL_INFO(std::bitset<16>(m_entities[id::Index(id)]), " | ", std::bitset<32>(id));

		return id;
	}

	entity_id Entities::Create(u32 index) {
		u32 nbEntities = m_entities.size();
		entity_id id;

		// Check if id already exist
		if (index < nbEntities && IsAliveAtIndex(entity_id(index))) return GetId(index);

		if (m_freeIds.size() > 0) {
			for (u32 i = 0; i < m_freeIds.size(); i++) {
				if (id::Index(m_freeIds[i]) == index) {
					id = m_freeIds[i];
					id = entity_id{ id::NewGeneration(id) };
					m_entities[id::Index(id)] = id::Generation(id);

					m_freeIds.erase(m_freeIds.begin() + i);

					break;
				}
			}
		}
		else {
			for (u32 i = nbEntities; i <= index; i++) {
				id = entity_id{ static_cast<id::id_type>(i) };
				m_entities.push_back(0);
				m_entities[id::Index(id)] = id::SetGenerationAlive(m_entities[id::Index(id)], false);

				if(i != index) m_freeIds.push_back(id);
			}
		}

		m_entities[id::Index(id)] = id::SetGenerationAlive(m_entities[id::Index(id)], true);

		return id;
	}

	entity_id Entities::GetId(u32 index) {
		return static_cast<entity_id>(id::SetGeneration(static_cast<id::id_type>(index), id::GetGenerationValue(m_entities[id::Index(index)])));
	}

	void Entities::Destroy(entity_id id) {
		if (!IsAlive(id)) return;

		m_freeIds.push_back(id);
		m_entities[id::Index(id)] = id::SetGenerationAlive(m_entities[id::Index(id)], false);
	}

	void Entities::DestroyAtIndex(entity_id id) {
		entity_id newId = GetId(id);
		if (!IsAlive(newId)) return;

		m_freeIds.push_back(newId);
		m_entities[id::Index(newId)] = id::SetGenerationAlive(m_entities[id::Index(newId)], false);
	}

	bool Entities::IsAlive(entity_id id) {
		if (!id::IsValid(id) ) return false;
		id::id_type index = id::Index(id);
		if (index > m_entities.size()) return false;
		return id::GetGenerationValue(m_entities[index]) == id::Generation(id) && id::IsGenerationAlive(m_entities[index]);
	}

	bool Entities::IsAliveAtIndex(entity_id id) {
		entity_id newId = GetId(id);
		return IsAlive(newId);
	}
}