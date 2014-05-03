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
	}
}
