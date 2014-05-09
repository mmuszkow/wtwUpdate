#include "stdinc.h"
#include "UpdateWnd.hpp"
#include "Updater/InstallThread.hpp"

namespace wtwUpdate {
	namespace ui {
		UpdateWnd::UpdateWnd() : _hWnd(NULL), /*_searchBar(NULL),*/ _tree(NULL), _text(NULL) { }

		UpdateWnd::~UpdateWnd()  {
			destroy();
		}

		void UpdateWnd::freeControls() {
			//if (_searchBar) delete _searchBar;
			if (_tree) delete _tree;
			if (_text) delete _text;
			_tree = NULL;
			_text = NULL;
		}

		INT_PTR CALLBACK UpdateWnd::DlgProc(HWND hDlg, UINT Msg, WPARAM wParam, LPARAM lParam) {
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
					updater::InstallThread& thread = updater::InstallThread::get();
					thread.setArg(wnd._tree->getSelected(), wnd._tree->getUnselected());
					thread.start();
					EndDialog(hDlg, NULL);
					return TRUE;
				}
				case IDCANCEL:
					UpdateWnd::get().freeControls();
					EndDialog(hDlg, NULL);
					return TRUE;
				}
				return FALSE;
			case WM_LBUTTONUP:
				UpdateWnd::get()._tree->check();
				return FALSE;
			// TODO: move this to AddonsTree
			case WM_NOTIFY:
				if (wParam == IDC_TREE) {
					LPNMHDR lpnmh = reinterpret_cast<LPNMHDR>(lParam);
					if (lpnmh->code == TVN_SELCHANGED) {
						LPNMTREEVIEW pnmtv = reinterpret_cast<LPNMTREEVIEW>(lParam);
						LPARAM id = pnmtv->itemNew.lParam;
						UpdateWnd::get().selectHandler(id);
						return 0;
					} else if (lpnmh->code == NM_CLICK) {
						UpdateWnd::get()._tree->check();
					}
				}
				return FALSE;
			default: return FALSE;
			}
		}

		void UpdateWnd::selectHandler(LPARAM id) {
			if (!_text || !_tree)
				return;

			_text->setHtml(utow(_tree->getDescription(id)).c_str());
		}

		bool UpdateWnd::open(HWND hParent, HINSTANCE hInst, wtw::CJson* json) {
			destroy();

			_hWnd = CreateDialog(hInst, MAKEINTRESOURCE(IDD_UPDATE), hParent, DlgProc);
			if (!_hWnd)
				return false;

			//_searchBar = new SearchBar(GetDlgItem(hwnd, IDC_SEARCH_BAR), NULL);
			_tree = new tree::AddonsTree(GetDlgItem(_hWnd, IDC_TREE), json);
			_text = new MyRichEdit::RichEdit(GetDlgItem(_hWnd, IDC_TEXT));

			ShowWindow(_hWnd, SW_SHOW);
			return true;
		}

		void UpdateWnd::destroy() {
			if (_hWnd)
				DestroyWindow(_hWnd);
			freeControls();
		}
	}
}
