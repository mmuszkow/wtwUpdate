#pragma once

#include <Windows.h>
#include <string>

namespace wtwUpdate {
	namespace utils {		
		class BinaryFile {
			FILE* _f;
			std::wstring _path;		
		public:
			BinaryFile() : _f(NULL) { }

			inline const std::wstring& getPath() const {
				return _path;
			}

			static boolean exists(const std::wstring& path);

			inline bool isOpen() const {
				return _f != NULL;
			}

			bool openTmp();
			static std::wstring tmpFn();
			bool open(const std::wstring& path, const wchar_t* mode);
			size_t read(void* buff, size_t s) const;
			bool write(const void* buff, size_t len);
			void close();

			bool del();

			static FILETIME dos2ft(unsigned long unix) {
				__int64 t = Int32x32To64(unix, 10000000) + 116444736000000000;
				FILETIME ft;
				ft.dwHighDateTime = (t >> 32) & 0xFFFFFFFF;
				ft.dwLowDateTime = t & 0xFFFFFFFF;
				return ft;
			}

			bool setModTime(unsigned long dosTime) {
				FILETIME modTime;
				if (DosDateTimeToFileTime((dosTime >> 16) & 0xFFFF, dosTime & 0xFFFF, &modTime) != TRUE)
					return false;

				HANDLE hFile = CreateFile(_path.c_str(), FILE_WRITE_ATTRIBUTES, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
				if (hFile == INVALID_HANDLE_VALUE)
					return false;

				bool ret = (SetFileTime(hFile, NULL, NULL, &modTime) == TRUE);
				CloseHandle(hFile);
				return ret;
			}
		};

		class FileCopy {
			std::wstring _origin;
			std::wstring _copy;
		public:
			FileCopy(const std::wstring& path) {
				std::wstring dst = BinaryFile::tmpFn();
				if (dst.size() == 0 || path.size() == 0)
					return;

				if (CopyFile(path.c_str(), dst.c_str(), FALSE) != FALSE) {
					_origin = path;
					_copy = dst;
				}
			}

			bool created() const {
				return _copy.size() > 0;
			}

			bool bringBack() {
				if (_copy.size() == 0)
					return false;

				if (CopyFile(_copy.c_str(), _origin.c_str(), TRUE) == 0)
					return false; // do not delete

				return del(); // return true? file was restored...
			}

			bool del() {
				if (_copy.size() == 0)
					return false;

				return (DeleteFile(_copy.c_str()) != 0);
			}
		};
	}
}
