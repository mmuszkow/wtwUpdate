#include <Windows.h>
#include <Commctrl.h>
#include <GdiPlus.h>

#include "RichEdit.h"
#include "FastStrBuff.h"

namespace MyRichEdit {
	HMODULE		RichEdit::_hRichEditLib = NULL;
#ifndef MY_RICHEDIT_NO_OLE
	ULONG_PTR	RichEdit::_gdiplusToken = 0L;
#endif

	RichEdit::RichEdit(HWND hControl) : wtwUpdate::ui::Control(hControl) {
		if (!hControl)
			return;

		memset(&_defaultCf, 0, sizeof(CHARFORMAT));
#ifndef MY_RICHEDIT_NO_OLE
		_pRichEditOle = NULL; 
		SendMessage(getHwnd(), EM_GETOLEINTERFACE, 0, reinterpret_cast<LPARAM>(&_pRichEditOle));
#endif
		SendMessage(getHwnd(), EM_AUTOURLDETECT, TRUE, 0);

		LRESULT mask = SendMessage(getHwnd(), EM_GETEVENTMASK, 0, 0);
		SendMessage(getHwnd(), EM_SETEVENTMASK, 0, mask | ENM_LINK);

		_defaultCf.cbSize = sizeof(CHARFORMAT);
		_defaultCf.dwMask = CFM_ALL;

		SendMessage(getHwnd(), EM_GETCHARFORMAT, SCF_DEFAULT, reinterpret_cast<LPARAM>(&_defaultCf));
		_defaultCf.dwEffects = CFE_AUTOCOLOR;
		wcscpy_s(_defaultCf.szFaceName, 32, L"Tahoma");
		SendMessage(getHwnd(), EM_SETCHARFORMAT, SCF_DEFAULT, reinterpret_cast<LPARAM>(&_defaultCf));

		SetWindowSubclass(getHwnd(), RichEdit::WndProc,
			0, reinterpret_cast<DWORD_PTR>(this));
		SetWindowSubclass(GetParent(getHwnd()), RichEdit::ParentWndProc,
			0, reinterpret_cast<DWORD_PTR>(this));
		HideCaret(getHwnd());
		_scrolledDown = true;

	}

	RichEdit::~RichEdit() {
#ifndef MY_RICHEDIT_NO_OLE
		if (_pRichEditOle)
			_pRichEditOle->Release();
#endif
		RemoveWindowSubclass(GetParent(getHwnd()), RichEdit::ParentWndProc, 0);
		RemoveWindowSubclass(getHwnd(), RichEdit::WndProc, 0);
		if (getHwnd())
			DestroyWindow(getHwnd());
	}

	void RichEdit::handleImg(std::wstring tag) {
#ifndef MY_RICHEDIT_NO_OLE
		if(!_pRichEditOle)
			return addText(L" [Obrazek] ");
		
		size_t srcStart = tag.find(L"src=\"");
		if(srcStart == std::wstring::npos)
			return addText(L" [Obrazek] ");
		
		size_t srcEnd = tag.find(L'\"', srcStart+5);
		if(srcEnd == std::wstring::npos)
			return addText(L" [Obrazek] ");

		std::wstring imgPath = tag.substr(srcStart+5, srcEnd-srcStart-5);
		SendMessage(getHwnd(), EM_SETSEL, 0xFFFFFFF, 0xFFFFFFF);

		Gdiplus::GpBitmap* image;
		Gdiplus::DllExports::GdipCreateBitmapFromFile(imgPath.c_str(), &image);
		if(!image)
			return addText(L" [Obrazek] ");
		HBITMAP hImg;
		Gdiplus::DllExports::GdipCreateHBITMAPFromBitmap(image, &hImg, 0xFFFFFFFF);
		if(hImg) {
			_imgDataObj.InsertBitmap(_pRichEditOle, hImg);
			DeleteObject(hImg);
		}
		Gdiplus::DllExports::GdipDisposeImage(image);
#else
		return addText(L" [Obrazek] ");
#endif
	}

	void RichEdit::libInit() {
		InitCommonControls();
		_hRichEditLib = LoadLibraryW(L"Msftedit.dll");
#ifndef MY_RICHEDIT_NO_OLE
		Gdiplus::GdiplusStartupInput gdiplusStartupInput;
		Gdiplus::GdiplusStartup(&_gdiplusToken, &gdiplusStartupInput, 0);
#endif
	}

