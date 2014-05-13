#pragma once

#include <vector>
#include "UniqueThread.hpp"
#include "cpp/Json.h"
#include "bson_decoder.h"

namespace wtwUpdate {
	namespace updater {
		class UpdateThread : public UniqueThread {
		public:
			wtw::CJson* downloadJson(const std::wstring& url);
			bds_node*   downloadBson(const std::wstring& url, bds_strings_map* map = NULL);
		protected:
			static DWORD WINAPI proc(LPVOID args);
		public:
			inline static UpdateThread& get() {
				static UpdateThread instance;
				return instance;
			}
			
			inline bool start() {
				return UniqueThread::start(proc);
			}
		};
	}
}
