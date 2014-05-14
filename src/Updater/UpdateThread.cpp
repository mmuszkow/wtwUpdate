#pragma once

#include "stdinc.h"
#include "UpdateThread.hpp"
#include "UI/UpdateWnd.hpp"
#include "JsonObjs/AddonsList.hpp"
#include "Utils/Http.hpp"

namespace wtwUpdate {
	namespace updater {
		wtw::CJson* UpdateThread::downloadJson(const std::wstring& url) {
			utils::Http http;
			utils::Http::Buffer buff;

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

		bds_node* UpdateThread::downloadBson(const std::wstring& url, bds_strings_map* map) {
			utils::Http http;
			utils::BinaryFile f;

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

		DWORD UpdateThread::proc(LPVOID args) {
			UpdateThread* thread = static_cast<UpdateThread*>(args);
			thread->setRunning(true);

			// download JSON
			wtw::CJson* root = thread->downloadJson(L"http://wtw-addons.cba.pl/central.json");
			//bds_strings_map* map = bdf_strings_map_create(8192);
			//bds_node* root = thread->downloadBson(L"http://wtw-addons.cba.pl/central.bson", map);
			if (!root) {
				// logged in downloadBson
				//bdf_strings_map_destroy(map);
				thread->setRunning(false);
				return 1;
			}

			// get all addons and check which need to be updated
			// TODO: this should be also filtered by installation request, only the addons installed by wtwUpdate should be here
			json::AddonsList addons(root);
			unsigned int needUpdate = 0, i, len = addons.size();
			for (i = 0; i < len; i++) {
				if (thread->isAborted()) {
					wtw::CJson::decref(root);
					//bdf_node_destroy(root, false);
					//bdf_strings_map_destroy(map);
					thread->setRunning(false);
					return 0;
				}
				if (addons[i].getState() == json::Addon::MODIFIED)
					needUpdate++;
			}

			if (needUpdate == 0) {
				wtw::CJson::decref(root);
				//bdf_node_destroy(root, false);
				//bdf_strings_map_destroy(map);
				thread->setRunning(false);
				return 0;
			}

			// TODO: quiet update? wait for user action?
			//ui::UpdateWnd wnd(root);

			wtw::CJson::decref(root);
			//bdf_node_destroy(root, false);
			//bdf_strings_map_destroy(map);
			thread->setRunning(false);
			return 0;
		}
	}
}
