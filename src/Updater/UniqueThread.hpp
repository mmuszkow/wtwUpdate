#pragma once

#include "Utils/Mutex.hpp"

namespace wtwUpdate {
	namespace updater {
		class UniqueThread {
			utils::Mutex _mutex;
			bool _running;
			bool _aborted;			
		protected:
			UniqueThread();
			virtual ~UniqueThread();

			void setRunning(bool running);			
			bool start(LPTHREAD_START_ROUTINE proc);
		public:			
			virtual bool start() = 0;

			bool isRunning();
			
			inline bool isAborted() const {
				return _aborted;
			}
			
			inline void abort() {
				_aborted = true;
			}
		};
	}
}
