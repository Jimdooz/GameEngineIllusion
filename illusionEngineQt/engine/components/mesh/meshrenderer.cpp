#include "meshrenderer.h"
#include "../entity.h"

namespace illusion::mesh::meshrenderer {

namespace {
}

Component create(init_info info, game_entity::Entity entity){
    assert(entity.is_valid());
    id::id_type entity_index{ id::index(entity.get_id()) };

    //Create Component
    Component component(meshrenderer_id{ entity_index });

    return component;
}

void remove(Component c){
    //No need to do anything
    assert(c.is_valid());
    return;
}

}
