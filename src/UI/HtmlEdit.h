#pragma once

#include <string>

#include "Control.h"



namespace wtwUpdate {
	namespace ui {
		// TODO: rich edit or html control
		class HtmlEdit : public Control {

		public:
			HtmlEdit(HWND handle) : Control(handle) {

			}

			inline void setText(const std::string& html) {
				// TODO: utf-8 -> utf-16
				wchar_t* w16 = wtw::CConv::utow(html.c_str());
				SetWindowText(getHwnd(), w16);
				wtw::CConv::release(w16);
			}
		};
	}
}
