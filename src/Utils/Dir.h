#pragma once

#include <Windows.h>
#include <string>
#include <algorithm>

namespace wtwUpdate {
	namespace utils {
		class Dir {
			std::wstring _path;
		public:
			Dir(const std::wstring& path) {
				size_t lastSlash = path.find_last_of('\\');
				size_t lastBackslach = path.find_last_of('/');
				size_t last;
				if (lastSlash != std::wstring::npos && lastBackslach != std::wstring::npos)
					last = std::max<size_t>(lastSlash, lastBackslach);
				else if (lastSlash != std::wstring::npos)
					last = lastSlash;
				else
					last = lastBackslach;

				if (last != std::wstring::npos)
					_path = path.substr(0, last);
				else
					_path = path;
			}

			bool exists() const {
				DWORD dwAttrib = GetFileAttributes(_path.c_str());
				return (dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
			}

			bool create() {
				// TODO: nested directories
				return (_wmkdir(_path.c_str()) == 0);
			}
		};
	}
}
