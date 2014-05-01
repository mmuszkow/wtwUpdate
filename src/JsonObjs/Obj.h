#pragma once

#include "cpp/Json.h"

namespace wtwUpdate { 
	namespace json {
		class Obj {
			wtw::CJson* _json;
		public:
			Obj() : _json(NULL) { }

			Obj(wtw::CJson* json) : _json(json) { }
		protected:
			__int64 getInt(const char* key, __int64 defVal = 0) const {
				if (!_json)
					return defVal;

				wtw::CJson* jsonVal = _json->find(key);
				if (jsonVal && jsonVal->isInteger())
					return jsonVal->c_int();

				return defVal;
			}

			std::string getStr(const char* key, const std::string& defVal = "") const {
				if (!_json)
					return defVal;

				wtw::CJson* jsonVal = _json->find(key);
				if (jsonVal && jsonVal->isString())
					return jsonVal->c_str();

				return defVal;
			}

			std::vector<std::string> getStrArray(const char* key) const {
				if (!_json)
					return std::vector<std::string>();

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

			template <class T>
			std::vector<T> getObjArray(const char* key) const {
				if (!_json)
					return std::vector<T>();

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
		};
	} 
}
