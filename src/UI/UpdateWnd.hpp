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

			bool init();
		public:
			~UpdateWnd();

			template<class T> bool open(HWND hParent, HINSTANCE hInst, T data) { // problems when in .cpp
				destroy();

				_hWnd = CreateDialog(hInst, MAKEINTRESOURCE(IDD_UPDATE), hParent, DlgProc);
				if (!_hWnd)
					return false;

				//_searchBar = new SearchBar(GetDlgItem(hwnd, IDC_SEARCH_BAR), NULL);
				_tree = new tree::AddonsTree(GetDlgItem(_hWnd, IDC_TREE), data);
				_text = new MyRichEdit::RichEdit(GetDlgItem(_hWnd, IDC_TEXT));

				ShowWindow(_hWnd, SW_SHOW);
				return true;
			}

			static UpdateWnd& get() {
				static UpdateWnd instance;
				return instance;
			}

			void destroy();			
		};
	}
}
