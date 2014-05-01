#pragma once

#include <Windows.h>

namespace wtwUpdate {
	namespace ui {
		class Control {
			HWND _handle;
		public:
			Control(HWND handle) {
				_handle = handle;
			}

			inline HWND getHwnd() const {
				return _handle;
			}
		};
	}
}
