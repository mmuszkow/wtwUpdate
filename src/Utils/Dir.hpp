#pragma once

#include <string>

namespace wtwUpdate {
	namespace utils {
		class Dir {
			std::wstring _path;
		public:
			Dir(const std::wstring& path);

			bool exists() const;
			bool create();
		};
	}
}
