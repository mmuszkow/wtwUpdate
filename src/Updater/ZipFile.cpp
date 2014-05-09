#include "stdinc.h"
#include "ZipFile.hpp"
#include "FilePath.hpp"
#include "Utils/Dir.hpp"

namespace wtwUpdate {
	namespace updater {
		ZipFile::ZipFile(const std::wstring& fn) {
			_file = unzOpen(wtou(fn).c_str()); // TODO: unzOpen is utf8?
		}

		ZipFile::~ZipFile() {
			if (_file) unzClose(_file);
		}

		bool ZipFile::unzip() {
			if (!_file)
				return false;

			unz_global_info global_info;
			if (unzGetGlobalInfo(_file, &global_info) != UNZ_OK) {
				LOG_ERR(L"unzGetGlobalInfo failed");
				return false;
			}

			uLong i;
			char read_buffer[16384];
			for (i = 0; i < global_info.number_entry; ++i) {
				// Get info about current file.
				unz_file_info file_info;
				char filename[MAX_PATH + 1];
				if (unzGetCurrentFileInfo(_file, &file_info, filename, MAX_PATH, NULL, 0, NULL, 0) != UNZ_OK) {
					LOG_ERR(L"unzGetCurrentFileInfo failed");
					rollback();
					return false;
				}

				// Check if this entry is a directory or file.
				FilePath path(filename);
				char lastC = filename[strlen(filename) - 1];
				if (lastC != '\\' && lastC != '/')	{
					utils::Dir dir(path.getPath());
					if (!dir.exists() && !dir.create()) {
						LOG_ERR(L"Could not create the directory for %s", path.getPath().c_str());
						rollback();
						return false;
					}

					if (unzOpenCurrentFile(_file) != UNZ_OK) {
						LOG_ERR(L"unzOpenCurrentFile failed for %s", path.getPath().c_str());
						rollback();
						return false;
					}

					// open a file to write out the data
					// TODO: set modification date included in zip file

					// make a copy of existing file
					if (utils::BinaryFile::exists(path.getPath())) {
						if (!addCopy(path)) {
							LOG_ERR(L"Coping existing file %s failed", path.getPath().c_str());
							unzCloseCurrentFile(_file);
							rollback();
							return false;
						}
					}

					unz_file_info info;
					if (unzGetCurrentFileInfo(_file, &info, NULL, 0, NULL, 0, NULL, 0) != UNZ_OK) {
						LOG_ERR(L"unzGetCurrentFileInfo failed for %s", path.getPath().c_str());
						unzCloseCurrentFile(_file);
						rollback();
						return false;
					}
					int a = info.dosDate;

					// unpack
					utils::BinaryFile out;
					if (!out.open(path.getPath(), L"wb")) {
						LOG_ERR(L"Creating file %s failed", path.getPath().c_str());
						unzCloseCurrentFile(_file);
						rollback();
						return false;
					}
					addCreated(out);

					int error = UNZ_OK;
					do {
						error = unzReadCurrentFile(_file, read_buffer, 16384);
						if (error < 0) {
							LOG_ERR(L"unzReadCurrentFile failed for %s", path.getPath().c_str());
							unzCloseCurrentFile(_file);
							rollback();
							return false;
						}

						if (error > 0)
							out.write(read_buffer, error); // TODO: check return of fwrite...
					} while (error > 0);

					out.close();

					if (!out.setModTime(info.dosDate)) {
						LOG_ERR(L"Setting modifiaction time for file %s failed", path.getPath().c_str());
						unzCloseCurrentFile(_file);
						rollback();
						return false;
					}
				}

				unzCloseCurrentFile(_file);

				// go the the next entry listed in the zip file
				if ((i + 1) < global_info.number_entry)	{
					if (unzGoToNextFile(_file) != UNZ_OK)	{
						LOG_ERR(L"unzGoToNextFile failed");
						rollback();
						return false;
					}
				}
			}

			commit();

			return true;
		}
	}
}
