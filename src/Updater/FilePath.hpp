#pragma once

#include "JsonObjs/File.hpp"

namespace wtwUpdate {
	namespace updater {
		class FilePath {
			std::wstring _path;

			static std::wstring _profileDir;
			static std::wstring _plugins32Dir;
			static std::wstring _plugins64Dir;
			static std::wstring _libs32Dir;
			static std::wstring _libs64Dir;
			static std::wstring _pluginDataDir;
			static std::wstring _themeDir;
			static std::wstring _langDir;

			static bool replace(std::wstring& str, const std::wstring& from, const std::wstring& to);
		public:
			static bool initDirPaths();

			FilePath(const char* utf8path);			
			FilePath(const json::File& file);

			inline const std::wstring& getPath() const {
				return _path;
			}
		};
	}
}
