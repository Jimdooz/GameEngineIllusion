#pragma once

#include "utilities/Utilities.h"
#include "common/CommonHeaders.h"

#include "resources/system/Json.h"
using json = illusion::json;

/**
 * S�ries de fonction permettant de faciliter le syst�me import / export de donn�es sous forme de json
 * 
 * Le principe �tant qu'il est possible de d�clarer ses propres convertisseurs
 * en cr�ant un objet avec diff�rents callback import / export et un type de donn�e qu'il acceptera
 * 
 * Pour des soucis pratique on augmentera ce syst�me avec la possibilit� d'importer et exporter une donn�e se trouvant dans un tableau du type que l'on veut convertir
 * Comme l'ECS r�alis� dans cet engine se retrouve souvant dans cette situation
 * 
 * Le choix d'utiliser le hash_code ( via typeid(T).hash_code() ) est qu'il permet de r�aliser une pseudo solution du fait que c++ ne peut pas s'introspecter par d�faut
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
		 * Fonction pour g�n�rer un convertisseur JSON
		 * @param T le type accept� par le convertisseur
		 * @param exportJSON fonction de callback pour exporter une donn�e en JSON
		 * @param importJSON fonction de callback pour importer une donn�e provenant d'un JSON
		 * @param convertJSON fonction permettant de retourner la valeur � l'index d'un tableau contenant le type acceptable
		 */
		template<typename T> static JsonConvertor Create(exportJsonPtr exportJSON, importJsonPtr importJSON, convertArrayJsonPtr convertJSON) {
			JsonConvertor j{ typeid(T).hash_code(), typeid(illusion::util::Array<T>).hash_code(), exportJSON, importJSON, convertJSON };
			JsonConvertor::Convertor.push_back(j);
			return j;
		}

		/**
		 * Fonction permettant de g�n�rer les convertisseurs par d�faut
		 * /!\ ne doit �tre appel� qu'une seule fois
		 */
		static void Initialize();

		/**
		 * Fonction permettant d'exporter une donn�e en JSON
		 * @param data le pointeur vers la donn�e � modifier
		 * @param type le hash_code du type de la donn�e -> peut �tre obtenue avec typeid(T).hash_code()
		 */
		static json ExportToJSON(void* data, const size_t& type) {
			for (u32 i = 0; i < JsonConvertor::Convertor.size(); i++) {
				if (type == JsonConvertor::Convertor[i].type)
					return JsonConvertor::Convertor[i].exportJSON(data);
			}

			return nullptr;
		}

		/**
		 * Fonction permettant d'importer une donn�e provenant d'un JSON
		 * @param data le json contenant exclusivement les �lements n�cessaire au remplissage de l'importation
		 * @param to le receptacle qui sera remplis lors de l'importation
		 * @param type le hash_code du type de la donn�e -> peut �tre obtenue avec typeid(T).hash_code()
		 */
		static void ImportFromJSON(json& data, void* to, const size_t& type) {
			for (u32 i = 0; i < JsonConvertor::Convertor.size(); i++) {
				if (type == JsonConvertor::Convertor[i].type)
					return JsonConvertor::Convertor[i].importJSON(data, to);
			}
		}

		/**
		 * Fonction permettant d'exporter une donn�e en JSON
		 * la donn�e pass� en param�tre devra �tre une liste contenant la donn�e que l'on voudra exporter
		 * @param data la liste de donn�e de m�me type
		 * @param type le hash_code du type de la donn�e -> peut �tre obtenue avec typeid(T).hash_code()
		 * @param index la position de l'�l�ment que l'on veut exporter
		 */
		static json ExportToJSONFromArray(void* data, const size_t& type, u32 index) {
			for (u32 i = 0; i < JsonConvertor::Convertor.size(); i++) {
				if (type == JsonConvertor::Convertor[i].typeArray)
					return JsonConvertor::Convertor[i].exportJSON(JsonConvertor::Convertor[i].convertJSON(data, index));
			}

			return nullptr;
		}

		/**
		 * Fonction permettant d'importer une donn�e provenant d'un JSON
		 * la donn�e pass� en param�tre devra �tre une liste contenant la donn�e que l'on voudra exporter
		 * @param data le json contenant exclusivement les �lements n�cessaire au remplissage de l'importation
		 * @param type le hash_code du type de la donn�e -> peut �tre obtenue avec typeid(T).hash_code()
		 * @param index la position de l'�l�ment que l'on veut exporter
		 */
		static void ImportFromJSONFromArray(json& data, void* to, const size_t& type, u32 index) {
			for (u32 i = 0; i < JsonConvertor::Convertor.size(); i++) {
				if (type == JsonConvertor::Convertor[i].typeArray)
					return JsonConvertor::Convertor[i].importJSON(data, JsonConvertor::Convertor[i].convertJSON(to, index));
			}
		}
	};
}