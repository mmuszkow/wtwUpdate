#include "stdinc.h"
#include "WtwMenuItem.hpp"

namespace wtwUpdate {
	namespace ui {
		WtwMenuItem::WtwMenuItem(const wchar_t* parent, const wchar_t* id, const wchar_t* caption, WTWFUNCTION callback) {
			wtwMenuItemDef menuDef;
			menuDef.menuID = parent;
			menuDef.itemId = id;
			menuDef.menuCaption = caption;
			menuDef.callback = callback;
			_created = SUCCEEDED(fn->fnCall(WTW_MENU_ITEM_ADD, menuDef, NULL));
		}

		WtwMenuItem::~WtwMenuItem() {
			if (_created) {
				wtwMenuItemDef menuDef;
				menuDef.menuID = _parent.c_str();
				menuDef.itemId = _id.c_str();
				fn->fnCall(WTW_MENU_ITEM_DELETE, menuDef, NULL);
			}
		}
	}
}
