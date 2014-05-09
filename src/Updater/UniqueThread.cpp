#pragma once

#include "stdinc.h"
#include "UniqueThread.hpp"

namespace wtwUpdate {
	namespace updater {
		UniqueThread::UniqueThread() : _running(false), _aborted(false) { }
		UniqueThread::~UniqueThread() { }

		void UniqueThread::setRunning(bool running) {
			_mutex.enter();
			_running = running;
			_mutex.leave();
		}

		bool UniqueThread::start(LPTHREAD_START_ROUTINE proc) {
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

		bool UniqueThread::isRunning() {
			_mutex.enter();
			bool ret = _running;
			_mutex.leave();
			return ret;
		}
	}
}
