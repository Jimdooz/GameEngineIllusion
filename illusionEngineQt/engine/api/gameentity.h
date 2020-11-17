#pragma once

#include <typeinfo>
#include <typeindex>

#include "engine/components/componentcommon.h"
#include "engine/api/transformcomponent.h"
#include "engine/api/scriptcomponent.h"
#include "engine/api/mesh/meshfiltercomponent.h"
#include "engine/api/mesh/meshrenderercomponent.h"

#include "engine/core/time.h"

namespace illusion {

namespace game_entity {

    //Create an id_type to this namespace
    DEFINE_TYPED_ID(entity_id);

    class Entity {
    public:
        constexpr explicit Entity(entity_id id) : _id{ id } {}
        constexpr Entity() : _id{ id::invalid_id } {}
        constexpr entity_id get_id() const { return _id; }
        bool is_valid() const { return id::is_valid(_id); }

        //Defined in entity.cpp
        transform::Component transform() const;
        utl::vector<script::Component>& script() const;
        void addComponent( script::Component &c ) const;
        void removeComponent( script::Component &c ) const;
        template<class script_class> script::Component getComponent() const;
    private:
        entity_id _id;
    };

} //namespace game_entity

namespace script {
    class EntityScript : public game_entity::Entity {
    public:
        virtual ~EntityScript() = default;
        virtual void Start(){}
        virtual void Update(Time){}

    protected:
        constexpr explicit EntityScript(game_entity::Entity entity)
            : game_entity::Entity{ entity }{}
    };

    namespace detail {
        using script_ptr = std::unique_ptr<illusion::script::EntityScript>;
        using script_creator = script_ptr(*)(game_entity::Entity entity);
        using string_hash = std::hash<std::string>;

        u8 register_script(size_t, script_creator);
        u8 register_type(size_t index, const char* name);

        script_creator get_script_creator(size_t tag);

        template<class script_class> script_ptr create_script(game_entity::Entity entity){
            assert(entity.is_valid());
            return std::make_unique<script_class>(entity);
        }
        u8 add_script_name(const char* name);
        utl::vector<std::string>& get_scripts_name();
        std::string& get_script_name(Component script);
        script_ptr* get_script( Component c );

        const utl::vector<detail::script_ptr>& all_scripts_game_entities();
    } //namespace detail

//Macro to add a new script to the game engine
//When create a new EntityScript class, add this macro before class declaration
#define REGISTER_SCRIPT(TYPE)                                       \
    namespace {                                                     \
        const u8 _name_##TYPE                                       \
        { illusion::script::detail::add_script_name(#TYPE) };       \
        const u8 _type_##TYPE                                       \
        { illusion::script::detail::register_type(typeid(TYPE).hash_code(), #TYPE) };   \
        const u8 _reg_##TYPE                                        \
        {   illusion::script::detail::register_script (             \
            illusion::script::detail::string_hash()(#TYPE),         \
            &illusion::script::detail::create_script<TYPE>) };      \
     }


} //namespace script

}
