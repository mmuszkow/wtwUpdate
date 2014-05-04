#pragma once

#include <vector>

#include "File.h"
#include "Rev.h"
#include "Section.h"
#include "../Updater/FilePath.h"

#include "cpp/Conv.h"

namespace wtwUpdate {
	namespace json {
		class Addon : public Obj {
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

			static __int64 ft2unix(const FILETIME& ft) {
				LARGE_INTEGER date;
				date.HighPart = ft.dwHighDateTime;
				date.LowPart = ft.dwLowDateTime;
				date.QuadPart -= 116444736000000000;
				return date.QuadPart / 10000000;
			}
		public:
			Addon() {
				_time = _size = 0;
			}

			Addon(wtw::CJson* json, const char* dir) : Obj(json) {
				_id = getStr("id");
				_name = getStr("name");
				_author = getStr("author");
				_ver = getStr("ver");
				_desc = getStr("desc");
				_time = getInt("time");
				_size = getInt("size");
				_depends = getStrArray("depends");
				_files = getObjArray<File>("file");
				_revs = getObjArray<Rev>("rev");
				char zipUrl[1024];
				sprintf_s(zipUrl, 1024, "%s/%s-%u.zip", dir, _id.c_str(), _time);
				_zipUrl = zipUrl;
			}

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

			const std::vector<std::string>& getDependencies() const {
				return _depends;
			}

			const std::vector<File>& getFiles() const {
				return _files;
			}

			const std::vector<Rev>& getRevisions() const {
				return _revs;
			}

			const std::string& getZipUrl() const {
				return _zipUrl;
			}

			enum InstallState { NOT_INSTALLED, INSTALLED, MODIFIED };

			InstallState getInstallationState() const {
				size_t files = _files.size(), installedCount = 0;
				InstallState ret = NOT_INSTALLED;
				for (size_t i = 0; i < files; i++) {
					const File& f = _files[i];
					updater::FilePath path(f);					
					WIN32_FIND_DATA fData;
					HANDLE hFile = FindFirstFile(path.getPath().c_str(), &fData);
					// file doesn't exist
					if (hFile == INVALID_HANDLE_VALUE)
						continue;

					FindClose(hFile);					

					// size mismatch
					__int64 fSize = ((__int64)fData.nFileSizeHigh << 32) | fData.nFileSizeLow;
					if (fSize != f.getSize())
						return MODIFIED;

					// modification date mismatch, TODO: needs unzip to put valid mod date
					//__int64 mTime = ft2unix(fData.ftLastWriteTime);
					//if (mTime != f.getTime())
						//return MODIFIED;

					installedCount++;
				}

				if (installedCount == 0)
					return NOT_INSTALLED;
				if (installedCount == files)
					return INSTALLED;
				return MODIFIED;
			}
		};
	}
}
