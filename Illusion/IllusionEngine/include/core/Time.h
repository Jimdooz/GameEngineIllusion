#pragma once

#include "common/CommonHeaders.h"

namespace illusion {

	/**
	 * Structure représentant le Temps dans le Game Engine
	 */

	struct Time {
		static f32 deltaTime;			// Le temps écoulé entre deux frames
		static f32 unscaledDeltaTime;	// Le temps écoulé entre deux frames non dépendant du timeScale
		static f32 time;				// Le temps écoulé depuis le début du jeux
		static f32 unscaledTime;		// Le temps écoulé depuis le début du jeux non dépendant du timeScale
		static f32 timeScale;			// La vitesse du temps ( permet de faire des slow motion )

		static f32 fixedDeltaTime;
		static f32 unscaledFixedDeltaTime;

		static u8 tick;					// Compteur permettant de réaliser des optimisations pour les calculs ( boucle de 0 à 255 exclus )
		static f32 previousFrame;		// Valeur du temps de la dernière frame

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
		 * Fonction permettant de mettre à jour les données du temps
		 * Doit être appelé en début de boucle du jeux
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