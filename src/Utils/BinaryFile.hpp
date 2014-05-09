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

			static bool exists(const std::wstring& path);

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

			static FILETIME dos2ft(unsigned long unix);
			bool setModTime(unsigned long dosTime);
		};
	}
}
