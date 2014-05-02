#pragma once

#include <vector>

#include "UniqueThread.h"

#include "../UI/UpdateWnd.h"
#include "../JsonObjs/AddonsList.h"

#include "cpp/Json.h"
#include "cpp/Internet.h"

namespace wtwUpdate {
	namespace updater {
		class CheckThread : public UniqueThread {

			static wtw::CJson* downloadJson(const std::wstring& url) {
				wtw::CInternetHttp http;
				wtw::CBuffer buff;
				if (FAILED(http.downloadFile(url.c_str(), buff))) {
					// TODO: log
					return NULL;
				}

				// parse
				const char* webpage = reinterpret_cast<const char*>(buff.getBuffer());
				wtw::CJson* json = wtw::CJson::load(webpage);
				if (!json) {
					// TODO: log
					return NULL;
				}

				wtw::CJson* tree = json->find("update");
				if (!tree) {
					wtw::CJson::decref(json);
					// TODO: log
					return NULL;
				}

				return tree;
			}

			static DWORD WINAPI proc(LPVOID args) {
				// download JSON
				wtw::CJson* root = downloadJson(L"http://muh.cba.pl/central.json");
				if (!root) {
					// TODO: log
					return 1;
				}

				// get all addons and check which need to be updated
				json::AddonsList addons(root);				
				unsigned int needUpdate = 0, i, len = addons.size();
				for (i = 0; i < len; i++)
					if (addons[i].getInstallationState() == json::Addon::MODIFIED)
						needUpdate++;

				if (needUpdate == 0) {
					wtw::CJson::decref(root);
					return 0;
				}

				// show update wnd, TODO: on click
				ui::UpdateWnd wnd(root);

				wtw::CJson::decref(root);
				return 0;
			}
		public:
			CheckThread() : Thread(proc) {
			}
		};
	}
}
