#pragma once

#include "ecs/Entity.h"
#include "ecs/Component.h"

namespace illusion::ecs {

	struct Scene;

	struct System {
		virtual void Update();
		virtual void LateUpdate();
		virtual void FixedUpdate();

		util::Array<entity_id> ToEntity;
		util::Array<component_id> ToData;
		util::Array<Component*> components;

		u32 currIndex = 0;

		//Events
		void OnEntityCreate(entity_id id);
		void OnEntityDestroy(entity_id id);
		template<typename C> inline void OnComponentAdd(entity_id id) {
			if (AcceptEntity(id)) {
				ToData[id::Index(id)] = component_id{ static_cast<u32>(ToEntity.size()) };
				AddData(ToEntity, id);
			}
		}
		template<typename C> inline void OnComponentRemove(entity_id id) {
			if (!AcceptEntity(id)) {
				INTERNAL_INFO("REMOVE ", id)
				RemoveEntity(id);
			}
		}
		void RemoveEntity(entity_id id);

		// /!\ to override
		virtual void Initialize(Scene& scene);

	protected:
		//Helpers
		template<typename T> inline void RemoveData(illusion::util::Array<T>& array, entity_id index) {
			util::EraseUnordered(array, index);
		}
		template<typename T> inline void AddData(illusion::util::Array<T>& array, T data) {
			array.push_back(data);
		}

		// /!\ to override
		inline bool AcceptEntity(ecs::entity_id id) {
			for (auto const& val : components) {
				if (val->getIndex(id) == ecs::id::invalid_id) return false;
			}
			return true;
		}

		constexpr void AddComponents() {}

		template<typename Arg1, class... Args> inline void AddComponents(Arg1 c1, Args... cNext) {
			components.push_back(c1);
			AddComponents(cNext...);
		}
	};
}