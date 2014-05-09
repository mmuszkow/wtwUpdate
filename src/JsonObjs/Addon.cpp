#include "stdinc.h"
#include "Addon.hpp"
#include "Updater/FilePath.hpp"

namespace wtwUpdate {
	namespace json {
		__int64 Addon::ft2unix(const FILETIME& ft) {
			LARGE_INTEGER date;
			date.HighPart = ft.dwHighDateTime;
			date.LowPart = ft.dwLowDateTime;
			date.QuadPart -= 116444736000000000;
			return date.QuadPart / 10000000;
		}

		Addon::Addon() {
			_time = _size = 0;
			_installationState = UNKNOWN;
		}

		Addon::Addon(wtw::CJson* json, const std::string& dir) : Obj(json) {
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
			sprintf_s(zipUrl, 1024, "%s/%s-%u.zip", dir.c_str(), _id.c_str(), _time);
			_zipUrl = zipUrl;
			_installationState = UNKNOWN;
		}

		void Addon::updateInstallationState(const wtwUtils::Settings& s) {
			if (s.getInt64(utow(_id).c_str(), 0) == 0) {
				_installationState = NOT_INSTALLED;
				return;
			}

			size_t len = _files.size();
			for (size_t i = 0; i < len; i++) {
				const json::File& f = _files[i];
				updater::FilePath path(f);
				WIN32_FIND_DATA fData;
				HANDLE hFile = FindFirstFile(path.getPath().c_str(), &fData);
				// file doesn't exist
				if (hFile == INVALID_HANDLE_VALUE) {
					_installationState = MODIFIED;
					return;
				}

				FindClose(hFile);

				// size mismatch
				__int64 fSize = ((__int64)fData.nFileSizeHigh << 32) | fData.nFileSizeLow;
				if (fSize != f.getSize()) {
					_installationState = MODIFIED;
					return;
				}

				// modification date mismatch, TODO: needs unzip to put valid mod date
				//__int64 mTime = ft2unix(fData.ftLastWriteTime);
				//if (mTime != f.getTime()) {
				//_installationState = MODIFIED;
				//return;		}
			}

			_installationState = INSTALLED;
		}
	}
}
