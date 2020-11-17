#pragma once
#include "engine/components/componentcommon.h"
#include "common/primitivetypes.h"

namespace illusion::mesh::meshrenderer {

    DEFINE_TYPED_ID(meshrenderer_id);

    class Component final {
    public:
        constexpr explicit Component(meshrenderer_id id) : _id{ id } {}
        constexpr Component() : _id{ id::invalid_id } {}
        constexpr meshrenderer_id get_id() const { return _id; }
        bool is_valid() const { return id::is_valid(_id); }

    private:
        meshrenderer_id _id;
    };

}
