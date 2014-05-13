#include "stdinc.h"
#include "Obj.hpp"

namespace wtwUpdate {
	namespace json {
		__int64 Obj::getInt(const char* key, __int64 defVal) const {
			// JSON
			if (_json) {
				wtw::CJson* jsonVal = _json->find(key);
				if (jsonVal && jsonVal->isInteger())
					return jsonVal->c_int();

				return defVal;
			}

			// BSON
			bds_node* child = bdf_elem_child(_bson, key);
			bdt_int64 i;
			if (child && bdf_elem_int64(child, &i))
				return i;

			return defVal;
		}

		std::string Obj::getStr(const char* key, const std::string& defVal) const {
			// JSON
			if (_json) {
				wtw::CJson* jsonVal = _json->find(key);
				if (jsonVal && jsonVal->isString())
					return jsonVal->c_str();

				return defVal;
			}

			// BSON
			bds_node* child = bdf_elem_child(_bson, key);
			char* str;
			if (child && bdf_elem_str(child, &str))
				return str;

			return defVal;
		}

		std::vector<std::string> Obj::getStrArray(const char* key) const {
			// JSON
			if (_json) {
				wtw::CJson* jsonVal = _json->find(key);

				if (!jsonVal)
					return std::vector<std::string>();

				if (jsonVal->isString()) {
					std::vector<std::string> single;
					single.push_back(jsonVal->c_str());
					return single;
				}

				if (jsonVal->isArray()) {
					std::vector<std::string> arr;
					size_t len = jsonVal->size();
					for (size_t i = 0; i < len; i++)
						arr.push_back(jsonVal->getAt(i)->c_str());
					return arr;
				}

				return std::vector<std::string>();
			}

			// BSON
			bds_node* child = bdf_elem_child(_bson, key);
			if (bdf_elem_type(child) != E_ARRAY)
				return std::vector<std::string>();

			// TODO
			std::vector<std::string> arr;
			while (child) {
				char* str;
				if (!bdf_elem_str(child, &str))
					return std::vector<std::string>();

				arr.push_back(str);
				child = child->next;
			}

			return arr;
		}		
	}
}
