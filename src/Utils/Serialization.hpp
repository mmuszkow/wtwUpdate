#pragma once

#include "bson_decoder.h"
#include "cpp/Json.h"

namespace wtwUpdate {
	namespace utils {
		bds_node* downloadBson(const std::wstring& url, bds_strings_map* map);
		wtw::CJson* downloadJson(const std::wstring& url);
	}
}
