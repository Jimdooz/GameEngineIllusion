#include "resources/DataConvertor.h"

#include "ecs/Ecs.h"
#include "ecs/Entity.h"

#include <string>

#define TYPE_ID(T) typeid(T).hash_code()

namespace illusion::resources{

	#define JSON_EXPORT [](void* data)->json
	#define JSON_IMPORT [](json &data, void* to)
	#define JSON_VECTOR_CONVERTOR(TYPE) [](void* data, u32 i)-> void* { return &((*((illusion::util::Array< TYPE >*)data))[i]); }

	illusion::util::Array<JsonConvertor> JsonConvertor::Convertor;

	template<typename T> size_t hashOf() { return typeid(T).hash_code(); }

	void JsonConvertor::Initialize() {
		//size_t
		JsonConvertor::Create<size_t>(
			JSON_EXPORT{
				size_t& rdata = *((size_t*)data);
				json value = rdata;
				return value;
			},
			JSON_IMPORT{
				size_t* rto = (size_t*)to;
				*rto = data;
			},
			JSON_VECTOR_CONVERTOR(size_t)
			);
		//Boolean
		JsonConvertor::Create<boolean>(
			JSON_EXPORT{
				boolean& rdata = *((boolean*)data);
				json value = rdata;
				return value;
			},
			JSON_IMPORT{
				boolean* rto = (boolean*)to;
				*rto = data;
			},
			JSON_VECTOR_CONVERTOR(boolean)
		);
		//String
		JsonConvertor::Create<std::string>(
			JSON_EXPORT{
				std::string& rdata = *((std::string*)data);
				json array = rdata;
				return array;
			},
			JSON_IMPORT{
				std::string* rto = (std::string*)to;
				*rto = data;
			},
			JSON_VECTOR_CONVERTOR(std::string)
			);
		//Vector4
		JsonConvertor::Create<Vec2>(
			JSON_EXPORT{
				Vec2& rdata = *((Vec2*)data);
				json array = json::array();
				array.push_back(rdata.x);
				array.push_back(rdata.y);
				return array;
			},
			JSON_IMPORT{
				Vec2* rto = (Vec2*)to;
				rto->x = data[0];
				rto->y = data[1];
			},
			JSON_VECTOR_CONVERTOR(Vec2)
			);
		//Vector3
		JsonConvertor::Create<Vec3>(
			JSON_EXPORT {
				Vec3 &rdata = *((Vec3*)data);
				json array = json::array();
				array.push_back(rdata.x);
				array.push_back(rdata.y);
				array.push_back(rdata.z);
				return array;
			},
			JSON_IMPORT{
				Vec3* rto = (Vec3*)to;
				rto->x = data[0];
				rto->y = data[1];
				rto->z = data[2];
			},
			JSON_VECTOR_CONVERTOR(Vec3)
		);
		//Vector4
		JsonConvertor::Create<Vec4>(
			JSON_EXPORT{
				Vec4& rdata = *((Vec4*)data);
				json array = json::array();
				array.push_back(rdata.x);
				array.push_back(rdata.y);
				array.push_back(rdata.z);
				array.push_back(rdata.w);
				return array;
			},
			JSON_IMPORT{
				Vec4* rto = (Vec4*)to;
				rto->x = data[0];
				rto->y = data[1];
				rto->z = data[2];
				rto->w = data[3];
			},
			JSON_VECTOR_CONVERTOR(Vec4)
			);
		//Quaternion
		JsonConvertor::Create<Quaternion>(
			JSON_EXPORT{
				Quaternion& rdata = *((Quaternion*)data);
				json array = json::array();
				array.push_back(rdata.w);
				array.push_back(rdata.x);
				array.push_back(rdata.y);
				array.push_back(rdata.z);
				return array;
			},
			JSON_IMPORT{
				Quaternion* rto = (Quaternion*)to;
				rto->w = data[0];
				rto->x = data[1];
				rto->y = data[2];
				rto->z = data[3];
			},
			JSON_VECTOR_CONVERTOR(Quaternion)
		);
		//Entity_id
		JsonConvertor::Create<illusion::ecs::entity_id>(
			JSON_EXPORT{
				illusion::ecs::entity_id& rdata = *((illusion::ecs::entity_id*)data);
				if (!ecs::id::IsValid(rdata)) return nullptr;
				json id = ecs::id::Index(rdata);
				return id;
			},
			JSON_IMPORT{
				illusion::ecs::entity_id* rto = (illusion::ecs::entity_id*)to;
				if (data.is_null()) *rto = (ecs::entity_id)ecs::id::invalid_id;
				else *rto = ecs::entity_id(data);
			},
			JSON_VECTOR_CONVERTOR(illusion::ecs::entity_id)
		);
		//List<Entity_id>
		JsonConvertor::Create<illusion::util::Array<illusion::ecs::entity_id>>(
			JSON_EXPORT{
				illusion::util::Array<illusion::ecs::entity_id>& rdata = *((illusion::util::Array<illusion::ecs::entity_id>*)data);
				json idJson = json::array();
				for (u32 i = 0; i < rdata.size(); i++) {
					idJson.push_back(JsonConvertor::ExportToJSON(&rdata[i], TYPE_ID(illusion::ecs::entity_id)) );
				}
				return idJson;
			},
			JSON_IMPORT{
				illusion::util::Array<illusion::ecs::entity_id>* rto = (illusion::util::Array<illusion::ecs::entity_id>*)to;
				rto->clear();
				for (u32 i = 0; i < data.size(); i++) {
					rto->push_back(illusion::ecs::entity_id{0});
					JsonConvertor::ImportFromJSON(data[i], &((*rto)[i]), TYPE_ID(illusion::ecs::entity_id));
				}
			},
			JSON_VECTOR_CONVERTOR(illusion::util::Array<illusion::ecs::entity_id>)
		);
		//f32
		JsonConvertor::Create<f32>(
			JSON_EXPORT{
				f32& rdata = *((f32*)data);
				json idJson = rdata;
				return idJson;
			},
			JSON_IMPORT{
				f32* rto = (f32*)to;
				*rto = data;
			},
			JSON_VECTOR_CONVERTOR(f32)
		);
		//f16
		JsonConvertor::Create<f64>(
			JSON_EXPORT{
				f64& rdata = *((f64*)data);
				json idJson = rdata;
				return idJson;
			},
			JSON_IMPORT{
				f64* rto = (f64*)to;
				*rto = data;
			},
			JSON_VECTOR_CONVERTOR(f64)
		);
	}

}