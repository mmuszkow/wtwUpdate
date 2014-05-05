#pragma once

#ifndef _MSC_VER
# error This code can be only compiled using Visual Studio
#endif

#define _CRT_SECURE_NO_DEPRECATE 1
#define _ATL_SECURE_NO_DEPRECATE 1
#define _CRT_NON_CONFORMING_SWPRINTFS 1

#include <WinSock2.h> // without this there was "windows.h" including error
#include <plInterface.h>

#include <string>
#include <sstream>
#include <map>
#include <vector>
#include "resource.h"

#ifdef _DEBUG
# define CRTDBG_MAP_ALLOC
# include <stdlib.h>
# include <crtdbg.h>
#endif

#pragma comment (lib, "Comctl32.lib")
#pragma comment (lib, "Gdiplus.lib")

#ifdef _M_IX86
#pragma comment (lib, "../API/libs/lib/libWTW_Release_Win32_libWTW.lib")
#pragma comment (lib, "../API/libs/lib/libZlib_Release_Win32_libZlib.lib")
#elif _M_X64
#pragma comment (lib, "../API/libs/lib/libWTW_Release_x64_libWTW.lib")
#pragma comment (lib, "../API/libs/lib/libZlib_Release_x64_libZlib.lib")
#else
#error Unknown arch
#endif

#define MDL	L"UPDT"

#include "cpp/Conv.h"

static WTWFUNCTIONS* fn = NULL;
static HINSTANCE hInst;

static std::wstring utow(const char* str) {
	wchar_t* w = wtw::CConv::utow(str);
	std::wstring ret(w);
	wtw::CConv::release(w);
	return ret;
}

static std::wstring utow(const std::string& str) {
	return utow(str.c_str());
}

static std::string wtou(const wchar_t* str) {
	char* s = wtw::CConv::wtou(str);
	std::string ret(s);
	wtw::CConv::release(s);
	return ret;
}

static std::string wtou(const std::wstring& str) {
	return wtou(str.c_str());
}

static void notify(const wchar_t* fmt, ...) {
	wtwTrayNotifyDef tray;

	// format
	va_list ap;
	va_start(ap, fmt);
	int len = _vscwprintf(fmt, ap) + 1;
	wchar_t* msg = new wchar_t[len + 1];
	vswprintf_s(msg, len, fmt, ap);
	va_end(ap);

	// show
	tray.textMain = L"Aktualizacja";
	tray.textLower = msg;
	tray.iconId = WTW_GRAPH_ID_UPDATE;
	tray.flags = WTW_TN_FLAG_OVERRIDE_TIME;
	tray.showTime = 3000;
	tray.graphType = WTW_TN_GRAPH_TYPE_SKINID;
	fn->fnCall(WTW_SHOW_STANDARD_NOTIFY, tray, NULL);
	delete[] msg;
}

#define LOG_ERR(fmt, ...) __LOG_F(fn, WTW_LOG_LEVEL_ERROR, MDL, fmt, __VA_ARGS__)
