#pragma once

#include <Windows.h>

namespace wtwUpdate {
	namespace utils {
		class Mutex {
			HANDLE _hMutex;
		public:
			Mutex() {
				_hMutex = CreateMutex(NULL, FALSE, NULL);
			}

			~Mutex() {
				CloseHandle(_hMutex);
			}

			inline void enter() {
				WaitForSingleObject(_hMutex, INFINITE);
			}

			inline void leave() {
				ReleaseMutex(_hMutex);
			}
		};
	}
}
