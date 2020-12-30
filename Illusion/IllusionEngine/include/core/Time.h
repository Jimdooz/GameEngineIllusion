#pragma once

#include "common/CommonHeaders.h"

namespace illusion {

	/**
	 * Structure repr�sentant le Temps dans le Game Engine
	 */

	struct Time {
		static f32 deltaTime;			// Le temps �coul� entre deux frames
		static f32 unscaledDeltaTime;	// Le temps �coul� entre deux frames non d�pendant du timeScale
		static f32 time;				// Le temps �coul� depuis le d�but du jeux
		static f32 unscaledTime;		// Le temps �coul� depuis le d�but du jeux non d�pendant du timeScale
		static f32 timeScale;			// La vitesse du temps ( permet de faire des slow motion )

		static f32 fixedDeltaTime;
		static f32 unscaledFixedDeltaTime;

		static u8 tick;					// Compteur permettant de r�aliser des optimisations pour les calculs ( boucle de 0 � 255 exclus )
		static f32 previousFrame;		// Valeur du temps de la derni�re frame

		/**
		 * Permet d'initialiser le temps
		 */
		static void Init() {
			time = 0.0f;
			deltaTime = 0.0f;

			unscaledTime = 0.0f;
			unscaledDeltaTime = 0.0f;

			fixedDeltaTime = 1.0 / 50.0;
			unscaledFixedDeltaTime = 1.0 / 50.0;

			timeScale = 1.0f;
			tick = 0;
			previousFrame = 0.0f;
		}

		/**
		 * Fonction permettant de mettre � jour les donn�es du temps
		 * Doit �tre appel� en d�but de boucle du jeux
		 */
		static void UpdateTime(f32 frame) {
			unscaledDeltaTime = frame - previousFrame;
			deltaTime = unscaledDeltaTime * timeScale;

			unscaledTime += unscaledDeltaTime;
			time += deltaTime;

			fixedDeltaTime = unscaledFixedDeltaTime * timeScale;

			previousFrame = frame;
			tick = (tick + 1) % 255;
		}
	};
}