#pragma once

#include "../stdinc.h"

#include <algorithm>

#include "../JsonObjs/File.h"

#include "cpp/Conv.h"

namespace wtwUpdate {
	namespace updater {
		class FilePath {
			std::wstring _path;

			static std::wstring _plugins32Dir;
			static std::wstring _plugins64Dir;
			static std::wstring _libs32Dir;
			static std::wstring _libs64Dir;
			static std::wstring _pluginDataDir;
			static std::wstring _themeDir;
			static std::wstring _langDir;

			static bool replace(std::wstring& str, const std::wstring& from, const std::wstring& to) {
				size_t start_pos = str.find(from);
				if (start_pos == std::wstring::npos)
					return false;
				str.replace(start_pos, from.length(), to);
				return true;
			}
		public:
			FilePath(const char* utf8path) {
				wchar_t* wpath = wtw::CConv::utow(utf8path);
				_path = wpath;
				wtw::CConv::release(wpath);

				// replace variables
				replace(_path, L"__PLUGINS32__", _plugins32Dir);
				replace(_path, L"__PLUGINS64__", _plugins64Dir);
				replace(_path, L"__PLUGINDATA__", _pluginDataDir);
				replace(_path, L"__THEME__", _themeDir);
				replace(_path, L"__LANG__", _langDir);
				replace(_path, L"__LIBS32__", _libs32Dir);
				replace(_path, L"__LIBS64__", _libs64Dir);

				std::replace(_path.begin(), _path.end(), L'/', L'\\');
			}
			
			FilePath(const json::File& file) : FilePath(file.getPath().c_str()) { }

			static bool initDirPaths() {
				wtwDirectoryInfo dirInfo;
				wchar_t pathBuff[MAX_PATH + 1];
				dirInfo.flags = WTW_DIRECTORY_FLAG_FULLPATH;
				dirInfo.bi.bufferSize = MAX_PATH;
				dirInfo.bi.pBuffer = pathBuff;

				// plugins directories
				dirInfo.dirType = WTW_DIRECTORY_PROFILE;
				if (fn->fnCall(WTW_GET_DIRECTORY_LOCATION, dirInfo, NULL) != S_OK) {
					LOG_ERR(L"Getting the WTW_DIRECTORY_PROFILE failed");
					return false;
				}
				wchar_t tmp[512];
				swprintf_s(tmp, 512, L"%sPlugins32", pathBuff);
				_plugins32Dir = tmp;
				swprintf_s(tmp, 512, L"%sPlugins64", pathBuff);
				_plugins64Dir = tmp;
				swprintf_s(tmp, 512, L"%sLibraries32", pathBuff);
				_libs32Dir = tmp;
				swprintf_s(tmp, 512, L"%sLibraries64", pathBuff);
				_libs64Dir = tmp;
				swprintf_s(tmp, 512, L"%sI18N", pathBuff);
				_langDir = tmp;


				// plugins data dir
				dirInfo.dirType = WTW_DIRECTORY_PLUGINDATA;
				if (fn->fnCall(WTW_GET_DIRECTORY_LOCATION, dirInfo, NULL) != S_OK) {
					LOG_ERR(L"Getting the WTW_DIRECTORY_PLUGINDATA failed");
					return false;
				}
				pathBuff[wcslen(pathBuff) - 1] = 0;
				_pluginDataDir = pathBuff;

				// theme dir
				dirInfo.dirType = WTW_DIRECTORY_USERTHEME;
				if (fn->fnCall(WTW_GET_DIRECTORY_LOCATION, dirInfo, NULL) != S_OK) {
					LOG_ERR(L"Getting the WTW_DIRECTORY_USERTHEME failed");
					return false;
				}
				pathBuff[wcslen(pathBuff) - 1] = 0;
				_themeDir = pathBuff;

				return true;
			}

			inline const std::wstring& getPath() const {
				return _path;
			}
		};
	}
}
