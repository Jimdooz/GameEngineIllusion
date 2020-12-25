#pragma once

#include "utilities/Utilities.h"
#include "common/CommonHeaders.h"

#include "resources/system/Json.h"
using json = illusion::json;

namespace illusion::resources {

	using exportJsonPtr = json(*)(void*);
	using importJsonPtr = void (*)(json&, void*);
	using convertArrayJsonPtr = void* (*)(void*, u32 i);

	struct JsonConvertor {
		size_t type;						// Hash for data catch
		size_t typeArray;					// Hash for data catch array
		exportJsonPtr exportJSON;			// Callback export
		importJsonPtr importJSON;			// Callback import
		convertArrayJsonPtr convertJSON;	// Callback convert array

		static illusion::util::Array<JsonConvertor> Convertor;

		template<typename T> static JsonConvertor Create(exportJsonPtr exportJSON, importJsonPtr importJSON, convertArrayJsonPtr convertJSON) {
			JsonConvertor j{ typeid(T).hash_code(), typeid(illusion::util::Array<T>).hash_code(), exportJSON, importJSON, convertJSON };
			JsonConvertor::Convertor.push_back(j);
			return j;
		}

		static void Initialize();

		static json ExportToJSON(void* data, const size_t& type) {
			for (u32 i = 0; i < JsonConvertor::Convertor.size(); i++) {
				if (type == JsonConvertor::Convertor[i].type)
					return JsonConvertor::Convertor[i].exportJSON(data);
			}

			return nullptr;
		}

		static void ImportFromJSON(json& data, void* to, const size_t& type) {
			for (u32 i = 0; i < JsonConvertor::Convertor.size(); i++) {
				if (type == JsonConvertor::Convertor[i].type)
					return JsonConvertor::Convertor[i].importJSON(data, to);
			}
		}

		static json ExportToJSONFromArray(void* data, const size_t& type, u32 index) {
			for (u32 i = 0; i < JsonConvertor::Convertor.size(); i++) {
				if (type == JsonConvertor::Convertor[i].typeArray)
					return JsonConvertor::Convertor[i].exportJSON(JsonConvertor::Convertor[i].convertJSON(data, index));
			}

			return nullptr;
		}

		static void ImportFromJSONFromArray(json& data, void* to, const size_t& type, u32 index) {
			for (u32 i = 0; i < JsonConvertor::Convertor.size(); i++) {
				if (type == JsonConvertor::Convertor[i].typeArray)
					return JsonConvertor::Convertor[i].importJSON(data, JsonConvertor::Convertor[i].convertJSON(to, index));
			}
		}
	};
}