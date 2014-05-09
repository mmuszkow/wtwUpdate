#include "stdinc.h"
#include "Obj.hpp"

namespace wtwUpdate {
	namespace json {
		__int64 Obj::getInt(const char* key, __int64 defVal) const {
			if (!_json)
				return defVal;

			wtw::CJson* jsonVal = _json->find(key);
			if (jsonVal && jsonVal->isInteger())
				return jsonVal->c_int();

			return defVal;
		}

		std::string Obj::getStr(const char* key, const std::string& defVal) const {
			if (!_json)
				return defVal;

			wtw::CJson* jsonVal = _json->find(key);
			if (jsonVal && jsonVal->isString())
				return jsonVal->c_str();

			return defVal;
		}

		std::vector<std::string> Obj::getStrArray(const char* key) const {
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
	}
}
