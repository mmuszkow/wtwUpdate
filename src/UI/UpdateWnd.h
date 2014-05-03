#pragma once

#include "../resource.h"

#include "SearchBar.h"
#include "AddonsTree.h"
#include "RichEdit/RichEdit.h"
#include "../Updater/InstallThread.h"

#include "cpp/Json.h"

namespace wtwUpdate {
	namespace ui {
		class UpdateWnd {
			//SearchBar* _searchBar;
			AddonsTree* _tree;
			MyRichEdit::RichEdit* _text;

			void freeControls() {
				//if (_searchBar) delete _searchBar;
				if (_tree) delete _tree;
				if (_text) delete _text;
			}

			static INT_PTR CALLBACK DlgProc(HWND hDlg, UINT Msg, WPARAM wParam, LPARAM lParam) {
				switch (Msg) {
				case WM_INITDIALOG:
					return TRUE;
				case WM_CTLCOLORDLG:
				case WM_CTLCOLORBTN:
				case WM_CTLCOLOREDIT:
				case WM_CTLCOLORSTATIC:
					return reinterpret_cast<INT_PTR>(GetStockObject(WHITE_BRUSH));
				case WM_COMMAND:
					switch (wParam) {
					case IDOK: {
						UpdateWnd& wnd = UpdateWnd::get();
						// TODO: some selected to be installed/updated, some to be removed
						updater::InstallThread& thread = updater::InstallThread::get();
						thread.setArg(wnd._tree->getSelected(), std::vector<json::Addon>());
						thread.start();
						EndDialog(hDlg, NULL);
						return TRUE;
					}
					case IDCANCEL:
						EndDialog(hDlg, NULL);
						return TRUE;
					}
					return FALSE;
				case WM_NOTIFY:
					if (((LPNMHDR)lParam)->code == TVN_SELCHANGED) {
						LPNMTREEVIEW pnmtv = reinterpret_cast<LPNMTREEVIEW>(lParam);
						LPARAM id = pnmtv->itemNew.lParam;
						UpdateWnd::get().selectHandler(id);
						return 0;
					}
					return FALSE;
				default: return FALSE;
				}
			}

			void selectHandler(LPARAM id) {
				if (!_text || !_tree)
					return;

				_text->setHtml(utow(_tree->getDescription(id)).c_str());
			}

			UpdateWnd() : /*_searchBar(NULL),*/ _tree(NULL), _text(NULL) { }
		public:
			bool open(HWND hParent, HINSTANCE hInst, wtw::CJson* json) {
				HWND hwnd = CreateDialog(hInst, MAKEINTRESOURCE(IDD_UPDATE), hParent, DlgProc);
				if (!hwnd)
					return false;

				freeControls();
				//_searchBar = new SearchBar(GetDlgItem(hwnd, IDC_SEARCH_BAR), NULL);
				_tree = new AddonsTree(GetDlgItem(hwnd, IDC_TREE), json);
				_text = new MyRichEdit::RichEdit(GetDlgItem(hwnd, IDC_TEXT));

				ShowWindow(hwnd, SW_SHOW);
				return true;
			}

			static UpdateWnd& get() {
				static UpdateWnd instance;
				return instance;
			}

			~UpdateWnd() {
				freeControls();
			}
		};
	}
}