	void RichEdit::libDeinit() {
		if(_hRichEditLib)
			FreeLibrary(_hRichEditLib);
#ifndef MY_RICHEDIT_NO_OLE
		Gdiplus::GdiplusShutdown(_gdiplusToken);
#endif
	}

	bool RichEdit::scrollDown()	{
		if (SendMessage(getHwnd(), WM_VSCROLL, SB_BOTTOM, NULL) == 0) {
			_scrolledDown = true;
			return true;
		} else
			return false;
	}

	bool RichEdit::scrollTop() {
		if (SendMessage(getHwnd(), WM_VSCROLL, SB_TOP, NULL) == 0) {
			_scrolledDown = false;
			return true;
		} else
			return false;
	}

	void RichEdit::setBold(bool set) {
		CHARFORMAT cf;
		memset(&cf, 0, sizeof(CHARFORMAT));
		cf.cbSize = sizeof(CHARFORMAT);
		cf.dwMask = CFM_BOLD;
		if(set)
			cf.dwEffects |= CFE_BOLD;
		else
			cf.dwEffects &= ~CFE_BOLD;
		SendMessage(getHwnd(), EM_SETSEL, 0xFFFFFFF, 0xFFFFFFF);
		SendMessage(getHwnd(), EM_SETCHARFORMAT, SCF_SELECTION, reinterpret_cast<LPARAM>(&cf));
	}

	void RichEdit::setFont(const wchar_t* fontName)	{
		CHARFORMAT cf;
		memset(&cf, 0, sizeof(CHARFORMAT));
		cf.cbSize = sizeof(CHARFORMAT);
		cf.dwMask = CFM_FACE;
		wcscpy_s(cf.szFaceName, LF_FACESIZE, fontName);
		SendMessage(getHwnd(), EM_SETSEL, 0xFFFFFFF, 0xFFFFFFF);
		SendMessage(getHwnd(), EM_SETCHARFORMAT, SCF_DEFAULT, reinterpret_cast<LPARAM>(&cf));
	}

	void RichEdit::setItalic(bool set) {
		CHARFORMAT cf;
		memset(&cf, 0, sizeof(CHARFORMAT));
		cf.cbSize = sizeof(CHARFORMAT);
		cf.dwMask = CFM_ITALIC;
		if(set)
			cf.dwEffects |= CFE_ITALIC;
		else
			cf.dwEffects &= ~CFE_ITALIC;
		SendMessage(getHwnd(), EM_SETSEL, 0xFFFFFFF, 0xFFFFFFF);
		SendMessage(getHwnd(), EM_SETCHARFORMAT, SCF_SELECTION, reinterpret_cast<LPARAM>(&cf));
	}

	void RichEdit::setUnderline(bool set) {
		CHARFORMAT cf;
		memset(&cf, 0, sizeof(CHARFORMAT));
		cf.cbSize = sizeof(CHARFORMAT);
		cf.dwMask = CFM_UNDERLINE;
		if(set)
			cf.dwEffects |= CFE_UNDERLINE;
		else
			cf.dwEffects &= ~CFE_UNDERLINE;
		SendMessage(getHwnd(), EM_SETSEL, 0xFFFFFFF, 0xFFFFFFF);
		SendMessage(getHwnd(), EM_SETCHARFORMAT, SCF_SELECTION, reinterpret_cast<LPARAM>(&cf));
	}

	void RichEdit::setFontColor(COLORREF color) {
		CHARFORMAT cf;
		memset(&cf, 0, sizeof(CHARFORMAT));
		cf.cbSize = sizeof(CHARFORMAT);
		cf.dwMask = CFM_COLOR;
		cf.crTextColor = rgb2bgr(color);
		_defaultCf.crTextColor = cf.crTextColor;
		SendMessage(getHwnd(), EM_SETSEL, 0xFFFFFFF, 0xFFFFFFF);
		SendMessage(getHwnd(), EM_SETCHARFORMAT, SCF_SELECTION, reinterpret_cast<LPARAM>(&cf));
	}

	void RichEdit::setBackgroundColor(COLORREF color) {
		SendMessage(getHwnd(), EM_SETBKGNDCOLOR, 0, static_cast<LPARAM>(rgb2bgr(color)));
	}

