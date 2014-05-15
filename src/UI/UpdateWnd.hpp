#pragma once

//#include "SearchBar.hpp"
#include "Tree/AddonsTree.hpp"
#include "RichEdit/RichEdit.hpp"
#include "Updater/UniqueThread.hpp"

namespace wtwUpdate {
	namespace ui {
		class UpdateWnd : public updater::UniqueThread {
			HWND _hWnd;
			//SearchBar* _searchBar;
			tree::AddonsTree* _tree;
			MyRichEdit::RichEdit* _text;

			static INT_PTR CALLBACK DlgProc(HWND hDlg, UINT Msg, WPARAM wParam, LPARAM lParam);

			void selectHandler(LPARAM id);
			BOOL resizeHandler(int w, int h);


			static DWORD WINAPI download(PVOID arg);
			UpdateWnd();
			bool init();
			void freeControls();

			int _minW, _minH;

			static inline LONG rectW(RECT& r) {
				return r.right - r.left;
			}

			static inline LONG rectH(RECT& r) {
				return r.bottom - r.top;
			}
		public:
			~UpdateWnd();

			void destroy();

			bool start() {
				destroy();
				_hWnd = CreateDialog(hInst, MAKEINTRESOURCE(IDD_UPDATE), hMain, DlgProc);
				if (!_hWnd)
					return false;
				ShowWindow(_hWnd, SW_SHOW);
				return UniqueThread::start(download);
			}

			static UpdateWnd& get() {
				static UpdateWnd instance;
				return instance;
			}
		};
	}
}
