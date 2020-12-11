#pragma once
#include "common/CommonHeaders.h"
namespace illusion {
	struct InputState {
		Vec2 mousePos;
		f32 wheelDelta;
		bool mouseButton[GLFW_MOUSE_BUTTON_LAST] = { 0 };
		bool key[GLFW_KEY_LAST] = { 0 };

		InputState() {
			mousePos = Vec2();
			wheelDelta = 0;
		}
	};
	class Input {
		public :
			static bool isKey(int glfwKeyId);
			static bool isKeyDown(int glfwKeyId);
			static bool isKeyUp(int glfwKeyId);
			static bool isMouse(int glfwMouseButtonId);
			static bool isMouseDown(int glfwMouseButtonId);
			static bool isMouseUp(int glfwMouseButtonId);

			static Vec2 getMousePos();
			static Vec2 getMouseDelta();
			static f32 getMouseWheelDelta();
			static void HideMouse();
			static void ShowMouse();

			//Must be initialized after Window
			static void Init();
			static void Update();
	};	
}