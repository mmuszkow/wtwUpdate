#pragma once

#include "TimerThread.h"

#include "cpp/Json.h"
#include "cpp/Internet.h"

namespace wtwUpdate {
	namespace updater {
		class CheckThread : public TimerThread {

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
				wtw::CJson* tree;
				if (!json || !(tree = json->find("update"))) {
					// TODO: log
					return NULL;
				}

				return tree;
			}

			static DWORD WINAPI func(LPVOID args) {

				return 0;
			}
		public:
		};
	}
}
