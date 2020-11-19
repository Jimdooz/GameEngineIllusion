#include "IllusionEngine.h"

namespace illusion::window {
	int width=0; 
	int height=0;
	std::string title="Unitialized";
	GLFWwindow* glfwWindow=nullptr;

	void Create(const int _width, const int _height, const char* _title) {
		width = _width;
		height = _height;
		title = _title;
		ILSENGINE_INTERNAL_INFO("Init GLFW");
		glfwSetErrorCallback([](int error, const char* description)
			{
				ILSENGINE_INTERNAL_ERROR("GLFW error - ", error, " : ", description);
			});
		if (!glfwInit()) {
			ILSENGINE_INTERNAL_ERROR("Failed Initializing GLFW");
		}
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
		glfwWindow = glfwCreateWindow(_width, _height, _title, NULL, NULL);
		if (!glfwWindow)
		{
			ILSENGINE_INTERNAL_ERROR("Failed to Create GLFW Window");
		}
		glfwMakeContextCurrent(glfwWindow);
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			ILSENGINE_INTERNAL_ERROR("Failed to initialize GLAD");
		}
		ILSENGINE_INTERNAL_INFO("GLAD initialized - OpenGL version : ", GLVersion.major, ".", GLVersion.minor);
		glfwSetWindowCloseCallback(glfwWindow, [](GLFWwindow* _glfwWindow)
			{
				ILSENGINE_INTERNAL_INFO("Closing Window");
			});
	}
	void Destroy()
	{
		ILSENGINE_INTERNAL_INFO("Destoying GLFW Window");
		glfwDestroyWindow(glfwWindow);
		ILSENGINE_INTERNAL_INFO("GLFW Window Destroyed");
		ILSENGINE_INTERNAL_INFO("Terminating GLFW");
		glfwTerminate();
		ILSENGINE_INTERNAL_INFO("GLFW Terminated");
	}
	void Update()
	{
		glfwMakeContextCurrent(glfwWindow);
		glfwPollEvents();
	}
	bool ShouldClose(){
		return glfwWindowShouldClose(glfwWindow);
	}
}