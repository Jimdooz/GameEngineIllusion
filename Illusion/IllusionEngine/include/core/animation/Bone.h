#pragma once

#include "common/CommonHeaders.h"
#include "ecs/id.h"

constexpr u32 NUM_BONES_PER_VERTEX = 4;
constexpr u32 NUM_BONES_PER_MESH = 128;

namespace illusion::core::animation {

	struct Bone {
		Mat4x4 offset;
		std::string relativePath; //The relative path of the entity on the scene hierarchy pointed by the channel
		illusion::ecs::entity_id id;
	};

	struct VertexBoneData {
		GLuint boneIds[NUM_BONES_PER_VERTEX] = { 0 };
		GLfloat weights[NUM_BONES_PER_VERTEX] = { 0 };
	};

}