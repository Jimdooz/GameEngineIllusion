#pragma once

/*
	/!\ DO NOT INCLUDE THIS FILE, INCLUDE "CommonHeader.h" INSTEAD
*/

constexpr f32 PI = 3.14159265358979323846;
constexpr f32 EPSILON = std::numeric_limits<double>::epsilon();

constexpr bool NEAR_EPSILON(f32 v) { return v < EPSILON || v > -EPSILON; }

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

//Vectors
using Vec2 = glm::vec2;
using Vec3 = glm::vec3;
using Quaternion = glm::quat;

//Matrices
using Mat2x2 = glm::mat2x2;
using Mat3x3 = glm::mat3x3;
using Mat4x4 = glm::mat4x4;