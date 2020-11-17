#include <iostream>

#include "entity.h"
#include "engine/components/transform.h"
#include "engine/components/script.h"
#include "engine/components/mesh/meshfilter.h"

namespace illusion::game_entity {

// Anonymous namespace to protect from other files
// Same as add static variable
namespace  {
    utl::vector<transform::Component>   transforms;
    utl::vector<utl::vector<script::Component>>      scripts; //Point to the list of scripts

    //Core
    utl::vector<mesh::meshfilter::Component>      meshfilters;

    utl::vector<id::generation_type>    generations;
    utl::deque<entity_id>               free_ids;
}

Entity create(entity_info info){
    assert(info.transform); // All game entities must have a transform component
    if(!info.transform) return Entity{};

    entity_id id;

    if(free_ids.size() > id::min_deleted_elements){
        id = free_ids.front();
        assert(!is_alive(id));
        free_ids.pop_front();
        id = entity_id{ id::new_generation(id) };
        ++generations[id::index(id)]; //Increment generation
    }else{
        id = entity_id{ static_cast<id::id_type>(generations.size()) };
        generations.push_back(0);
        // Resize components
        // NOTE : we don't call resize(), so the number of memory allocations stays low
        transforms.emplace_back();
        scripts.emplace_back();
    }

    const Entity new_entity { id };
    const id::id_type index { id::index(id) };

    //--> Create transform component : every gameentity have a transform component
    assert(!transforms[index].is_valid());
    transforms[index] = transform::create(*info.transform, new_entity);
    if(!transforms[index].is_valid()) return Entity{}; //Error case

    //--> Create script component
    /*if(info.script && info.script->script_creator){
        //assert(!scripts[index].is_valid());
        scripts[index].push_back(script::create(*info.script, new_entity));
        if(!scripts[index].back().is_valid()) return Entity{}; //Error case
    }*/

    return new_entity;
}

void remove(entity_id id){
    const id::id_type index { id::index(id) };
    assert(is_alive(id));

    if(scripts[index].size() > 0){
        for(u32 i = 0; i < scripts[index].size();i++){
            std::cout << ">>>>" << scripts[index].at(i).get_id() << std::endl;
            script::remove(scripts[index].at(i));
        }
        scripts[index].clear();
    }

    utl::vector<transform::Component> *childs = transforms[index].child();

    // Call parent we dont exist anymore
    if(transforms[index].parent().is_valid()){
        utl::vector<transform::Component> *parent_childs = transforms[transforms[index].parent().get_id()].child();
        u32 i = 0;
        for(; i < parent_childs->size(); i++){
            if(parent_childs->at(i).get_id() == id) break;
        }
        if(i < parent_childs->size()){
            parent_childs->erase( parent_childs->begin() + i );
        }
    }

    // Remove all childs
    for(u64 i = 0; i < childs->size(); i++){
        remove(static_cast<entity_id>(childs->at(i).get_id()));
        i--;
    }

    transform::remove(transforms[index]);
    transforms[index] = transform::Component{}; //invalid component

    free_ids.push_back(id);
}

bool is_alive(entity_id id){
    assert(id::is_valid(id));
    const id::id_type index{ id::index(id) };

    assert(index < generations.size());
    assert(generations[index] == id::generation(id));

    return generations[index] == id::generation(id) && transforms[index].is_valid();
}

u64 get_active_slots(){
    return generations.size();
}

Entity get_entity(entity_id id){
    assert(is_alive(id));
    return Entity{ id };
}

entity_id get_index_id(entity_id id){
    assert(id::is_valid(id));
    const id::id_type index{ id::index(id) };
    assert(index < generations.size());
    entity_id real_id = static_cast<entity_id>(id::set_generation(id, generations[index]));
    return real_id;
}

transform::Component Entity::transform() const {
    assert(is_alive(_id));
    const id::id_type index{ id::index(_id) };
    return transforms[index];
}

utl::vector<script::Component>& Entity::script() const{
    assert(is_alive(_id));
    const id::id_type index{ id::index(_id) };
    return scripts[index];
}

void Entity::addComponent( script::Component &c ) const {
    std::cout << "ADD COMPONENT" << std::endl;
    script().push_back(c);
}

void Entity::removeComponent( script::Component &c ) const {
    std::cout << "REMOVE COMPONENT" << std::endl;
    for(u32 i = 0; i < script().size(); i++){
        if(script().at(i).get_id() == c.get_id()){
            script::remove(script().at(i));
            utl::erase_unordered(script(), i);
            std::cout << "--> SUCCESS !" << std::endl;
            break;
        }
    }
}

template<class script_class> script::Component Entity::getComponent() const {
    utl::vector<script::Component> scripts = script();
    size_t hcode_script = typeid (script_class).hash_code();
    for(u32 i = 0; i < scripts.size(); i++){
        auto script_pointing = illusion::script::detail::get_script( scripts.at(i) )->get();
        if(typeid (*script_pointing).hash_code() == hcode_script) return scripts.at(i);
    }

    return script::Component{};
}

}
