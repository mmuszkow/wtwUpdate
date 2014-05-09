#pragma once

#include "stdinc.h"
#include "ThreadScheduler.hpp"

namespace wtwUpdate {
	namespace updater {
		ThreadScheduler::ThreadScheduler() : _lastId(0) { }

		WTW_PTR ThreadScheduler::wtwTimerFunc(WTW_PARAM wPar, WTW_PARAM lPar, void* cData) {
			wtwTimerEvent* ev = reinterpret_cast<wtwTimerEvent*>(wPar);
			if (ev->event == WTW_TIMER_EVENT_TICK) {
				UniqueThread* thread = static_cast<UniqueThread*>(cData);
				thread->start();
			}
			return 0;
		}
		
		bool ThreadScheduler::schedule(UniqueThread& thread, int ms, bool once) {
			wchar_t id[128];
			swprintf_s(id, 128, L"wtwUpdate/thread-%d", _lastId++);

			wtwTimerDef timerDef;
			timerDef.id = id;
			timerDef.sleepTime = ms;
			timerDef.callback = wtwTimerFunc;
			timerDef.cbData = &thread;
			if (once)
				timerDef.flags = WTW_TIMER_FLAG_ONE_TICK;

			if (FAILED(fn->fnCall(WTW_TIMER_CREATE, timerDef, NULL))) {
				LOG_ERR(L"Cannot create timer %s", id);
				return false;
			}

			_created.push_back(timerDef.id);
			return true;
		}

		void ThreadScheduler::destroyAll() {
			size_t len = _created.size();
			for (size_t i = 0; i < len; i++) {
				wtwTimerDef timerDef;
				timerDef.id = _created[i].c_str();
				fn->fnCall(WTW_TIMER_DESTROY, timerDef, NULL);
			}
		}
	}
}
