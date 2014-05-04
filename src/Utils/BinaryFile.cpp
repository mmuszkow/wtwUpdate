#include "BinaryFile.h"

namespace wtwUpdate {
	namespace utils {		
		boolean BinaryFile::exists(const std::wstring& path) {
			BinaryFile tmp;
			if (!tmp.open(path, L"rb"))
				return false;
			tmp.close();
			return true;
		}

		bool BinaryFile::openTmp() {
			wchar_t path[MAX_PATH + 1], fn[MAX_PATH + 1];
			if (!GetTempPath(MAX_PATH, path))
				return false;

			if (GetTempFileName(path, L"wtw", 0, fn))
				return open(fn, L"wb");

			return false;
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

		bool BinaryFile::remove() {
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

		bool BinaryFile::copy(FileCopy& out) {
			if (!out.isOpen())
				return false;

			out.setOriginalPath(_path);

			char copyBuff[16384];
			while (1) {
				size_t ret = read(copyBuff, 16384);
				if (ret == -1) {
					out.remove();
					return false;
				}

				if (ret == 0)
					break;

				if (!out.write(copyBuff, ret)) {
					out.remove();
					return false;
				}
			}

			out.close();
			return true;
		}
	}
}
