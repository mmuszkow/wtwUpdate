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

			static bool replace(std::wstring& str, const std::wstring& from, const std::wstring& to) {
				size_t start_pos = str.find(from);
				if (start_pos == std::wstring::npos)
					return false;
				str.replace(start_pos, from.length(), to);
				return true;
			}
		public:
			FilePath(const json::File& file) {
				wchar_t* wpath = wtw::CConv::utow(file.getPath().c_str());
				_path = wpath;
				wtw::CConv::release(wpath);

				replace(_path, L"__PLUGINS32__", _plugins32Dir);
				replace(_path, L"__PLUGINS64__", _plugins64Dir);
				std::replace(_path.begin(), _path.end(), L'/', L'\\');
			}

			static bool initDirPaths(WTWFUNCTIONS* fn) {
				wtwDirectoryInfo dirInfo;
				wchar_t pathBuff[MAX_PATH + 1];
				dirInfo.dirType = WTW_DIRECTORY_PROFILE;
				dirInfo.flags = WTW_DIRECTORY_FLAG_FULLPATH;
				dirInfo.bi.bufferSize = MAX_PATH;
				dirInfo.bi.pBuffer = pathBuff;
				if (fn->fnCall(WTW_GET_DIRECTORY_LOCATION, dirInfo, NULL) != S_OK) {
					// TODO: log
					return false;
				}

				wchar_t tmp[1024];
				swprintf_s(tmp, 1024, L"%sPlugins32", pathBuff);
				_plugins32Dir = tmp;
				swprintf_s(tmp, 1024, L"%sPlugins64", pathBuff);
				_plugins64Dir = tmp;

				return true;
			}

			inline const std::wstring& getPath() const {
				return _path;
			}
		};
	}
}
