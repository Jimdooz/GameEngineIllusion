#pragma once
#include "engine/components/componentcommon.h"
#include "common/primitivetypes.h"

namespace illusion::transform {

    DEFINE_TYPED_ID(transform_id);

    class Component final {
    public:
        constexpr explicit Component(transform_id id) : _id{ id } {}
        constexpr Component() : _id{ id::invalid_id } {}
        constexpr transform_id get_id() const { return _id; }
        bool is_valid() const { return id::is_valid(_id); }

        math::Vector3& position() const;
        math::Quaternion& rotation() const;
        math::Vector3& scale() const;

        Component& parent() const;
        utl::vector<Component>* child() const;

        //Computed
        void compute_model(math::Matrix4x4 &model) const;
    private:
        transform_id _id;
    };

}
