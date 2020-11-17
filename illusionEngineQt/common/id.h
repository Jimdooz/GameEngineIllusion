#pragma once
#include "commonheaders.h"
#include <typeinfo>

#include <iostream>

namespace illusion::id {

    //Represent an id for identify the different elements like Objects
    // [generation][index]
    // |            |_ Represent the "real" id of the element
    // |_ Represent the generation that ecrement every time we destroy an object at this id

    // Generation are usefull to avoid pointing to a wrong element
    using id_type = u32;

    //detail namespace
    namespace detail {

        constexpr u32 generation_bits { 10 };
        constexpr u32 index_bit(sizeof(id_type) * 8 - generation_bits);

        // Create a mask to get the index
        // id_type{1}                       --> 0000000000000000000001
        // id_type{1} << index_bit          --> 0000000100000000000000
        // (id_type{1} << index_bit) - 1    --> 0000000011111111111111
        constexpr id_type index_mask{ (id_type{1} << index_bit) - 1 };
        // Mask to get the generation
        constexpr id_type generation_mask{ (id_type{1} << generation_bits) - 1 };

    }

    constexpr id_type invalid_id{ id_type{0} -1 };
    constexpr u32 min_deleted_elements { 1024 };

    //Ternary to choose a type <bool, t1, t2>
    using generation_type = std::conditional_t<detail::generation_bits <= 16, std::conditional_t<detail::generation_bits <= 8, u8, u16>, u32>;

    static_assert (sizeof (generation_type) * 8 >= detail::generation_bits, "Invalid generation type size");
    static_assert (sizeof (id_type) - sizeof (generation_type) > 0, "generation_type size must fit into id_type size");

    inline bool
    is_valid(id_type id){
        return id != invalid_id;
    }

    inline id_type
    index(id_type id){
        id_type index{ id & detail::index_mask };
        assert(index != detail::index_mask);
        return id & detail::index_mask;
    }

    inline id_type
    generation(id_type id){
        return (id >> detail::index_bit) & detail::generation_mask;
    }

    inline id_type set_generation(id_type id, id_type generation){
        assert(generation < detail::generation_mask);
        return index(id) | (generation << detail::index_bit);
    }

    inline id_type new_generation(id_type id){
        const id_type generation{ id::generation(id) + 1 };
        assert(generation < detail::generation_mask);
        return set_generation(id, generation);
    }

    //System to differenciate between id in debug mode & reduce to u32 type in release build

#if _DEBUG
    namespace detail {

    struct id_base {
        constexpr explicit id_base(id_type id) : _id { id } {}
        constexpr operator id_type() const { return _id; }
    private:
        id_type _id;
    };

    }

#define DEFINE_TYPED_ID(name)                                       \
    struct name final : id::detail::id_base {                     \
        constexpr explicit name(id::id_type id) : id_base{ id } {}  \
        constexpr name() : id_base { 0 } {}               \
    };

#else
#define DEFINE_TYPED_ID(name) using id::id_type;
#endif

}
