#include "stdinc.h"
#include "Serialization.hpp"
#include "Http.hpp"

bds_node* wtwUpdate::utils::downloadBson(const std::wstring& url, bds_strings_map* map) {
	wtwUpdate::utils::Http http;
	wtwUpdate::utils::BinaryFile f;

	if (!http.download2file(url, &f)) {
		f.del();
		LOG_ERR(L"Failed to download BSON file from %s", url.c_str());
		return NULL;
	}

	bds_decoder* decoder = bdf_decoder_create_with_cache_from(wtos(f.getPath()).c_str(), map);
	bds_node* node;
	if (!bdf_read_doc(decoder, &node)) {
		LOG_ERR(L"Failed to parse BSON file from %s", url.c_str());
		node = NULL;
	}

	bdf_decoder_destroy(decoder);
	f.del();
	return node;
}

wtw::CJson* wtwUpdate::utils::downloadJson(const std::wstring& url) {
	wtwUpdate::utils::Http http;
	wtwUpdate::utils::Http::Buffer buff;

	if (!http.download2buff(url.c_str(), &buff)) {
		LOG_ERR(L"Failed to download JSON file from %s", url.c_str());
		return NULL;
	}

	// parse
	const char* webpage = reinterpret_cast<const char*>(buff.getBuffer());
	wtw::CJson* json = wtw::CJson::load(webpage);
	if (!json) {
		LOG_ERR(L"Failed to parse JSON file from %s", url.c_str());
		return NULL;
	}

	wtw::CJson* tree = json->find("update");
	if (!tree) {
		wtw::CJson::decref(json);
		LOG_ERR(L"Failed to find update tag in JSON file from %s", url.c_str());
		return NULL;
	}

	return tree;
}
