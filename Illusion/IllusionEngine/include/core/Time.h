#pragma once

#include "common/CommonHeaders.h"

namespace illusion {

	struct Time {
		static f32 deltaTime;
		static f32 unscaledDeltaTime;
		static f32 time;
		static f32 unscaledTime;
		static f32 timeScale;

		static u8 tick;
		static f32 previousFrame;

		static void Init() {
			time = 0.0f;
			deltaTime = 0.0f;

			unscaledTime = 0.0f;
			unscaledDeltaTime = 0.0f;

			timeScale = 1.0f;
			tick = 0;
			previousFrame = 0.0f;
		}

		static void UpdateTime(f32 frame) {
			unscaledDeltaTime = frame - previousFrame;
			deltaTime = unscaledDeltaTime * timeScale;

			unscaledTime += unscaledDeltaTime;
			time += deltaTime;

			previousFrame = frame;
			tick = (tick + 1) % 255;
		}
	};
}