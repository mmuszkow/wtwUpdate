#pragma once

#include <string>
#include "Control.hpp"

namespace wtwUpdate {
	namespace ui {
		// TODO: call search handler after 3 sec?
		class SearchBar : public Control {

			typedef void(*tFilterHandler)(const std::wstring& filter);
		public:
			SearchBar(HWND handle, tFilterHandler handler) : Control(handle) {

			}
		};
	}
}
