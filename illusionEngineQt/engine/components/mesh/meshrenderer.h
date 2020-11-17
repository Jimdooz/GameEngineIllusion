#pragma once

#include "../componentcommon.h"

namespace illusion::mesh::meshrenderer {

    struct init_info{
    };

    Component create(init_info info, game_entity::Entity entity);
    void remove(Component c);

}
