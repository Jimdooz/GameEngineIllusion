#pragma once
#include "script.h"
#include "entity.h"

namespace illusion::script {

namespace  {
    utl::vector<detail::script_ptr>     entity_scripts; // Compact agencement
    utl::vector<id::id_type>            id_mapping;     // Non-Compact default system pointing to entity_script
    utl::vector<id::id_type>            invert_mapping;     // Non-Compact default system pointing to entity_script

    utl::vector<id::generation_type>    generations;
    utl::vector<script_id>              free_ids;

    //Contain all script creator
    using script_registry = std::unordered_map<size_t, detail::script_creator>;
    using script_type_mapping = std::unordered_map<size_t, std::string>;

    // NOTE : we put this static variable in a function beacause of
    //        the initialization order of static data.This way, we can
    //        be certain that the dta is initialized before accessing it.
    script_registry& registry(){
        static script_registry reg;
        return reg;
    }

    script_type_mapping& type_map(){
        static script_type_mapping t_map;
        return t_map;
    }

    utl::vector<std::string>& script_names(){
        static utl::vector<std::string> names;
        return names;
    }

    bool exists(script_id id){
        assert(id::is_valid(id));
        const id::id_type index{ id::index(id) };
        const id::id_type generation{ id::generation(id) };
        assert(index < generations.size() && id_mapping[index] < entity_scripts.size());
        assert(generations[index] == generation);
        return (generations[index] == generation) && entity_scripts[id_mapping[index]] &&  entity_scripts[id_mapping[index]]->is_valid();
    }

}// anonymous namespace

namespace detail {
    //Return the iterator of the registry
    const std::unordered_map<size_t, detail::script_creator>& get_registry(){
        return registry();
    }

    //Add script creator to the registry
    u8 register_script(size_t tag, script_creator func){
        bool result { registry().insert(script_registry::value_type{tag, func}).second };
        assert(result);
        return result;
    }

    u8 register_type(size_t index, const char* name) {
        bool result { type_map().insert( script_type_mapping::value_type{ index, name } ).second };
        std::cout << "Type : " << index << std::endl;
        std::cout << "Add : " << name << std::endl;
        return result;
    }

    //Get script creator by tag
    script_creator get_script_creator(size_t tag){
        auto script = illusion::script::registry().find(tag);
        assert(script != illusion::script::registry().end() && script->first == tag);
        return script->second;
    }

    //Add script name
    u8 add_script_name(const char* name){
        script_names().emplace_back(name);
        return true;
    }

    //Return all scripts name
    utl::vector<std::string>& get_scripts_name(){
        return script_names();
    }

    std::string& get_script_name(Component script){
        auto script_pointing = illusion::script::detail::get_script( script )->get();
        return type_map()[typeid (*script_pointing).hash_code()];
    }

    script_ptr* get_script( Component c ){
        if(exists(c.get_id())){
            const id::id_type index{ id::index(c.get_id()) };
            return &entity_scripts[id_mapping[index]];
        }
        return nullptr;
    }

    const utl::vector<detail::script_ptr>& all_scripts_game_entities(){
        return entity_scripts;
    }
}

Component create(init_info info, game_entity::Entity entity){
    assert(entity.is_valid());
    assert(info.script_creator);

    script_id id{};

    if(free_ids.size() > id::min_deleted_elements){
        id = free_ids.front();

        assert(!exists(id));

        free_ids.pop_back();
        id = script_id { id::new_generation(id) };
        ++generations[id::index(id)];
    }else{
        id = script_id{static_cast<id::id_type>(id_mapping.size())};
        id_mapping.emplace_back();
        generations.push_back(0);
    }

    assert(id::is_valid(id));
    entity_scripts.emplace_back(info.script_creator(entity)); //Generate script with script_creator
    invert_mapping.emplace_back(id);                          //Create the invert redirection
    assert(entity_scripts.back()->get_id() == entity.get_id());

    const id::id_type index{ static_cast<id::id_type>(entity_scripts.size() - 1) };
    id_mapping[id::index(id)] = index;

    return Component{ id };
}

void remove(Component c){
    assert(c.is_valid() && exists(c.get_id()));
    const script_id id { c.get_id() };
    const id::id_type index{ id_mapping[id::index(id)] };
    const id::id_type last_id{ invert_mapping.back() };

    utl::erase_unordered(entity_scripts, index); //Swip last and id, and remove the new last
    utl::erase_unordered(invert_mapping, index); //Swip last and id, and remove the new last for the invert indexing
    id_mapping[id::index(last_id)] = index;
    id_mapping[id::index(id)] = id::invalid_id;
    free_ids.push_back(id);
}

}
