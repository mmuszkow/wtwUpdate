#pragma once

#include <vector>
#include "UniqueThread.hpp"
#include "cpp/Json.h"
#include "bson_decoder.h"

namespace wtwUpdate {
	namespace updater {
		class UpdateThread : public UniqueThread {
			static WTW_PTR onNotificationClick(WTW_PARAM wPar, WTW_PARAM lPar, void* cData);
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
