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
			const Section* _parent;

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

			static __int64 ft2unix(const FILETIME& ft) {
				LARGE_INTEGER date, adjust;
				date.HighPart = ft.dwHighDateTime;
				date.LowPart = ft.dwLowDateTime;
				adjust.QuadPart = 11644473600000 * 10000;
				date.QuadPart -= adjust.QuadPart;
				return date.QuadPart / 10000000;
			}
		public:
			Addon() {
				_time = _size = 0;
				_parent = NULL;
			}

			Addon(wtw::CJson* json, const Section& section) : Obj(json) {
				_parent = &section; // TODO: is this safe? maybe a copy is better?
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
			}

			const Section* getParent() const {
				return _parent;
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

			enum InstallState { NOT_INSTALLED, INSTALLED, MODIFIED };

			InstallState getInstallationState() const {
				size_t files = _files.size();
				InstallState ret = NOT_INSTALLED;
				for (size_t i = 0; i < files; i++) {
					const File& f = _files[i];
					updater::FilePath path(f);					
					WIN32_FIND_DATA fData;
					HANDLE hFile = FindFirstFile(path.getPath().c_str(), &fData);
					// file doesn't exist
					if (hFile == INVALID_HANDLE_VALUE) {
						continue;
					}

					FindClose(hFile);

					// size mismatch
					__int64 fSize = ((__int64)fData.nFileSizeHigh << 32) | fData.nFileSizeLow;
					if (fSize != f.getSize()) {
						return MODIFIED;
					}

					// modification date mismatch
					__int64 mTime = ft2unix(fData.ftLastWriteTime);
					if (mTime != f.getTime())
						return MODIFIED;

					ret = INSTALLED;
				}
				return ret;
			}
		};
	}
}
