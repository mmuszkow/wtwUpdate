#pragma once

#include "../stdinc.h"

namespace wtwUpdate {
	namespace updater {
		class TimerThread {
			static WTW_PTR startThread(WTW_PARAM wPar, WTW_PARAM lPar, void* cData) {
				wtwTimerEvent* ev = reinterpret_cast<wtwTimerEvent*>(wPar);
				if (ev->event == WTW_TIMER_EVENT_TICK) {
					DWORD threadId;
					HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)cData, NULL, 0, &threadId);
					if (hThread)
						CloseHandle(hThread);
					// TODO: else log
				}

				return 0;
			}
		protected:
			bool shouldAbort;
		public:
			TimerThread(WTWFUNCTIONS* fn, int ms, bool once, LPTHREAD_START_ROUTINE function) : shouldAbort(false) {
				wtwTimerDef timerDef;
				timerDef.id = L"wtwUpdate/thread";
				timerDef.sleepTime = ms;
				timerDef.callback = startThread;
				timerDef.cbData = function;
				if (once)
					timerDef.flags = WTW_TIMER_FLAG_ONE_TICK;

				if (fn->fnCall(WTW_TIMER_CREATE, timerDef, NULL) != S_OK)
					__LOG_F(fn, WTW_LOG_LEVEL_ERROR, MDL, L"Cannot create timer");
			}

			void abort() {
				shouldAbort = true;
			}
		};
	}
}
