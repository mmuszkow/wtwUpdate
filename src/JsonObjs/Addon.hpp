#pragma once

#include <vector>
#include "File.hpp"
#include "Rev.hpp"
#include "Utils/Settings.hpp"

namespace wtwUpdate {
	namespace json {
		class Addon : public Obj {
		public:
			enum InstallationState { UNKNOWN, NOT_INSTALLED, INSTALLED, MODIFIED };
		private:
			std::string _id;
			std::string _name;
			std::string _author;
			std::string _ver;
			std::string _desc;
			__int64 _time;
			__int64 _size;
			std::vector<std::string> _depends;
			std::vector<File> _files;
			std::vector<Rev> _revs;
			std::string _zipUrl;
			InstallationState _installationState;

			static __int64 ft2unix(const FILETIME& ft);
		public:
			Addon();
			Addon(wtw::CJson* json, const std::string& dir);

			inline const std::string& getId() const {
				return _id;
			}

			inline const std::string& getName() const {
				return _name;
			}

			inline const std::string& getAuthor() const {
				return _author;
			}

			inline const std::string& getVer() const {
				return _ver;
			}

			inline const std::string& getDesc() const {
				return _desc;
			}

			inline __int64 getTime() const {
				return _time;
			}

			inline __int64 getSize() const {
				return _size;
			}

			inline const std::vector<std::string>& getDependencies() const {
				return _depends;
			}

			inline const std::vector<File>& getFiles() const {
				return _files;
			}

			inline const std::vector<Rev>& getRevisions() const {
				return _revs;
			}

			inline const std::string& getZipUrl() const {
				return _zipUrl;
			}

			inline InstallationState getState() const {
				return _installationState;
			}

			void updateInstallationState(const wtwUtils::Settings& s);
		};
	}
}
