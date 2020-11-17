#pragma once
#include "componentcommon.h"

namespace illusion::transform {

//static f32 position_default[3] = {0.f, 0.f, 0.f};
static f32 rotation_default[4] = {1.f, 0.f, 0.f, 0.0f};
static f32 scale_default[3] = {1.f, 1.f, 1.f};

//Structure to represent the data of the transform component
struct init_info{

    init_info() :
        position{0.f, 0.f, 0.f},
        rotation{1.f, 0.f, 0.f, 0.0f},
        scale{1.f, 1.f, 1.f},
        parent{}
    {}

    init_info(f32 position[3], f32 rotation[4] = rotation_default, f32 scale[3] = scale_default) :
        position{*position},
        rotation{*rotation},
        scale{*scale},
        parent{}
    {}

    init_info(transform_id parent) :
        position{0.f, 0.f, 0.f},
        rotation{1.f, 0.f, 0.f, 0.0f},
        scale{1.f, 1.f, 1.f},
        parent{parent}
    {}

    f32 position[3];
    f32 rotation[4];
    f32 scale[3];

    Component parent;
};

Component create(init_info info, game_entity::Entity entity);
void remove(Component c);

}
