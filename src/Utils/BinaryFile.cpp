#include "stdinc.h"
#include "BinaryFile.hpp"

namespace wtwUpdate {
	namespace utils {		
		bool BinaryFile::exists(const std::wstring& path) {
			BinaryFile tmp;
			if (!tmp.open(path, L"rb"))
				return false;
			tmp.close();
			return true;
		}

		bool BinaryFile::openTmp() {
			std::wstring fn = tmpFn();
			if (fn.size() > 0)
				return open(fn, L"wb");
			return false;
		}

		std::wstring BinaryFile::tmpFn() {
			wchar_t path[MAX_PATH + 1], fn[MAX_PATH + 1];
			if (!GetTempPath(MAX_PATH, path))
				return L"";
			if (GetTempFileName(path, L"wtw", 0, fn))
				return fn;
			return L"";
		}

		bool BinaryFile::open(const std::wstring& path, const wchar_t* mode) {
			close();

			if (_wfopen_s(&_f, path.c_str(), mode) != 0) {
				_f = NULL;
				return false;
			}

			_path = path;
			return true;
		}

		bool BinaryFile::del() {
			std::wstring fp = _path;
			close();
			return (DeleteFile(fp.c_str()) != 0);
		}

		void BinaryFile::close() {
			if (_f) {
				fclose(_f);
				_f = NULL;
			}
		}

		size_t BinaryFile::read(void* buff, size_t s) const {
			if (!_f)
				return -1;

			return fread(buff, 1, s, _f);
		}

		bool BinaryFile::write(const void* buff, size_t len) {
			if (!_f)
				return false;
			return (fwrite(buff, 1, len, _f) == len);
		}

		FILETIME BinaryFile::dos2ft(unsigned long unix) {
			__int64 t = Int32x32To64(unix, 10000000) + 116444736000000000;
			FILETIME ft;
			ft.dwHighDateTime = (t >> 32) & 0xFFFFFFFF;
			ft.dwLowDateTime = t & 0xFFFFFFFF;
			return ft;
		}

		bool BinaryFile::setModTime(unsigned long dosTime) {
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
	}
}
