#pragma once
#include "common/CommonHeaders.h"

/*
 * FONCTIONNEMENT GLOBAL
 */

namespace illusion {
	class Window {
	public:
		static int width, height;
		static std::string title;
		static GLFWwindow* glfwWindow;
		static bool shouldClose;
		/**
		 * @brief Permet la création d'une fenêtre
		 *
		 * Je suis une description un peu plus détaillé pour le coup
		 * Mais est ce que les gens font vraiment attention à moi :'C
		 *
		 * @param width : la largeur de la fenêtre
		 * @param height : la hauteur de la fenêtre
		 * @param title : le titre de la fenêtre
		 * @return void
		 */
		static void Create(const int width, const int height, const char* title);

		static void Update();
		static void Destroy();
		static void Close();
	};
}