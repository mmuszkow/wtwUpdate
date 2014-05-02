#pragma once

#include "../resource.h"

#include "SearchBar.h"
#include "AddonsTree.h"
#include "HtmlEdit.h"
#include "../Updater/InstallThread.h"

#include "cpp/Json.h"

namespace wtwUpdate {
	namespace ui {
		class UpdateWnd {
			SearchBar* _searchBar;
			AddonsTree* _tree;
			HtmlEdit* _text;
			wtw::CJson* _json;

			void freeControls() {
				if (_searchBar) delete _searchBar;
				if (_tree) delete _tree;
				if (_text) delete _text;
			}

			static INT_PTR CALLBACK DlgProc(HWND hDlg, UINT Msg, WPARAM wParam, LPARAM lParam) {
				switch (Msg) {
				case WM_INITDIALOG: {
					UpdateWnd* wnd = reinterpret_cast<UpdateWnd*>(lParam);
					SetProp(hDlg, L"PTR", wnd);
					wnd->freeControls();
					wnd->_searchBar = new SearchBar(GetDlgItem(hDlg, IDC_SEARCH_BAR), NULL);
					wnd->_tree = new AddonsTree(GetDlgItem(hDlg, IDC_TREE));
					wnd->_text = new HtmlEdit(GetDlgItem(hDlg, IDC_TEXT));
					return TRUE;
				}
				case WM_CTLCOLORDLG:
				case WM_CTLCOLORBTN:
				case WM_CTLCOLOREDIT:
				case WM_CTLCOLORSTATIC:
					return reinterpret_cast<INT_PTR>(GetStockObject(WHITE_BRUSH));
				case WM_COMMAND:
					switch (wParam) {
					case IDOK: {
						UpdateWnd* wnd = reinterpret_cast<UpdateWnd*>(GetProp(hDlg, L"PTR"));
						updater::InstallThread::start(wnd->_tree->getSelected());
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
						UpdateWnd* wnd = reinterpret_cast<UpdateWnd*>(GetProp(hDlg, L"PTR"));
						LPNMTREEVIEW pnmtv = reinterpret_cast<LPNMTREEVIEW>(lParam);
						LPARAM id = pnmtv->itemNew.lParam;
						wnd->selectHandler(id);
						return 0;
					}
					return FALSE;
				default: return FALSE;
				}
			}

			void selectHandler(LPARAM id) {
				if (!_text || !_tree)
					return;

				_text->setText(_tree->getDescription(id));
			}

		public:
			UpdateWnd(wtw::CJson* json) : _json(json) {
				_searchBar = NULL;
				_tree = NULL;
				_text = NULL;
				DialogBoxParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_UPDATE), NULL, DlgProc, reinterpret_cast<LPARAM>(this));
			}

			~UpdateWnd() {
				freeControls();
			}
		};
	}
}
