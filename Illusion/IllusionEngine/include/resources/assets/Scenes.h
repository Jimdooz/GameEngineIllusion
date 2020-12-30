#pragma once
#include <streambuf>
#include <sstream>

#include "ecs/Scene.h"
#include "resources/DataConvertor.h"
#include "resources/system/Json.h"
using json = illusion::json;

namespace illusion::resources::assets {

	void LoadScene(illusion::ecs::Scene& scene, json& jsonDatas) {
		scene.Reset();
		/*for (u32 i = 0; i < jsonDatas["Components"].size(); i++) {
			std::stringstream sstream((std::string)jsonDatas["Components"][i]);
			size_t result; sstream >> result;
			scene.UseComponent(result);
		}
		for (u32 i = 0; i < jsonDatas["Systems"].size(); i++) {
			std::stringstream sstream((std::string)jsonDatas["Systems"][i]);
			size_t result; sstream >> result;
			scene.UseSystem(result);
		}*/
		for (auto const& [key, val] : ecs::Component::AllComponents) {
			scene.UseComponent(key);
		}
		for (auto const& [key, val] : ecs::System::AllSystems) {
			scene.UseSystem(key);
		}

		for (u32 i = 0; i < jsonDatas["Entity"]["id"].size(); i++) {
			scene.CreateEntity(jsonDatas["Entity"]["id"][i]);
		}
		//Load Components Datas
		for (u32 i = 0; i < jsonDatas["Components"].size(); i++) {
			std::string componentHashS = jsonDatas["Components"][i];
			std::stringstream sstream(componentHashS);
			size_t componentHash; sstream >> componentHash;

			json componentElements = jsonDatas["Entity"]["Components"][componentHashS];
			ecs::Component* comp = scene.GetComponentSystem(componentHash);
			for (auto& el : componentElements.items()) {
				ecs::entity_id id = ecs::entity_id(std::stoi(el.key()));
				if (componentHash != typeid(ecs::core::Transform).hash_code()) scene.EntityAddComponent(id, componentHash);
				for (u32 j = 0; j < comp->publicDatas.size(); j++) {
					ecs::component_id componentId = comp->getIndex(id);
					if (componentId == ecs::id::invalid_id || componentElements[el.key()][comp->publicDatas[j].name].is_null()) continue;
					resources::JsonConvertor::ImportFromJSONFromArray(componentElements[el.key()][comp->publicDatas[j].name], comp->publicDatas[j].data, comp->publicDatas[j].type, componentId);
				}
			}
		}
	}

	json ExportScene(illusion::ecs::Scene& scene) {
		json jsonScene = {};
		jsonScene["Components"] = json::array();
		jsonScene["Systems"] = json::array();

		for (auto const& [key, val] : scene.components) {
			jsonScene["Components"].push_back(std::to_string(key));
		}
		for (auto const& [key, val] : scene.systems) {
			jsonScene["Systems"].push_back(std::to_string(key));
		}

		jsonScene["Entity"] = json::object();
		jsonScene["Entity"]["Components"] = json::object();
		jsonScene["Entity"]["id"] = json::array();

		json& components = jsonScene["Entity"]["Components"];

		for (auto const& [key, val] : scene.components) {
			std::string keyS = std::to_string(key);
			components[keyS] = json::object();
			for (u32 i = 0; i < val->ToEntity.size(); i++) {
				components[keyS][std::to_string(ecs::id::Index(val->ToEntity[i]))] = json::object();
				json& componentElement = jsonScene["Entity"]["Components"][keyS][std::to_string(ecs::id::Index(val->ToEntity[i]))];
				for (u32 j = 0; j < val->publicDatas.size(); j++) {
					ecs::entity_id entity = scene.entities.GetId(ecs::id::Index(val->ToEntity[i]));
					if (ecs::id::IsValid(entity)) {
						ecs::component_id componentId = val->getIndex(entity);
						componentElement[val->publicDatas[j].name] = illusion::resources::JsonConvertor::ExportToJSONFromArray(val->publicDatas[j].data, val->publicDatas[j].type, componentId);
					}
					else {
						componentElement[val->publicDatas[j].name] = nullptr;
					}
				}
			}
		}

		for (u32 i = 0; i < scene.entities.m_entities.size(); i++) {
			if (scene.entities.IsAliveAtIndex(ecs::entity_id(i))) jsonScene["Entity"]["id"].push_back(i);
		}

		return jsonScene;
	}

}