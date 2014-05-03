#pragma once

#include "Mutex.h"

namespace wtwUpdate {
	namespace updater {
		class UniqueThread {
			Mutex _mutex;
			bool _running;
			bool _aborted;			
		protected:
			WTWFUNCTIONS* fn;

			UniqueThread() : _running(false), _aborted(false), fn(NULL) { }

			void setRunning(bool running) {
				_mutex.enter();
				_running = running;
				_mutex.leave();
			}
			
			bool start(LPTHREAD_START_ROUTINE proc) {
				if (isRunning()) {
					// TODO: log
					return false;
				}

				_aborted = false;				
				DWORD threadId;
				HANDLE hThread = CreateThread(NULL, 0, proc, this, 0, &threadId);
				if (hThread) {
					CloseHandle(hThread);
					return true;
				}
				
				return false;
			}
		public:			
			virtual bool start() = 0;

			bool isRunning() {
				_mutex.enter();
				bool ret = _running;
				_mutex.leave();
				return ret;
			}			
			
			bool isAborted() const {
				return _aborted;
			}
			
			void abort() {
				_aborted = true;
			}

			void setFn(WTWFUNCTIONS* wtw) {
				fn = wtw;
			}
		};
	}
}
