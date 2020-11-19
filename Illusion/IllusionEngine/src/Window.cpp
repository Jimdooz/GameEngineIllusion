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
		INTERNAL_INFO("Init GLFW");
		glfwSetErrorCallback([](int error, const char* description)
			{
				INTERNAL_ERR("GLFW error - ", error, " : ", description);
			});
		if (!glfwInit()) {
			INTERNAL_ERR("Failed Initializing GLFW");
		}
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
		glfwWindow = glfwCreateWindow(_width, _height, _title, NULL, NULL);
		if (!glfwWindow)
		{
			INTERNAL_ERR("Failed to Create GLFW Window");
		}
		glfwMakeContextCurrent(glfwWindow);
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			INTERNAL_ERR("Failed to initialize GLAD");
		}
		INTERNAL_INFO("GLAD initialized - OpenGL version : ", GLVersion.major, ".", GLVersion.minor);
		glfwSetWindowCloseCallback(glfwWindow, [](GLFWwindow* _glfwWindow)
			{
				INTERNAL_INFO("Closing Window");
			});
	}
	void Destroy()
	{
		INTERNAL_INFO("Destoying GLFW Window");
		glfwDestroyWindow(glfwWindow);
		INTERNAL_INFO("GLFW Window Destroyed");
		INTERNAL_INFO("Terminating GLFW");
		glfwTerminate();
		INTERNAL_INFO("GLFW Terminated");
	}
	void Update()
	{
		glClearColor(1.0,0.0,0.0,1.0);
		glfwMakeContextCurrent(glfwWindow);
		glfwPollEvents();

		glfwSwapBuffers(glfwWindow);
	}
	bool ShouldClose(){
		return glfwWindowShouldClose(glfwWindow);
	}
}
