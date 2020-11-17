#pragma once

#include "../componentcommon.h"
#include "engine/core/mesh.h"

namespace illusion::mesh::meshfilter {

    struct init_info{
        illusion::core::Mesh* mesh;
    };

    Component create(init_info info, game_entity::Entity entity);
    void remove(Component c);

}
