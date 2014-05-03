#pragma once

#include <string>
#include <Windows.h>

namespace wtwUpdate {
	namespace updater {
		class TmpFile {
			std::wstring _path;
		public:
			TmpFile() {
				wchar_t path[MAX_PATH + 1], fn[MAX_PATH + 1];
				if (!GetTempPath(MAX_PATH, path))
					return;

				if (GetTempFileName(path, L"wtwUpdate-", 0, fn))
					_path = fn;
			}

			const std::wstring& getPath() const {
				return _path;
			}

			bool isValid() const {
				return _path.size() > 0;
			}

			bool write(const void* buff, size_t len) {
				if (!isValid())
					return false;

				FILE* f = _wfopen(_path.c_str(), L"wb");
				if (!f)
					return false;

				if (fwrite(buff, 1, len, f) != len) {
					fclose(f);
					return false;
				}

				fclose(f);
				return true;
			}

			void remove() {
				if (!isValid())
					return;

				DeleteFile(_path.c_str());
				_path = L"";
			}
		};
	}
}
