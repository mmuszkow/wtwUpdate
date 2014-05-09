#pragma once

#include "UniqueThread.hpp"
#include "JsonObjs/AddonsList.hpp"

namespace wtwUpdate {
	namespace updater {
		class InstallThread : public UniqueThread {
			// it's a singleton so this is used for currently running
			json::AddonsList _toInstall;
			json::AddonsList _toRemove;

			static DWORD WINAPI proc(LPVOID args);
		public:
			inline static InstallThread& get() {
				static InstallThread instance;
				return instance;
			}
			
			inline bool start() {
				return UniqueThread::start(proc);
			}

			void setArg(const std::vector<json::Addon>& toInstall, const std::vector<json::Addon>& toRemove);
		};
	}
}
