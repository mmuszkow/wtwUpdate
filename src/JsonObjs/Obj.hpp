#pragma once

#include "cpp/Json.h"
#include "bson_decoder.h"
#include <vector>

namespace wtwUpdate { 
	namespace json {
		class Obj {
			wtw::CJson* _json;
			bds_node* _bson;
		public:
			Obj() : _json(NULL), _bson(NULL) { }
			Obj(wtw::CJson* json) : _json(json), _bson(NULL) { }
			Obj(bds_node* bson) : _json(NULL), _bson(bson) { }
			virtual ~Obj() { }
		protected:
			__int64 getInt(const char* key, __int64 defVal = 0) const;
			std::string getStr(const char* key, const std::string& defVal = "") const;
			std::vector<std::string> getStrArray(const char* key) const;

			// some linking problems when in .cpp
			template <class T>
			std::vector<T> getObjArray(const char* key) const {
				// JSON
				if (_json) {
					wtw::CJson* jsonVal = _json->find(key);

					if (!jsonVal)
						return std::vector<T>();

					if (jsonVal->isObject()) {
						std::vector<T> single;
						single.push_back(T(jsonVal));
						return single;
					}

					if (jsonVal->isArray()) {
						std::vector<T> arr;
						size_t len = jsonVal->size();
						for (size_t i = 0; i < len; i++)
							arr.push_back(T(jsonVal->getAt(i)));
						return arr;
					}

					return std::vector<T>();
				}

				// BSON
				bds_node* child = bdf_elem_child(_bson, key);
				if (bdf_elem_type(child) != E_ARRAY)
					return std::vector<T>();

				std::vector<T> arr;
				child = child->elem.data.node;
				while (child) {
					arr.push_back(T(child));
					child = child->next;
				}

				return arr;
			}
		};
	} 
}
