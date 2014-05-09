#pragma once

#include <string>
#include <plInterface.h>

namespace wtwUpdate {
	namespace ui {
		class WtwMenuItem {
			bool _created;
			std::wstring _parent;
			std::wstring _id;
		public:
			WtwMenuItem(const wchar_t* parent, const wchar_t* id, const wchar_t* caption, WTWFUNCTION callback);
			virtual ~WtwMenuItem();

			bool isCreated() const {
				return _created;
			}
		};
	}
}
