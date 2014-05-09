#include "stdinc.h"
#include "FileCopy.hpp"
#include "BinaryFile.hpp"

namespace wtwUpdate {
	namespace utils {
		FileCopy::FileCopy(const std::wstring& path) {
			std::wstring dst = BinaryFile::tmpFn();
			if (dst.size() == 0 || path.size() == 0)
				return;

			if (CopyFile(path.c_str(), dst.c_str(), FALSE) != FALSE) {
				_origin = path;
				_copy = dst;
			}
		}

		bool FileCopy::bringBack() {
			if (_copy.size() == 0)
				return false;

			if (CopyFile(_copy.c_str(), _origin.c_str(), TRUE) == 0)
				return false; // do not delete

			return del(); // return true? file was restored...
		}

		bool FileCopy::del() {
			if (_copy.size() == 0)
				return false;

			return (DeleteFile(_copy.c_str()) != 0);
		}
	}
}
