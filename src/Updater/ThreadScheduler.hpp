#pragma once

#include "UniqueThread.hpp"

namespace wtwUpdate {
	namespace updater {
		class ThreadScheduler {
			int _lastId;
			std::vector<std::wstring> _created;
			
			static WTW_PTR wtwTimerFunc(WTW_PARAM wPar, WTW_PARAM lPar, void* cData);
			
			ThreadScheduler();
		public:
			inline static ThreadScheduler& get() {
				static ThreadScheduler instance;
				return instance;
			}
			
			bool schedule(UniqueThread& thread, int ms, bool once);

			void destroyAll();
		};
	}
}
