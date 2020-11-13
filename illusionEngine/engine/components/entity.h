#pragma once
#include "componentcommon.h"

namespace illusion {

#define INIT_INFO(component) namespace component { struct init_info; }
INIT_INFO(transform);
INIT_INFO(script);
#undef INIT_INFO

namespace game_entity {

struct entity_info {
    transform::init_info* transform { nullptr };
    //script::init_info* script { nullptr };
};

//Create entity into the game
Entity create(entity_info info);
//Remove the entity from the game
void remove(entity_id id);
//Test if the given Entity exist
bool is_alive(entity_id id);
//Give the number of slot active for entity number
u64 get_active_slots();
//Get entity by its Id
Entity get_entity(entity_id id);
//Get entity by its Id
entity_id get_index_id(entity_id id);

}}
