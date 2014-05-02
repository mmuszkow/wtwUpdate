#pragma once

#include "../resource.h"

#include "SearchBar.h"
#include "AddonsTree.h"
#include "HtmlEdit.h"
#include "../Updater/ZipFile.h"

namespace wtwUpdate {
	namespace ui {
		class UpdateWnd {
			SearchBar* _searchBar;
			AddonsTree* _tree;
			HtmlEdit* _text;

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
					wnd->_tree->setJson(downloadJson(L"http://muh.cba.pl/central.json"));
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
						std::vector<json::Addon> selected = wnd->_tree->getSelected();
						update(selected);
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

			// TODO: separate thread and not in this class
			static wtw::CJson* downloadJson(const std::wstring& url) {
				wtw::CInternetHttp http;
				wtw::CBuffer buff;
				if (FAILED(http.downloadFile(url.c_str(), buff))) {
					// TODO: log
					return NULL;
				}

				// parse
				const char* webpage = reinterpret_cast<const char*>(buff.getBuffer());
				wtw::CJson* json = wtw::CJson::load(webpage);
				wtw::CJson* tree;
				if (!json || !(tree = json->find("update"))) {
					// TODO: log
					return NULL;
				}

				return tree;
			}

			class TmpFile {
				std::wstring _path;
			public:
				TmpFile() {
					wchar_t path[MAX_PATH + 1], fn[MAX_PATH + 1];
					if (!GetTempPath(MAX_PATH, path))
						return;

					if(GetTempFileName(path, L"wtwUpdate-", 0, fn))
						_path = fn;
				}

				const std::wstring& getPath() const {
					return _path;
				}

				bool isValid() const {
					return _path.size() > 0;
				}

				bool write(const void* buff, size_t len) {
					FILE* f = _wfopen(_path.c_str(), L"wb");
					if (!f)
						return false;

					if (fwrite(buff, 1, len, f) != len) {
						fclose(f);
						return false;
					}

					fclose(f);
					return true;
				}

				void remove() {
					if (_path.size() > 0) {
						DeleteFile(_path.c_str());
						_path = L"";
					}
				}
			};

			// TODO: in WTW cache? download buffering?
			static TmpFile download2cache(const wchar_t* url) {
				wtw::CInternetHttp http;
				wtw::CBuffer buff;
				if (FAILED(http.downloadFile(url, buff)))
					return TmpFile();

				TmpFile f;
				if (!f.write(buff.getBuffer(), buff.getLength())) {
					f.remove();
					return TmpFile();
				}

				return f;
			}

			// TODO: separate thread and not in this class
			static void update(const std::vector<json::Addon>& addons) {
				// TODO: extend list with dependencies


				size_t len = addons.size();
				for (size_t i = 0; i < len; i++) {
					const json::Addon& addon = addons[i];

					if (addon.getInstallationState() == json::Addon::INSTALLED)
						continue;

					// TODO: inheritance of section's "dir"
					wchar_t zipUrl[1024];
					wchar_t* id = wtw::CConv::mtow(addon.getId().c_str());
					wchar_t* urlBase = wtw::CConv::mtow(addon.getParent()->getDir().c_str());
					wsprintf(zipUrl, L"%s/%s-%u.zip", urlBase, id, addon.getTime());
					wtw::CConv::release(urlBase);
					wtw::CConv::release(id);

					TmpFile f = download2cache(zipUrl);

					if (f.isValid()) {
						// TODO: error checking
						wtwUpdate::updater::ZipFile zip(f.getPath());
						zip.unzip();
						f.remove();
					}
				}
			}
		public:
			UpdateWnd(HINSTANCE hInst, HWND parent) {
				_searchBar = NULL;
				_tree = NULL;
				_text = NULL;
				if (parent)
					DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_UPDATE), NULL, DlgProc, reinterpret_cast<LPARAM>(this));
			}

			~UpdateWnd() {
				freeControls();
			}
		};
	}
}
