#pragma once
#include "common/CommonHeaders.h"
namespace illusion {
		class Input {
		public :
			//KEYS
			static bool isKey(int glfwKeyId);
			static bool isKeyDown(int glfwKeyId);
			static bool isKeyUp(int glfwKeyId);
			//END KEYS

			//MOUSE
			static bool isMouse(int glfwMouseButtonId);
			static bool isMouseDown(int glfwMouseButtonId);
			static bool isMouseUp(int glfwMouseButtonId);
			static Vec2 getMousePos();
			static Vec2 getMouseDelta();
			static f32 getMouseWheelDelta();
			static void HideMouse();
			static void ShowMouse();
			//END MOUSE
			
			//GAMEPAD
			static bool isGamepadButton(int glfwGamepadButtonId);
			static bool isGamepadButtonDown(int glfwGamepadButtonId);
			static bool isGamepadButtonUp(int glfwGamepadButtonId);
			static f32 getGamepadAxis(int glfwGamepadAxisId);
			static f32 getGamepadAxisDelta(int glfwGamepadAxisId);

			//initialized after Window
			static void Init();
			//inside window Update
			static void Update();
	};	
}