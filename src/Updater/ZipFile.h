#pragma once

#include <Windows.h>

#include "../../lib/zlib-1.2.8/contrib/minizip/unzip.h"

#include "cpp/Conv.h"

#include "FilePath.h"

namespace wtwUpdate {
	namespace updater {
		class ZipFile {
			unzFile _file;

			// TODO: if unzipping of any file failed and file existed before, restores it to previous version
			void rollback() {				
			}
		public:
			ZipFile(const std::wstring& fn) {
				_file = unzOpen(wtou(fn).c_str()); // TODO: unzOpen is utf8?
			}

			bool isValid() const {
				return _file != NULL;
			}

			bool unzip() {
				if (!_file)
					return false;

				unz_global_info global_info;
				if (unzGetGlobalInfo(_file, &global_info) != UNZ_OK)
					return false;

				uLong i;
				char read_buffer[16384];
				for (i = 0; i < global_info.number_entry; ++i) {
					// Get info about current file.
					unz_file_info file_info;
					char filename[MAX_PATH + 1];
					if (unzGetCurrentFileInfo(_file, &file_info, filename, MAX_PATH, NULL, 0, NULL, 0) != UNZ_OK) {
						rollback();
						return false;
					}

					// Check if this entry is a directory or file.
					char lastC = filename[strlen(filename) - 1];
					if (lastC != '\\' && lastC != '/')	{
						FilePath path(filename);

						if (!path.createDirs()) {
							rollback();
							return false;
						}

						if (unzOpenCurrentFile(_file) != UNZ_OK) {
							rollback();
							return false;
						}

						// open a file to write out the data
						// TODO: set modification date included in zip file
						FILE *out = _wfopen(path.getPath().c_str(), L"wb");
						if (out == NULL) {
							unzCloseCurrentFile(_file);
							rollback();
							return false;
						}

						int error = UNZ_OK;
						do {
							error = unzReadCurrentFile(_file, read_buffer, 16384);
							if (error < 0) {
								unzCloseCurrentFile(_file);
								rollback();
								return false;
							}

							if (error > 0)
								fwrite(read_buffer, error, 1, out); // TODO: check return of fwrite...
						} while (error > 0);

						fclose(out);
					}

					unzCloseCurrentFile(_file);

					// go the the next entry listed in the zip file
					if ((i + 1) < global_info.number_entry)	{
						if (unzGoToNextFile(_file) != UNZ_OK)	{
							rollback();
							return false;
						}
					}
				}

				return true;
			}

			~ZipFile() {
				if(_file) unzClose(_file);
			}
		};
	}
}
