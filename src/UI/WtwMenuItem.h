#pragma once

#include "../stdinc.h"

namespace wtwUpdate {
	namespace ui {
		class WtwMenuItem {
			bool _created;
			std::wstring _parent;
			std::wstring _id;
		public:
			WtwMenuItem(const wchar_t* parent, const wchar_t* id, const wchar_t* caption, WTWFUNCTION callback) {
				wtwMenuItemDef menuDef;
				menuDef.menuID = parent;
				menuDef.itemId = id;
				menuDef.menuCaption = caption;
				menuDef.callback = callback;
				_created = (fn->fnCall(WTW_MENU_ITEM_ADD, menuDef, NULL) == S_OK);
			}

			bool isCreated() const {
				return _created;
			}

			virtual ~WtwMenuItem() {
				if (_created) {
					wtwMenuItemDef menuDef;
					menuDef.menuID = _parent.c_str();
					menuDef.itemId = _id.c_str();
					fn->fnCall(WTW_MENU_ITEM_DELETE, menuDef, NULL);
				}
			}
		};


	}
}
