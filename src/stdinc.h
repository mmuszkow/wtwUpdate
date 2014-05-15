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
#pragma comment (lib, "Wininet.lib")


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

extern WTWFUNCTIONS* fn;
extern HINSTANCE hInst;
extern HWND hMain;

#define LOG_ERR(fmt, ...) __LOG_F(fn, WTW_LOG_LEVEL_ERROR, MDL, fmt, __VA_ARGS__)

/** Converts the std::string with defined encoding into the UTF-16 std::wstring */
static std::wstring stow(const std::string& str, UINT encoding = CP_UTF8) {
	int len = MultiByteToWideChar(encoding, 0, str.c_str(), -1, NULL, 0);
	if (len <= 0) return L"";

	wchar_t* buff = new wchar_t[len + 1];
	MultiByteToWideChar(encoding, 0, str.c_str(), -1, buff, len);
	buff[len] = 0;
	std::wstring res(buff);
	delete[] buff;
	return res;
}

/** Converts the UTF-16 std::wstring into the std::string with chosen encoding */
static std::string wtos(const std::wstring& str, UINT encoding = CP_UTF8) {
	int len = WideCharToMultiByte(encoding, 0, str.c_str(), -1, NULL, 0, 0, 0);
	if (len <= 0) return "";

	char* buff = new char[len + 1];
	WideCharToMultiByte(encoding, 0, str.c_str(), -1, buff, len, 0, 0);
	buff[len] = 0;
	std::string res(buff);
	delete[] buff;
	return res;
}

static void notifyEx(DWORD time, WTWFUNCTION callback, void* cbData, const wchar_t* fmt, ...) {
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
	tray.showTime = time;
	tray.graphType = WTW_TN_GRAPH_TYPE_SKINID;
	tray.callback = callback;
	tray.cbData = cbData;
	fn->fnCall(WTW_SHOW_STANDARD_NOTIFY, tray, NULL);
	delete[] msg;
}

#define notify(fmt, ...) notifyEx(3000, NULL, NULL, fmt, __VA_ARGS__)
