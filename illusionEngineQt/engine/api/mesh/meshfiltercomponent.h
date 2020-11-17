#pragma once
#include "engine/components/componentcommon.h"
#include "common/primitivetypes.h"

namespace illusion::mesh::meshfilter {

    DEFINE_TYPED_ID(meshfilter_id);

    class Component final {
    public:
        constexpr explicit Component(meshfilter_id id) : _id{ id } {}
        constexpr Component() : _id{ id::invalid_id } {}
        constexpr meshfilter_id get_id() const { return _id; }
        bool is_valid() const { return id::is_valid(_id); }

    private:
        meshfilter_id _id;
    };

}
