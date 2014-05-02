#pragma once

#include <Windows.h>

namespace wtwUpdate {
	namespace updater {
		class Mutex {
			HANDLE _hMutex;
		public:
			Mutex() {
				_hMutex = CreateMutex(NULL, FALSE, NULL);
			}

			~Mutex() {
				CloseHandle(_hMutex);
			}

			void enter() {
				WaitForSingleObject(_hMutex, INFINITE);
			}

			void leave() {
				ReleaseMutex(_hMutex);
			}
		};

		class UniqueThread {
			Mutex _mutex;
			bool _running;

			UniqueThread() {

			}

		protected:

		public:
			UniqueThread& get() {
				static UniqueThread instance;
				return instance;
			}

			bool isRunning() const {
				_mutex.enter();
				bool ret = _running;
				_mutex.leave();
				return ret;
			}

			void start(LPTHREAD_START_ROUTINE proc) {
				if (isRunning()) {
					// TODO: log
					return;
				}

				DWORD threadId;
				HANDLE hThread = CreateThread(NULL, 0, proc, NULL, 0, &threadId);
				if (hThread)
					CloseHandle(hThread);
			}
		};
	}
}
