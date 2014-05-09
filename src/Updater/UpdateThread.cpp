#pragma once

#include "stdinc.h"
#include "UpdateThread.hpp"
#include "UI/UpdateWnd.hpp"
#include "JsonObjs/AddonsList.hpp"
#include "Utils/Http.hpp"
#include "libWTW-src/libJSON/jansson.h"

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

		DWORD UpdateThread::proc(LPVOID args) {
			UpdateThread* thread = static_cast<UpdateThread*>(args);
			thread->setRunning(true);

			// download JSON
			wtw::CJson* root = thread->downloadJson(L"http://muh.cba.pl/central.json");
			if (!root) {
				// logged in downloadJson
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
					thread->setRunning(false);
					return 0;
				}
				if (addons[i].getState() == json::Addon::MODIFIED)
					needUpdate++;
			}

			if (needUpdate == 0) {
				wtw::CJson::decref(root);
				thread->setRunning(false);
				return 0;
			}

			// TODO: quiet update? wait for user action?
			//ui::UpdateWnd wnd(root);

			wtw::CJson::decref(root);
			thread->setRunning(false);
			return 0;
		}
	}
}
