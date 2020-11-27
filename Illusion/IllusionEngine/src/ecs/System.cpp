#include "ecs/System.h"
#include "ecs/Scene.h"

namespace illusion::ecs {
	void System::Update() {}
	void System::LateUpdate() {}
	void System::FixedUpdate() {}

	//Events
	void System::OnEntityCreate(entity_id id) {
		id::id_type index = id::Index(id);
		if (index >= ToData.size()) ToData.resize(index + 1);
		ToData[index] = component_id{ id::invalid_id };
	}
	void System::OnEntityDestroy(entity_id id) {
		RemoveEntity(id);
	}

	void System::RemoveEntity(entity_id id) {
		const entity_id getId{ id::Index(id) };
		const entity_id lastId{ id::Index(ToEntity.back()) };
		const entity_id index{ ToData[getId] };

		RemoveData(ToEntity, index);
		RemoveDatas(index);
		ToData[lastId] = component_id{ index };
		ToData[getId] = component_id{ id::invalid_id };
	}

	//Helpers

	// /!\ to override
	void System::AddDatas(Scene& scene, entity_id id) {}
	void System::RemoveDatas(entity_id index) {}

	bool System::AcceptedComponent(Scene& scene, size_t hashComponent) { return false; }
}