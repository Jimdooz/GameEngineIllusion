#include "Input.h"
#include "Window.h"
namespace illusion {
	struct InputState {
		GLFWgamepadstate gamepad = { 0 };
		Vec2 mousePos;
		f32 wheelDelta=0.0f;
		bool mouseButton[GLFW_MOUSE_BUTTON_LAST] = { 0 };
		bool key[GLFW_KEY_LAST] = { 0 };

		InputState() {
			mousePos = Vec2();
		}
	};
	static InputState currentInputState=InputState();
	static InputState previousInputState= InputState();
	
	//KEYS
	bool Input::isKey(int glfwKeyId)
	{
		if (glfwKeyId < 0 || glfwKeyId >GLFW_KEY_LAST) { INTERNAL_ERR("INPUT : UNKNOW KEY"); }
		return currentInputState.key[glfwKeyId];
	}

	bool Input::isKeyDown(int glfwKeyId)
	{
		if (glfwKeyId < 0 || glfwKeyId >GLFW_KEY_LAST) { INTERNAL_ERR("INPUT : UNKNOW KEY"); }
		return currentInputState.key[glfwKeyId] && !previousInputState.key[glfwKeyId];
	}

	bool Input::isKeyUp(int glfwKeyId)
	{
		if (glfwKeyId < 0 || glfwKeyId >GLFW_KEY_LAST) { INTERNAL_ERR("INPUT : UNKNOW KEY"); }
		return !currentInputState.key[glfwKeyId] && previousInputState.key[glfwKeyId];;
	}
	//END KEYS

	//MOUSE
	bool Input::isMouse(int glfwMouseButtonId)
	{
		if (glfwMouseButtonId < 0 || glfwMouseButtonId >GLFW_MOUSE_BUTTON_LAST) { INTERNAL_ERR("INPUT : UNKNOW MOUSE BUTTON"); }
		return currentInputState.mouseButton[glfwMouseButtonId];
	}
	bool Input::isMouseDown(int glfwMouseButtonId)
	{
		if (glfwMouseButtonId < 0 || glfwMouseButtonId >GLFW_MOUSE_BUTTON_LAST) { INTERNAL_ERR("INPUT : UNKNOW MOUSE BUTTON"); }
		return currentInputState.mouseButton[glfwMouseButtonId] && !previousInputState.mouseButton[glfwMouseButtonId];
	}
	bool Input::isMouseUp(int glfwMouseButtonId)
	{
		if (glfwMouseButtonId < 0 || glfwMouseButtonId >GLFW_MOUSE_BUTTON_LAST) { INTERNAL_ERR("INPUT : UNKNOW MOUSE BUTTON"); }
		return !currentInputState.mouseButton[glfwMouseButtonId] && previousInputState.mouseButton[glfwMouseButtonId];
	}
	Vec2 Input::getMousePos()
	{
		return currentInputState.mousePos;
	}
	Vec2 Input::getMouseDelta()
	{
		return currentInputState.mousePos - previousInputState.mousePos;
	}
	f32 Input::getMouseWheelDelta()
	{
		return currentInputState.wheelDelta;
	}
	void Input::HideMouse()
	{
		glfwSetInputMode(Window::glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
	void Input::ShowMouse()
	{
		glfwSetInputMode(Window::glfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
	//END MOUSE

	//GAMEPAD
	bool Input::isGamepadButton(int glfwGamepadButtonId)
	{
		if (glfwGamepadButtonId < 0 || glfwGamepadButtonId >GLFW_GAMEPAD_BUTTON_LAST) { INTERNAL_ERR("INPUT : UNKNOW GAMEPAD BUTTON"); }
		return currentInputState.gamepad.buttons[glfwGamepadButtonId] == GLFW_PRESS;
	}

	bool Input::isGamepadButtonDown(int glfwGamepadButtonId)
	{
		if (glfwGamepadButtonId < 0 || glfwGamepadButtonId >GLFW_GAMEPAD_BUTTON_LAST) { INTERNAL_ERR("INPUT : UNKNOW GAMEPAD BUTTON"); }
		return currentInputState.gamepad.buttons[glfwGamepadButtonId]==GLFW_PRESS && previousInputState.gamepad.buttons[glfwGamepadButtonId] == GLFW_RELEASE;
	}

	bool Input::isGamepadButtonUp(int glfwGamepadButtonId)
	{
		if (glfwGamepadButtonId < 0 || glfwGamepadButtonId >GLFW_GAMEPAD_BUTTON_LAST) { INTERNAL_ERR("INPUT : UNKNOW GAMEPAD BUTTON"); }
		return currentInputState.gamepad.buttons[glfwGamepadButtonId]== GLFW_RELEASE && previousInputState.gamepad.buttons[glfwGamepadButtonId] == GLFW_PRESS;
	}
	f32 Input::getGamepadAxis(int glfwGamepadAxisId)
	{
		if (glfwGamepadAxisId < 0 || glfwGamepadAxisId >GLFW_GAMEPAD_AXIS_LAST) { INTERNAL_ERR("INPUT : UNKNOW GAMEPAD AXIS"); }
		return currentInputState.gamepad.axes[glfwGamepadAxisId];
	}
	f32 Input::getGamepadAxisDelta(int glfwGamepadAxisId)
	{
		if (glfwGamepadAxisId < 0 || glfwGamepadAxisId >GLFW_GAMEPAD_AXIS_LAST) { INTERNAL_ERR("INPUT : UNKNOW GAMEPAD AXIS"); }
		return currentInputState.gamepad.axes[glfwGamepadAxisId] - previousInputState.gamepad.axes[glfwGamepadAxisId];
	}
	//END GAMEPAD

	void Input::Init() {
		glfwSetWindowUserPointer(Window::glfwWindow, &currentInputState);
		glfwSetCursorPosCallback(Window::glfwWindow, [](GLFWwindow* _glfwWindow, double xPos, double yPos) {
			currentInputState.mousePos = Vec2(xPos, yPos);
		});
		glfwSetMouseButtonCallback(Window::glfwWindow, [](GLFWwindow* _glfwWindow, int button, int action, int mods) {
			if (button < 0 || button >GLFW_MOUSE_BUTTON_LAST) { INTERNAL_ERR("GLFW : UNKNOW MOUSE BUTTON"); }
			currentInputState.mouseButton[button] = action != GLFW_RELEASE;
		});
		glfwSetScrollCallback(Window::glfwWindow, [](GLFWwindow * _glfwWindow, double xoffset, double yoffset) {
			currentInputState.wheelDelta = static_cast<f32>(yoffset);
		});
		glfwSetKeyCallback(Window::glfwWindow, [](GLFWwindow* _glfwWindow, int keyId, int scancode, int action, int mods) {
			if (keyId < 0 || keyId >GLFW_KEY_LAST) { INTERNAL_ERR("GLFW : UNKNOW KEY"); }
			currentInputState.key[keyId] = (action != GLFW_RELEASE);
		});
	}
	void Input::Update() {
		previousInputState = currentInputState;
		currentInputState.wheelDelta = 0;
		glfwPollEvents();
		if (glfwJoystickIsGamepad(GLFW_JOYSTICK_1))// check if joystick is connected and gamepad inputs are mapped
		{
			if (!glfwGetGamepadState(GLFW_JOYSTICK_1, &currentInputState.gamepad))//getGamepadState and check if it has no error
			{
				INTERNAL_ERR("GLFW ERROR :Can't Get GamePad state ");
			}
		}
	}
}
