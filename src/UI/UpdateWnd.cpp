#include "stdinc.h"
#include "UpdateWnd.hpp"
#include "Updater/InstallThread.hpp"

namespace wtwUpdate {
	namespace ui {
		UpdateWnd::UpdateWnd() : _hWnd(NULL), /*_searchBar(NULL),*/ _tree(NULL), _text(NULL), _minW(0), _minH(0) { }

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
			case WM_INITDIALOG: {
				RECT r;
				GetWindowRect(hDlg, &r);
				UpdateWnd::get()._minW = r.right - r.left;
				UpdateWnd::get()._minH = r.bottom - r.top;
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
			case WM_GETMINMAXINFO: {
				MINMAXINFO* mmi = reinterpret_cast<MINMAXINFO*>(lParam);
				mmi->ptMinTrackSize.x = UpdateWnd::get()._minW;
				mmi->ptMinTrackSize.y = UpdateWnd::get()._minH;
				return 0;
			}
			case WM_SIZE:
				return UpdateWnd::get().resizeHandler(LOWORD(lParam), HIWORD(lParam));
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

			_text->setHtml(stow(_tree->getDescription(id)).c_str());
		}

		BOOL UpdateWnd::resizeHandler(int w, int h) {
			RECT treeRect = { 0, 0, 150, 200 };
			RECT textRect = { 150, 0, 300, 200 };
			RECT buttonRect = { 235, 200, 300, 220 };
			if (MapDialogRect(_hWnd, &treeRect) == FALSE
				|| MapDialogRect(_hWnd, &textRect) == FALSE
				|| MapDialogRect(_hWnd, &buttonRect) == FALSE)
				return 1;

			MoveWindow(GetDlgItem(_hWnd, IDC_TREE),
				treeRect.left, treeRect.top,
				rectW(treeRect), h - rectH(buttonRect),
				FALSE);
			MoveWindow(GetDlgItem(_hWnd, IDC_TEXT), 
				textRect.left, textRect.top, 
				w - textRect.left, h - rectH(buttonRect),
				FALSE);
			MoveWindow(GetDlgItem(_hWnd, IDCANCEL), 
				w - rectW(buttonRect) * 2, h - rectH(buttonRect),
				rectW(buttonRect), rectH(buttonRect), 
				FALSE);
			MoveWindow(GetDlgItem(_hWnd, IDOK), 
				w - rectW(buttonRect), h - rectH(buttonRect),
				rectW(buttonRect), rectH(buttonRect), 
				FALSE);

			InvalidateRect(_hWnd, NULL, TRUE);

			return 0;
		}

		void UpdateWnd::destroy() {
			if (_hWnd)
				DestroyWindow(_hWnd);
			freeControls();
		}
	}
}
