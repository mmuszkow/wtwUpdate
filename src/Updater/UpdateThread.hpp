#pragma once

#include <vector>
#include "UniqueThread.hpp"
#include "cpp/Json.h"

namespace wtwUpdate {
	namespace updater {
		class UpdateThread : public UniqueThread {
		public:
			wtw::CJson* downloadJson(const std::wstring& url);
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
