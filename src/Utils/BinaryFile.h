#pragma once

#include <Windows.h>
#include <string>

namespace wtwUpdate {
	namespace utils {
		class FileCopy;

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
			bool open(const std::wstring& path, const wchar_t* mode);
			size_t read(void* buff, size_t s) const;
			bool write(const void* buff, size_t len);
			void close();

			bool remove();
			bool copy(FileCopy& out);
		};

		class FileCopy : public BinaryFile {
			std::wstring _originalPath;
		public:
			inline void setOriginalPath(const std::wstring& path) {
				_originalPath = path;
			}

			inline const std::wstring getOriginalPath() const {
				return _originalPath;
			}
		};
	}
}
