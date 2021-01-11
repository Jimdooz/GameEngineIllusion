#include "IllusionEngine.h"
#include "Input.h"
namespace illusion {
	int Window::width = 0;
	int Window::height = 0;
	int Window::last_width = 0;
	int Window::last_height = 0;
	int Window::posX = 0;
	int Window::posY = 0;
	int Window::last_posX = 0;
	int Window::last_posY = 0;
	std::string Window::title = "Unitialized";
	GLFWwindow* Window::glfwWindow = nullptr;
	bool Window::shouldClose = false;
	bool Window::fullscreen = false;

	void Window::Create(const int _width, const int _height, const char* _title) {
		width = _width;
		height = _height;
		last_width = _width;
		last_height = _height;		
		
		title = _title;

		fullscreen = false;
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
		glfwGetWindowPos(glfwWindow, &posX, &posY);
		last_posX = posX;
		last_posY = posY;

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
			last_width = width;
			last_height = height;
			width = _width;
			height = _height;
			glViewport(0, 0, width, height);
			INTERNAL_INFO("WINDOW RESIZED : ", width, height)
		});
		glfwSetWindowPosCallback(glfwWindow, [](GLFWwindow* _glfwWindow, int xpos, int ypos) {
			last_posX = posX;
			last_posY = posY;
			posX = xpos;
			posY = ypos;
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
	GLFWmonitor* get_current_monitor(GLFWwindow* window)
	{
		int nmonitors, i;
		int wx, wy, ww, wh;
		int mx, my, mw, mh;
		int overlap, bestoverlap;
		GLFWmonitor* bestmonitor;
		GLFWmonitor** monitors;
		const GLFWvidmode* mode;

		bestoverlap = 0;
		bestmonitor = NULL;

		glfwGetWindowPos(window, &wx, &wy);
		glfwGetWindowSize(window, &ww, &wh);
		monitors = glfwGetMonitors(&nmonitors);

		for (i = 0; i < nmonitors; i++) {
			mode = glfwGetVideoMode(monitors[i]);
			glfwGetMonitorPos(monitors[i], &mx, &my);
			mw = mode->width;
			mh = mode->height;

			overlap =
				std::max(0, std::min(wx + ww, mx + mw) - std::max(wx, mx)) *
				std::max(0, std::min(wy + wh, my + mh) - std::max(wy, my));

			if (bestoverlap < overlap) {
				bestoverlap = overlap;
				bestmonitor = monitors[i];
			}
		}

		return bestmonitor;
	}
	void Window::SetFullScreen(bool _fullscreen) {
		if(fullscreen==_fullscreen){
			INTERNAL_WARN("Window Mode already Set");
			return;
		}
		GLFWmonitor* glfwMonitor = glfwGetPrimaryMonitor();//get_current_monitor(glfwWindow);
		const GLFWvidmode* vidMode = glfwGetVideoMode(glfwMonitor);

		INTERNAL_INFO("CHANGE Window MODE");
		if (_fullscreen) {
			int posX, posY;
			glfwGetMonitorPos(glfwMonitor, &posX, &posY);
			INTERNAL_INFO("FullScreen MODE");			
			glfwSetWindowMonitor(glfwWindow, glfwMonitor, posX, posY, vidMode->width, vidMode->height, vidMode->refreshRate);
		}
		else {
			INTERNAL_INFO("Windowed MODE");
			glfwSetWindowMonitor(glfwWindow, NULL, 0, 0, last_width, last_height, vidMode->refreshRate);
			glfwSetWindowAttrib(glfwWindow, GLFW_DECORATED, GLFW_TRUE);
			glfwSetWindowPos(glfwWindow, last_posX, last_posY);
		}
		fullscreen = _fullscreen;
	}

}
