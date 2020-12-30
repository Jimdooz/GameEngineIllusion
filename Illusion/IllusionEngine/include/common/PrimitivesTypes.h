#pragma once

/*
	/!\ DO NOT INCLUDE THIS FILE, INCLUDE "CommonHeader.h" INSTEAD
*/

//unsigned integers
using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

//signed integers
using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

//floating-points
using f32 = float;
using f64 = double;

//Boolean
using boolean = unsigned char;

//Vectors
using Vec2 = glm::vec2;
using Vec3 = glm::vec3;
using Vec4 = glm::vec4;
using Quaternion = glm::quat;

//Matrices
using Mat2x2 = glm::mat2x2;
using Mat3x3 = glm::mat3x3;
using Mat4x4 = glm::mat4x4;

static constexpr f32 const& PI = 3.14159265358979323846;
static constexpr f32 const& EPSILON = FLT_EPSILON;

constexpr bool NEAR_EPSILON(f32 v) { return v < EPSILON || v > -EPSILON; }
inline bool CMP(f32 x, f32 y) {
	return (fabsf((x)-(y)) <= EPSILON * fmaxf(1.0f, fmaxf(fabsf(x), fabsf(y))));
}