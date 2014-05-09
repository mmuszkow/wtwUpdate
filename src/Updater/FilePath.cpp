#include "stdinc.h"
#include "FilePath.hpp"
#include <algorithm>

namespace wtwUpdate {
	namespace updater {
		std::wstring FilePath::_profileDir;
		std::wstring FilePath::_plugins32Dir;
		std::wstring FilePath::_plugins64Dir;
		std::wstring FilePath::_pluginDataDir;
		std::wstring FilePath::_libs32Dir;
		std::wstring FilePath::_libs64Dir;
		std::wstring FilePath::_themeDir;
		std::wstring FilePath::_langDir;

		bool FilePath::replace(std::wstring& str, const std::wstring& from, const std::wstring& to) {
			size_t start_pos = str.find(from);
			if (start_pos == std::wstring::npos)
				return false;
			str.replace(start_pos, from.length(), to);
			return true;
		}

		FilePath::FilePath(const char* utf8path) {
			_path = utow(utf8path);

			// replace variables
			replace(_path, L"__PLUGINS32__", _plugins32Dir);
			replace(_path, L"__PLUGINS64__", _plugins64Dir);
			replace(_path, L"__PLUGINDATA__", _pluginDataDir);
			replace(_path, L"__THEME__", _themeDir);
			replace(_path, L"__LANG__", _langDir);
			replace(_path, L"__LIBS32__", _libs32Dir);
			replace(_path, L"__LIBS64__", _libs64Dir);
			replace(_path, L"__PROFILE__", _profileDir);			

			std::replace(_path.begin(), _path.end(), L'/', L'\\');
		}

		FilePath::FilePath(const json::File& file) : FilePath(file.getPath().c_str()) { }

		bool FilePath::initDirPaths() {
			wtwDirectoryInfo dirInfo;
			wchar_t pathBuff[MAX_PATH + 1];
			dirInfo.flags = WTW_DIRECTORY_FLAG_FULLPATH;
			dirInfo.bi.bufferSize = MAX_PATH;
			dirInfo.bi.pBuffer = pathBuff;

			// profile dir
			dirInfo.dirType = WTW_DIRECTORY_PROFILE;
			if (FAILED(fn->fnCall(WTW_GET_DIRECTORY_LOCATION, dirInfo, NULL))) {
				LOG_ERR(L"Getting the WTW_DIRECTORY_PROFILE failed");
				return false;
			}
			_profileDir = pathBuff;

			// plugins, libs and lang dir
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
			if (FAILED(fn->fnCall(WTW_GET_DIRECTORY_LOCATION, dirInfo, NULL))) {
				LOG_ERR(L"Getting the WTW_DIRECTORY_PLUGINDATA failed");
				return false;
			}
			pathBuff[wcslen(pathBuff) - 1] = 0;
			_pluginDataDir = pathBuff;

			// user theme dir
			dirInfo.dirType = WTW_DIRECTORY_USERTHEME;
			if (FAILED(fn->fnCall(WTW_GET_DIRECTORY_LOCATION, dirInfo, NULL))) {
				LOG_ERR(L"Getting the WTW_DIRECTORY_USERTHEME failed");
				return false;
			}
			pathBuff[wcslen(pathBuff) - 1] = 0;
			_themeDir = pathBuff;

			return true;
		}
	}
}