	void RichEdit::setSpecial(COLORREF color, DWORD effects, float size) {
		CHARFORMAT cf;
		memset(&cf, 0, sizeof(CHARFORMAT));
		cf.cbSize = sizeof(CHARFORMAT);
		cf.dwMask = CFM_BOLD|CFM_ITALIC|CFM_UNDERLINE|CFM_SIZE|CFM_COLOR;
		cf.crTextColor = color;
		cf.dwEffects = effects;
		cf.yHeight = static_cast<int>(size * _defaultCf.yHeight);

		SendMessage(getHwnd(), EM_SETSEL, 0xFFFFFFF, 0xFFFFFFF);
		SendMessage(getHwnd(), EM_SETCHARFORMAT, SCF_SELECTION, reinterpret_cast<LPARAM>(&cf));
	}

	void RichEdit::setNormal() {
		CHARFORMAT cf;
		memset(&cf, 0, sizeof(CHARFORMAT));
		cf.cbSize = sizeof(CHARFORMAT);
		cf.dwMask = CFM_BOLD|CFM_ITALIC|CFM_UNDERLINE|CFM_SIZE|CFM_COLOR;
		cf.dwEffects = CFE_AUTOCOLOR;
		cf.yHeight = _defaultCf.yHeight;

		SendMessage(getHwnd(), EM_SETSEL, 0xFFFFFFF, 0xFFFFFFF);
		SendMessage(getHwnd(), EM_SETCHARFORMAT, SCF_SELECTION, reinterpret_cast<LPARAM>(&cf));
	}

	void RichEdit::addHtml(const wchar_t* html) {
		size_t i = 0, j, len = wcslen(html);
		FastStrBuff tmp;
		bool insideTag = false;

		while(i < len) {
			switch(html[i]) {
			case L'\r':
			case L'\n':
				break;
			case L'<':
				if(tmp.size() > 0) {
					addText(tmp.c_str());
					tmp.clear();
				}
				insideTag = true;
				break;
			case L'>':
				insideTag = false;
				if(tmp.equalIC(L"b"))
					setBold(true);
				else if(tmp.equalIC(L"/b"))
					setBold(false);
				else if(tmp.equalIC(L"i"))
					setItalic(true);
				else if(tmp.equalIC(L"/i"))
					setItalic(false);
				else if(tmp.equalIC(L"u"))
					setUnderline(true);
				else if(tmp.equalIC(L"/u"))
					setUnderline(false);
				else if(tmp.equalIC(L"br") || tmp.equalIC(L"br/") || tmp.equalIC(L"br /")) {
					addText(L"\r\n");
					setFontColor(_defaultCf.crTextColor);
				}
				else if(tmp.size() > 4) {
					std::wstring tag = tmp.firstChars(3);
					if(tag == L"img" || tag == L"IMG")
						handleImg(tmp.c_str());
				}
				tmp.clear();
				break;
			case L'&':
				if(insideTag) {
					tmp.push_back(html[i]); // it's tag, don't care
					break;
				}
				if(len > i + 3) {
					// &lt;
					if(html[i+1] == L'l' && html[i+2] == L't' && html[i+3] == L';')	{
						addText(tmp.c_str());
						tmp.clear();
						addText(L"<");
						i += 3;
						break;
					}
					// &gt;
					else if(html[i+1] == L'g' && html[i+2] == L't' && html[i+3] == L';') {
						addText(tmp.c_str());
						tmp.clear();
						addText(L">");
						i += 3;
						break;
					}
				}
				if(len > i + 4) {
					// &amp;
					if(html[i+1] == L'a' && html[i+2] == L'm' && html[i+3] == L'p' && html[i+4] == L';') {
						addText(tmp.c_str());
						tmp.clear();
						addText(L"&");
						i += 4;
						break;
					}
				}
				if(len > i + 5) {
					// &quot;
					if(html[i+1] == L'q' && html[i+2] == L'u' && html[i+3] == L'o' && html[i+4] == L't' && html[i+5] == L';') {
						addText(tmp.c_str());
						tmp.clear();
						addText(L"\"");
						i += 5;
						break;
					} 
					// &nbsp;
					else if(html[i+1] == L'n' && html[i+2] == L'b' && html[i+3] == L's' && html[i+4] == L'p' && html[i+5] == L';') {
						addText(tmp.c_str());
						tmp.clear();
						addText(L" ");
						i += 5;
						break;
					}
					// &apos;
					else if(html[i+1] == L'a' && html[i+2] == L'p' && html[i+3] == L'o' && html[i+4] == L's' && html[i+5] == L';') {
						addText(tmp.c_str());
						tmp.clear();
						addText(L"'");
						i += 5;
						break;
					}
				}
				// &#xxxx;
				if(len > i + 6 && html[i+1] == L'#' && html[i+6] == L';') {
					wchar_t hex[4];
					bool valid = true;
					for(j=0; j<4; j++) {
						hex[j] = html[i+j+2];
						if(hex[j] < L'0' || hex[j] > L'9') {
							valid = false;
							break;
						}
						hex[j] -= 0x30;
					}
					addText(tmp.c_str());
					tmp.clear();
					if(valid) {
						wchar_t letter[2] = {0,0};
						letter[0] = hex[0]*1000 + hex[1]*100 + hex[2]*10 + hex[3];
						addText(letter);
					}
					i += 6;
					break;
				}
				tmp.push_back(html[i]); // if & not for special char
				break;
			default:
				tmp.push_back(html[i]);
			}
			i++;
		}
		if(tmp.size() > 0)
			addText(tmp.c_str());
	}

