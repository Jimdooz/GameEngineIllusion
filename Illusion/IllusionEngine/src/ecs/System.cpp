#include "ecs/System.h"
#include "ecs/Scene.h"

namespace illusion::ecs {
	util::Map<size_t, System*> System::AllSystems;

	void System::Update() {}
	void System::LateUpdate() {}
	void System::FixedUpdate() {}

	//Events
	void System::OnEntityCreate(entity_id id) {
		id::id_type index = id::Index(id);
		if (index >= ToData.size()) ToData.resize(static_cast<size_t>(index) + 1);
		ToData[index] = component_id{ id::invalid_id };
	}
	void System::OnEntityDestroy(entity_id id) {
		RemoveEntity(id);
	}

	void System::RemoveEntity(entity_id id) {
		if (ToEntity.size() <= 0) return;
		const entity_id getId{ id::Index(id) };
		const entity_id lastId{ id::Index(ToEntity.back()) };
		const entity_id index{ ToData[getId] };
		if (index == id::invalid_id) return;

		RemoveData(ToEntity, index);
		ToData[lastId] = component_id{ index };
		ToData[getId] = component_id{ id::invalid_id };
	}

	//Helpers

	// /!\ to override
	void System::Initialize(Scene& scene) {}

	void System::AppendCoreSystems() {
	}
}