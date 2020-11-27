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

		//Events
		void OnEntityCreate(entity_id id);
		void OnEntityDestroy(entity_id id);
		template<typename C> void OnComponentAdd(Scene& scene, entity_id id) {
			if (AcceptedComponent(scene, typeid(C).hash_code())) {
				ToData[id::Index(id)] = component_id{ static_cast<u32>(ToEntity.size()) };
				AddData(ToEntity, id);
				AddDatas(scene, id);
			}
		}
		template<typename C> void OnComponentRemove(entity_id id) {
			if (AcceptedComponent(typeid(C).hash_code())) {
				RemoveEntity(id);
			}
		}
		void RemoveEntity(entity_id id);

	protected:
		//Helpers
		template<typename T> void RemoveData(illusion::util::Array<T>& array, entity_id index) {
			util::EraseUnordered(array, index);
		}
		template<typename T> void AddData(illusion::util::Array<T>& array, T data) {
			array.push_back(data);
		}

		// /!\ to override
		virtual void AddDatas(Scene& scene, entity_id id);
		virtual void RemoveDatas(entity_id index);

		virtual bool AcceptedComponent(Scene& scene, size_t hashComponent);
	};
}