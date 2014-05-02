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

#ifdef _M_IX86
#pragma comment (lib,"../API/libs/lib/libWTW_Release_Win32_libWTW.lib")
#pragma comment (lib,"../API/libs/lib/libZlib_Release_Win32_libZlib.lib")
#elif _M_X64
#pragma comment (lib,"../API/libs/lib/libWTW_Release_x64_libWTW.lib")
#pragma comment (lib,"../API/libs/lib/libZlib_Release_x64_libZlib.lib")
#else
#error Unknown arch
#endif

#define MDL	L"UPDT"