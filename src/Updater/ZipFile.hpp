#pragma once

#include "Rollbackable.hpp"
#include "contrib/minizip/unzip.h"

namespace wtwUpdate {
	namespace updater {
		class ZipFile : public Rollbackable {
			unzFile _file;
		public:
			ZipFile(const std::wstring& fn);
			~ZipFile();

			bool isValid() const {
				return _file != NULL;
			}

			bool unzip();
		};
	}
}
