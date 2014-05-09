#pragma once

//#include "SearchBar.hpp"
#include "Tree/AddonsTree.hpp"
#include "RichEdit/RichEdit.hpp"
#include "cpp/Json.h"

namespace wtwUpdate {
	namespace ui {
		class UpdateWnd {
			HWND _hWnd;
			//SearchBar* _searchBar;
			tree::AddonsTree* _tree;
			MyRichEdit::RichEdit* _text;

			void freeControls();

			static INT_PTR CALLBACK DlgProc(HWND hDlg, UINT Msg, WPARAM wParam, LPARAM lParam);

			void selectHandler(LPARAM id);

			UpdateWnd();
		public:
			~UpdateWnd();
			bool open(HWND hParent, HINSTANCE hInst, wtw::CJson* json);

			static UpdateWnd& get() {
				static UpdateWnd instance;
				return instance;
			}

			void destroy();			
		};
	}
}
