#pragma once

#include "../stdinc.h"
#include "UniqueThread.h"

namespace wtwUpdate {
	namespace updater {
		class ThreadScheduler {
			int _lastId;
			std::vector<std::wstring> _created;
			
			static WTW_PTR wtwTimerFunc(WTW_PARAM wPar, WTW_PARAM lPar, void* cData) {
				wtwTimerEvent* ev = reinterpret_cast<wtwTimerEvent*>(wPar);
				if (ev->event == WTW_TIMER_EVENT_TICK) {
					UniqueThread* thread = static_cast<UniqueThread*>(cData);
					thread->start();
				}
				return 0;
			}
			
			ThreadScheduler() : _lastId(0) {}
		public:
			static ThreadScheduler& get() {
				static ThreadScheduler instance;
				return instance;
			}
			
			bool schedule(UniqueThread& thread, int ms, bool once) {
				wchar_t id[128];
				swprintf_s(id, 128, L"wtwUpdate/thread-%d", _lastId++);

				wtwTimerDef timerDef;
				timerDef.id = id;
				timerDef.sleepTime = ms;
				timerDef.callback = wtwTimerFunc;
				timerDef.cbData = &thread;
				if (once)
					timerDef.flags = WTW_TIMER_FLAG_ONE_TICK;

				if (fn->fnCall(WTW_TIMER_CREATE, timerDef, NULL) != S_OK) {
					LOG_ERR(L"Cannot create timer %s", id);
					return false;
				}
				
				_created.push_back(timerDef.id);
				return true;
			}

			void destroyAll() {
				size_t len = _created.size();
				for (size_t i = 0; i < len; i++) {
					wtwTimerDef timerDef;
					timerDef.id = _created[i].c_str();
					fn->fnCall(WTW_TIMER_DESTROY, timerDef, NULL);
				}
			}
		};
	}
}
