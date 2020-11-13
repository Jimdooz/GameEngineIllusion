#pragma once
#include "engine/components/componentcommon.h"

namespace illusion::script {

    DEFINE_TYPED_ID(script_id);

    class Component final {
    public:
        constexpr explicit Component(script_id id) : _id{ id } {}
        constexpr Component() : _id{ id::invalid_id } {}
        constexpr script_id get_id() const { return _id; }
        bool is_valid() const { return id::is_valid(_id); }

    private:
        script_id _id;
    };

}
