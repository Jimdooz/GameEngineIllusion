#include "IllusionEngine.h"
#include "Input.h"
namespace illusion {
	int Window::width = 0;
	int Window::height = 0;
	std::string Window::title = "Unitialized";
	GLFWwindow* Window::glfwWindow = nullptr;
	bool Window::shouldClose = false;

	void Window::Create(const int _width, const int _height, const char* _title) {
		width = _width;
		height = _height;
		title = _title;
		INTERNAL_INFO("Init GLFW");
		glfwSetErrorCallback([](int error, const char* description)
		{
			INTERNAL_ERR("GLFW error - ", error, " : ", description);
		});
		if (!glfwInit()) {
			INTERNAL_ERR("Failed Initializing GLFW");
		}
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
		glfwWindow = glfwCreateWindow(_width, _height, _title, NULL, NULL);
		if (!glfwWindow)
		{
			INTERNAL_ERR("Failed to Create GLFW Window");
		}
		glfwMakeContextCurrent(glfwWindow);
		glfwSetWindowCloseCallback(glfwWindow, [](GLFWwindow* _glfwWindow)
		{
			shouldClose = true;
			INTERNAL_INFO("Closing Window");
		});
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			INTERNAL_ERR("Failed to initialize GLAD");
		}
		glfwSetFramebufferSizeCallback(glfwWindow, [](GLFWwindow* _glfwWindow, int _width, int _height)
		{
			width = _width;
			height = _height;
			glViewport(0, 0, width, height);
			INTERNAL_INFO("WINDOW RESIZED : ", width, height)
		});
		INTERNAL_INFO("GLAD initialized - OpenGL version : ", GLVersion.major, ".", GLVersion.minor);
		glfwSetFramebufferSizeCallback(glfwWindow, [](GLFWwindow* _glfwWindow, int _width, int _height)
		{
			width = _width;
			height = _height;
			glViewport(0, 0, width, height);
			INTERNAL_INFO("WINDOW RESIZED : ",width,height)
		});
		glViewport(0, 0, width, height);
		Input::Init();
	}
	void Window::Destroy()
	{
		INTERNAL_INFO("Destoying GLFW Window");
		glfwDestroyWindow(glfwWindow);
		INTERNAL_INFO("GLFW Window Destroyed");
		INTERNAL_INFO("Terminating GLFW");
		glfwTerminate();
		INTERNAL_INFO("GLFW Terminated");
	}
	void Window::Update()
	{
		glfwMakeContextCurrent(glfwWindow);
		glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		Input::Update();
		glfwSwapBuffers(glfwWindow);
	}
	void Window::Close() {
		INTERNAL_INFO("Closing Window");
		shouldClose = true;
	}
}
