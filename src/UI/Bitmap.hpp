#pragma once

#include <Windows.h>

namespace wtwUpdate {
	namespace ui {
        class Bitmap {
			HBITMAP _hBmp;
			bool _wtwManaged;
		public:
			Bitmap();
			Bitmap(const wchar_t* id, const wchar_t* themeFn, int resId);
			void release();

			inline HBITMAP getBmp() const {
				return _hBmp;
			}
		};
    }
}
