#pragma once

#include "utilities/Utilities.h"
#include "common/CommonHeaders.h"

#include "resources/system/Json.h"
using json = illusion::json;

/**
 * Séries de fonction permettant de faciliter le système import / export de données sous forme de json
 * 
 * Le principe étant qu'il est possible de déclarer ses propres convertisseurs
 * en créant un objet avec différents callback import / export et un type de donnée qu'il acceptera
 * 
 * Pour des soucis pratique on augmentera ce système avec la possibilité d'importer et exporter une donnée se trouvant dans un tableau du type que l'on veut convertir
 * Comme l'ECS réalisé dans cet engine se retrouve souvant dans cette situation
 * 
 * Le choix d'utiliser le hash_code ( via typeid(T).hash_code() ) est qu'il permet de réaliser une pseudo solution du fait que c++ ne peut pas s'introspecter par défaut
 */

namespace illusion::resources {

	using exportJsonPtr = json(*)(void*);					// Fonction callback pour export en JSON
	using importJsonPtr = void (*)(json&, void*);			// Fonction callbakc pour import du JSON
	using convertArrayJsonPtr = void* (*)(void*, u32 i);	// Fonction permettant de catcher un tableau du type qu'on peut accepter

	struct JsonConvertor {
		size_t type;						// Hash for data catch
		size_t typeArray;					// Hash for data catch array
		exportJsonPtr exportJSON;			// Callback export
		importJsonPtr importJSON;			// Callback import
		convertArrayJsonPtr convertJSON;	// Callback convert array

		static illusion::util::Array<JsonConvertor> Convertor;	// Liste de tous les convertisseurs existants

		/**
		 * Fonction pour générer un convertisseur JSON
		 * @param T le type accepté par le convertisseur
		 * @param exportJSON fonction de callback pour exporter une donnée en JSON
		 * @param importJSON fonction de callback pour importer une donnée provenant d'un JSON
		 * @param convertJSON fonction permettant de retourner la valeur à l'index d'un tableau contenant le type acceptable
		 */
		template<typename T> static JsonConvertor Create(exportJsonPtr exportJSON, importJsonPtr importJSON, convertArrayJsonPtr convertJSON) {
			JsonConvertor j{ typeid(T).hash_code(), typeid(illusion::util::Array<T>).hash_code(), exportJSON, importJSON, convertJSON };
			JsonConvertor::Convertor.push_back(j);
			return j;
		}

		/**
		 * Fonction permettant de générer les convertisseurs par défaut
		 * /!\ ne doit être appelé qu'une seule fois
		 */
		static void Initialize();

		/**
		 * Fonction permettant d'exporter une donnée en JSON
		 * @param data le pointeur vers la donnée à modifier
		 * @param type le hash_code du type de la donnée -> peut être obtenue avec typeid(T).hash_code()
		 */
		static json ExportToJSON(void* data, const size_t& type) {
			for (u32 i = 0; i < JsonConvertor::Convertor.size(); i++) {
				if (type == JsonConvertor::Convertor[i].type)
					return JsonConvertor::Convertor[i].exportJSON(data);
			}

			return nullptr;
		}

		/**
		 * Fonction permettant d'importer une donnée provenant d'un JSON
		 * @param data le json contenant exclusivement les élements nécessaire au remplissage de l'importation
		 * @param to le receptacle qui sera remplis lors de l'importation
		 * @param type le hash_code du type de la donnée -> peut être obtenue avec typeid(T).hash_code()
		 */
		static void ImportFromJSON(json& data, void* to, const size_t& type) {
			for (u32 i = 0; i < JsonConvertor::Convertor.size(); i++) {
				if (type == JsonConvertor::Convertor[i].type)
					return JsonConvertor::Convertor[i].importJSON(data, to);
			}
		}

		/**
		 * Fonction permettant d'exporter une donnée en JSON
		 * la donnée passé en paramètre devra être une liste contenant la donnée que l'on voudra exporter
		 * @param data la liste de donnée de même type
		 * @param type le hash_code du type de la donnée -> peut être obtenue avec typeid(T).hash_code()
		 * @param index la position de l'élément que l'on veut exporter
		 */
		static json ExportToJSONFromArray(void* data, const size_t& type, u32 index) {
			for (u32 i = 0; i < JsonConvertor::Convertor.size(); i++) {
				if (type == JsonConvertor::Convertor[i].typeArray)
					return JsonConvertor::Convertor[i].exportJSON(JsonConvertor::Convertor[i].convertJSON(data, index));
			}

			return nullptr;
		}

		/**
		 * Fonction permettant d'importer une donnée provenant d'un JSON
		 * la donnée passé en paramètre devra être une liste contenant la donnée que l'on voudra exporter
		 * @param data le json contenant exclusivement les élements nécessaire au remplissage de l'importation
		 * @param type le hash_code du type de la donnée -> peut être obtenue avec typeid(T).hash_code()
		 * @param index la position de l'élément que l'on veut exporter
		 */
		static void ImportFromJSONFromArray(json& data, void* to, const size_t& type, u32 index) {
			for (u32 i = 0; i < JsonConvertor::Convertor.size(); i++) {
				if (type == JsonConvertor::Convertor[i].typeArray)
					return JsonConvertor::Convertor[i].importJSON(data, JsonConvertor::Convertor[i].convertJSON(to, index));
			}
		}
	};
}