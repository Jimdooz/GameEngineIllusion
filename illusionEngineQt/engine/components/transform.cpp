#include "transform.h"
#include "entity.h"

#include <iostream>

namespace illusion::transform {

namespace {
utl::vector<math::Vector3> positions;
utl::vector<math::Quaternion> rotations;
utl::vector<math::Vector3> scales;
utl::vector<Component> parents;
utl::vector<utl::vector<Component>> childs;
}

Component create(init_info info, game_entity::Entity entity){
    assert(entity.is_valid());
    id::id_type entity_index{ id::index(entity.get_id()) };

    if(positions.size() > entity_index){
        positions[entity_index] = math::Vector3(info.position[0], info.position[1], info.position[2]);
        rotations[entity_index] = math::Quaternion(info.rotation[0], info.rotation[1], info.rotation[2], info.rotation[3]);
        scales[entity_index] = math::Vector3(info.scale[0], info.scale[1], info.scale[2]);

        parents[entity_index] = info.parent;
        childs[entity_index] = utl::vector<Component>();
    }else{
        assert(positions.size() == entity_index);
        positions.emplace_back(math::Vector3(info.position[0], info.position[1], info.position[2]));
        rotations.emplace_back(math::Quaternion(info.rotation[0], info.rotation[1], info.rotation[2], info.rotation[3]));
        scales.emplace_back(math::Vector3(info.scale[0], info.scale[1], info.scale[2]));

        parents.emplace_back(info.parent);
        childs.emplace_back(utl::vector<Component>());

        entity_index = static_cast<id::id_type>(positions.size() - 1);
    }

    //Create Component
    Component component(transform_id{ entity_index });

    //Test if parent exist
    if(info.parent.is_valid()){
        childs[info.parent.get_id()].push_back(component);
    }

    return component;
}

void remove(Component c){
    //No need to do anything
    assert(c.is_valid());
    return;
}

math::Vector3& Component::position() const {
    assert(is_valid());
    return positions[id::index(_id)];
}

math::Quaternion& Component::rotation() const {
    assert(is_valid());
    return rotations[id::index(_id)];
}

math::Vector3& Component::scale() const {
    assert(is_valid());
    return scales[id::index(_id)];
}

Component& Component::parent() const {
    assert(is_valid());
    return parents[id::index(_id)];
}

utl::vector<Component>* Component::child() const {
    assert(is_valid());
    return &childs[id::index(_id)];
}

void Component::compute_model(math::Matrix4x4 &model) const {
    assert(is_valid());

    if(parent().is_valid()){
        parent().compute_model(model);
    }

    model.translate(position());
    model.scale(scale());
    model.rotate(rotation());
}

}
