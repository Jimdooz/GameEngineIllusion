#include "Input.h"
#include "Window.h"
namespace illusion {
	static InputState currentInputState=InputState();
	static InputState previousInputState= InputState();
	
	bool Input::isKey(int glfwKeyId)
	{
		return currentInputState.key[glfwKeyId];
	}

	bool Input::isKeyDown(int glfwKeyId)
	{
		return currentInputState.key[glfwKeyId] && !previousInputState.key[glfwKeyId];
	}

	bool Input::isKeyUp(int glfwKeyId)
	{
		return !currentInputState.key[glfwKeyId] && previousInputState.key[glfwKeyId];;
	}
	bool Input::isMouse(int glfwMouseButtonId)
	{
		return currentInputState.mouseButton[glfwMouseButtonId];
	}
	bool Input::isMouseDown(int glfwMouseButtonId)
	{
		return currentInputState.mouseButton[glfwMouseButtonId] && !previousInputState.mouseButton[glfwMouseButtonId];
	}
	bool Input::isMouseUp(int glfwMouseButtonId)
	{
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
	void Input::Init() {
		glfwSetWindowUserPointer(Window::glfwWindow, &currentInputState);
		glfwSetCursorPosCallback(Window::glfwWindow, [](GLFWwindow* _glfwWindow, double xPos, double yPos) {
			currentInputState.mousePos = Vec2(xPos, yPos);
		});
		glfwSetMouseButtonCallback(Window::glfwWindow, [](GLFWwindow* _glfwWindow, int button, int action, int mods) {
			currentInputState.mouseButton[button] = action != GLFW_RELEASE;
		});
		glfwSetScrollCallback(Window::glfwWindow, [](GLFWwindow * _glfwWindow, double xoffset, double yoffset) {
			currentInputState.wheelDelta = yoffset;
		});
		glfwSetKeyCallback(Window::glfwWindow, [](GLFWwindow* _glfwWindow, int keyId, int scancode, int action, int mods) {
			if (keyId < 0) INTERNAL_WARN("GLFW : UNKNOW KEY");
			currentInputState.key[keyId] = (action != GLFW_RELEASE);
		});
	}
	void Input::Update() {
		previousInputState = currentInputState;
		currentInputState.wheelDelta = 0;
		glfwPollEvents();
	}
}
