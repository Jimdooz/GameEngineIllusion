#pragma once
#include "common/CommonHeaders.h"

namespace illusion::ecs::id {
    //Represent an id for identify the different elements like Objects
    // [generation][index]
    // |            |_ Represent the "real" id of the element
    // |_ Represent the generation that ecrement every time we destroy an object at this id

    // Generation are usefull to avoid pointing to a wrong element
    using id_type = u32;

    //define invalid IDs ( -1 )
    constexpr u8 u8_invalid_id{ 0xffui8 };
    constexpr u16 u16_invalid_id{ 0xffffui16 };
    constexpr u32 u32_invalid_id{ 0xffff'ffffui32 };
    constexpr u64 u64_invalid_id{ 0xffff'ffff'ffff'ffffui64 };

    //id and generation size
    constexpr u32 generation_bits { 10 };
    constexpr u32 index_bit(sizeof(id_type) * 8 - generation_bits);

    // Create a mask to get the index
    // id_type{1}                       --> 0000000000000000000001
    // id_type{1} << index_bit          --> 0000000100000000000000
    // (id_type{1} << index_bit) - 1    --> 0000000011111111111111
    constexpr id_type index_mask{ (id_type{1} << index_bit) - 1 };
    // Mask to get the generation
    constexpr id_type generation_mask{ (id_type{1} << generation_bits) - 1 };


    constexpr id_type invalid_id{ id_type{0} -1 };
    constexpr u32 min_deleted_elements { 1024 };

    //Ternary to choose a type <bool, t1, t2>
    using generation_type = std::conditional_t<generation_bits <= 15, std::conditional_t<generation_bits <= 7, u8, u16>, u32>;

    static_assert (sizeof (generation_type) * 8 >= generation_bits, "Invalid generation type size");
    static_assert (sizeof (id_type) - sizeof (generation_type) > 0, "generation_type size must fit into id_type size");

    //Generation System with Alive value Functions
    inline bool
    IsGenerationAlive(generation_type generation) {
        return generation >> generation_bits;
    }

    inline generation_type
    SetGenerationAlive(generation_type generation, bool alive) {
        generation_type aliveValue = (alive ? 1 : 0) << (generation_bits);
        return (generation & generation_mask) + aliveValue;
    }

    inline generation_type
    GetGenerationValue(generation_type generation) {
        return generation & generation_mask;
    }

    inline bool
        IsValid(id_type id) {
        return id != invalid_id;
    }

    inline id_type
    Index(id_type id){
        id_type index{ id & index_mask };
        assert(index != index_mask);
        return id & index_mask;
    }

    inline id_type
    Generation(id_type id){
        return (id >> index_bit) & generation_mask;
    }

    inline id_type SetGeneration(id_type id, id_type generation){
        if (generation >= generation_mask) generation = 0;
        return Index(id) | (generation << index_bit);
    }

    inline id_type NewGeneration(id_type id){
        id_type generation{ id::Generation(id) + 1 };
        return SetGeneration(id, generation);
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
#define DEFINE_TYPED_ID(name) using name = id::id_type;
#endif

}