	LRESULT RichEdit::ParentWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, 
		UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
		if(msg==WM_NOTIFY && (((LPNMHDR)lParam)->code)== EN_LINK) {
			ENLINK* enLinkInfo = (ENLINK *)lParam;
			if(enLinkInfo->msg == WM_LBUTTONUP || enLinkInfo->msg == WM_RBUTTONUP) {
				LONG utlBeg = enLinkInfo->chrg.cpMin;
				LONG utlEnd = enLinkInfo->chrg.cpMax;
				if(utlEnd - utlBeg > 0) {
					HWND hRichEdit = enLinkInfo->nmhdr.hwndFrom;
					wchar_t* urlString = new wchar_t[utlEnd-utlBeg+1];
					SendMessageW(hRichEdit, EM_EXSETSEL, 0, reinterpret_cast<LPARAM>(&enLinkInfo->chrg));
					SendMessageW(hRichEdit, EM_GETSELTEXT, 0, reinterpret_cast<LPARAM>(urlString));
					switch(enLinkInfo->msg) {
					case WM_LBUTTONUP:
						ShellExecuteW(NULL, L"open", urlString, NULL, NULL, SW_SHOWNORMAL);
						break;
					case WM_RBUTTONUP:
						copyToClipboard(hRichEdit, urlString);
						break;
					}
					delete [] urlString;
					SendMessage(hRichEdit, EM_SETSEL, utlEnd, utlEnd);
					HideCaret(hRichEdit);
				}
			}
		}
		return DefSubclassProc(hWnd, msg, wParam, lParam);
	}

	LRESULT RichEdit::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, 
		UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
		if(msg == WM_LBUTTONUP)
			HideCaret(hWnd);
		else if(msg == WM_RBUTTONDOWN) {
			CHARFORMAT cf;
			memset(&cf, 0, sizeof(CHARFORMAT));
			cf.dwMask = CFM_LINK;
			SendMessage(hWnd, EM_GETCHARFORMAT, SCF_SELECTION, reinterpret_cast<LPARAM>(&cf));
			if(!(cf.dwEffects & CFE_LINK)) {
				DWORD selBeg, selEnd;
				SendMessageW(hWnd, EM_GETSEL, (WPARAM)&selBeg, (LPARAM)&selEnd);
				if(selEnd-selBeg > 0) {
					wchar_t* urlString = new wchar_t[selEnd-selBeg+1];
					SendMessageW(hWnd, EM_GETSELTEXT, 0, reinterpret_cast<LPARAM>(urlString));
					copyToClipboard(hWnd, urlString);
					delete [] urlString;
					SendMessageW(hWnd, EM_SETSEL, selEnd, selEnd);
				}
			}
			HideCaret(hWnd);
		}
		return DefSubclassProc(hWnd, msg, wParam, lParam);
	}

	void RichEdit::copyToClipboard(HWND hWnd, const wchar_t* str) {
		if (OpenClipboard(hWnd) == TRUE) {
			EmptyClipboard();
			HGLOBAL clipbuffer = GlobalAlloc(GMEM_MOVEABLE | GMEM_SHARE, (wcslen(str) + 1) << 1);
			wchar_t* buff = reinterpret_cast<wchar_t*>(GlobalLock(clipbuffer));
			lstrcpyW(buff, str);
			GlobalUnlock(clipbuffer);
			SetClipboardData(CF_UNICODETEXT, clipbuffer);
			CloseClipboard();
		}
	}
};
