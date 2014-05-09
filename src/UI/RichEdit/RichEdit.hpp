#pragma once

#include "UI/Control.hpp"
#include <string>
#include "ImageDataObject.hpp"

namespace MyRichEdit {
	class RichEdit : public wtwUpdate::ui::Control {
		bool			_scrolledDown;
		CHARFORMAT		_defaultCf;
#ifndef MY_RICHEDIT_NO_OLE
		// for images in RichEdit
		LPRICHEDITOLE	 _pRichEditOle;
		CImageDataObject _imgDataObj;
#endif
		// static's for external lib loading
		static HMODULE	 _hRichEditLib;
		static ULONG_PTR _gdiplusToken;

		// handles the html img tag
		void handleImg(std::wstring tag);

		// wnd procs
		static LRESULT CALLBACK ParentWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, 
			UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
		static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, 
			UINT_PTR uIdSubclass, DWORD_PTR dwRefData);

		inline static COLORREF rgb2bgr(COLORREF color) {
			return (((color & 0x000000FF) << 16) | (color & 0x0000FF00) | ((color & 0x00FF00FF) >> 16));
		}

		static void copyToClipboard(HWND hWnd, const wchar_t* str);
	public:
		RichEdit(HWND hControl);
		~RichEdit();

		/// Must be called before using RichEdit
		static void libInit();
		static void libDeinit();

		void assign(HWND hControl);

		bool scrollDown();
		bool scrollTop();
		bool isScrolledDown() const {
			return _scrolledDown;
		}

		void setFont(const wchar_t* fontName);
		void setBold(bool set);
		void setItalic(bool set);
		void setUnderline(bool set);
		void setFontColor(COLORREF color);
		void setBackgroundColor(COLORREF color);

		void RichEdit::setSpecial(COLORREF color, DWORD effects, float size);
		void setNormal();

		inline void clear() {
			SetWindowTextW(getHwnd(), L"");
			_scrolledDown = true;
		}
		void addText(const wchar_t* text) {
			SendMessage(getHwnd(), EM_SETSEL, 0xFFFFFFF, 0xFFFFFFF);
			SendMessage(getHwnd(), EM_REPLACESEL, FALSE, reinterpret_cast<LPARAM>(text));
			_scrolledDown = false;
		}
		void addHtml(const wchar_t* html);
		inline void setHtml(const wchar_t* html) {
			clear();
			addHtml(html);
		}
	};
}; // namespace wndSimple
