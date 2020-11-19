#pragma once
#include "CommonHeaders.h"

namespace IllusionEngine {
	namespace Window {		
		extern int width, height;
		extern std::string title;
		extern GLFWwindow* glfwWindow;
		void Create(const int width, const int height, const char* title);
		void Update();
		void Destroy();
		bool ShouldClose();
	}
}