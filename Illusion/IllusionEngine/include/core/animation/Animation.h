#pragma once
#include "common/CommonHeaders.h"

namespace illusion::core::animation {

	struct TimeAnimation {
		f32 currentTime;
		f32 duration;
	};

	template<typename T> struct Key {
		T value;
		f32 time;
	};

	struct Channel {
		std::string relativePath; //The relative path of the entity on the scene hierarchy pointed by the channel
		ecs::entity_id id;
		
		util::Array<Key<Vec3>> positionKeys;
		util::Array<Key<Quaternion>> rotationKeys;
		util::Array<Key<Vec3>> scaleKeys;

		Vec3 computedPosition{ 0, 0, 0 };
		Quaternion computedRotation{ 1, 0, 0, 0 };
		Vec3 computedScale{1, 1, 1};

		void ComputeTransformations(TimeAnimation& time) {
			if (positionKeys.size() > 0) {
				size_t nearIndex = GetNearKey(time, positionKeys);
				auto A = positionKeys[nearIndex];
				auto B = positionKeys[(nearIndex + 1) % positionKeys.size()];
				f32 interpolation = ComputeInterpolation(time, A, B);
				computedPosition = glm::mix(A.value, B.value, interpolation);
			}
			if (rotationKeys.size() > 0) {
				size_t nearIndex = GetNearKey(time, rotationKeys);
				auto A = rotationKeys[nearIndex];
				auto B = rotationKeys[(nearIndex + 1) % rotationKeys.size()];
				f32 interpolation = ComputeInterpolation(time, A, B);
				computedRotation = glm::mix(A.value, B.value, interpolation);
			}
			if (scaleKeys.size() > 0) {
				size_t nearIndex = GetNearKey(time, scaleKeys);
				auto A = scaleKeys[nearIndex];
				auto B = scaleKeys[(nearIndex + 1) % scaleKeys.size()];
				f32 interpolation = ComputeInterpolation(time, A, B);
				computedScale = glm::mix(A.value, B.value, interpolation);
			}
		}

		// Compute a number between 0 and 1 to interpolate between key A & B
		// ---- A --|--- B ----
		// -----B ------ A --|-
		template<typename T> f32 ComputeInterpolation(const  TimeAnimation& time, const Key<T> &A, Key<T> const &B) {
			f32 distance = A.time <= B.time ? B.time - A.time : time.duration - A.time + B.time; //Compute time distance between 2 keys
			if (distance <= 0) return 0; //No interpolation or Same Frame
			if (A.time <= B.time) return (time.currentTime - A.time) / distance; //Normal case
			//Case when A > B
			if(time.currentTime >= A.time) return (time.currentTime - A.time) / distance; //After A
			return (time.currentTime + time.duration - A.time) / distance;
		}

		template<typename T> size_t GetNearKey(const TimeAnimation &time, const util::Array<Key<T>> &keys) {
			size_t size = keys.size();
			if(keys[0].time > time.currentTime) return size - 1;
			for (size_t i = 0; i < size; ++i) {
				if (keys[i].time == time.currentTime) return i;
				if (keys[i].time > time.currentTime) return i > 0 ? i - 1 : i;
			}
			return 0;
		}
	};

	struct Animation  {
		std::string name;
		f32 duration;
		util::Array<Channel> channels;
		bool idsComputed = false;
		bool loop = true;
	};

	struct AnimatorElement {
		util::Array<Animation> animations;
		size_t currentAnimation=0;
		f32 currentTime=0.0f;
		f32 speed=1.0f;
		bool stop=true;

		inline bool HasAnimations() { return animations.size() > 0; }
		void UpdateTime(float deltaTime) {
			currentTime += deltaTime;
			if (!HasAnimations()) return;
			while (currentTime > animations[currentAnimation].duration) {
				if(animations[currentAnimation].loop) currentTime -= animations[currentAnimation].duration;
				else currentTime = animations[currentAnimation].duration;
			}
		}
	};
}