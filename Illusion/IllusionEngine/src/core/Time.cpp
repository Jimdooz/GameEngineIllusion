#include "core/Time.h"

namespace illusion {
	f32 Time::deltaTime = 0.0;
	f32 Time::unscaledDeltaTime = 0.0;
	f32 Time::time = 0.0;
	f32 Time::unscaledTime = 0.0;
	f32 Time::timeScale = 0.0;

	f32 Time::fixedDeltaTime = 0.0;
	f32 Time::unscaledFixedDeltaTime = 0.0;

	u8 Time::tick = 0;
	f32 Time::previousFrame = 0.0;
}