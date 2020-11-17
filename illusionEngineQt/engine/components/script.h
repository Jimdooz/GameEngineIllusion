#pragma once
#include "componentcommon.h"

namespace illusion::script {

//Structure to represent the data of the transform component
struct init_info{
    detail::script_creator script_creator;
};

Component create(init_info info, game_entity::Entity entity);
void remove(Component c);

}
