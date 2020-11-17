#include "meshfilter.h"
#include "../entity.h"

namespace illusion::mesh::meshfilter {

namespace {
    utl::vector<illusion::core::Mesh*> meshes;
}

Component create(init_info info, game_entity::Entity entity){
    assert(entity.is_valid());
    id::id_type entity_index{ id::index(entity.get_id()) };

    if(meshes.size() > entity_index){
        meshes[entity_index] = info.mesh;
    }else{
        assert(meshes.size() == entity_index);
        meshes[entity_index] = info.mesh;

        entity_index = static_cast<id::id_type>(meshes.size() - 1);
    }

    //Create Component
    Component component(meshfilter_id{ entity_index });

    return component;
}

void remove(Component c){
    //No need to do anything
    assert(c.is_valid());
    return;
}

}
