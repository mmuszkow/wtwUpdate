#pragma once

#include <string>

namespace wtwUpdate {
	namespace utils {
		class FileCopy {
			std::wstring _origin;
			std::wstring _copy;
		public:
			FileCopy(const std::wstring& path);

			bool created() const {
				return _copy.size() > 0;
			}

			bool bringBack();
			bool del();
		};
	}
}
