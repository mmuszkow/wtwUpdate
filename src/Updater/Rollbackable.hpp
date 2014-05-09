#pragma once

#include <vector>
#include "Utils/BinaryFile.hpp"
#include "Utils/FileCopy.hpp"
#include "FilePath.hpp"

namespace wtwUpdate {
	namespace updater {
		class Rollbackable {
			std::vector<utils::BinaryFile> _created;
			std::vector<utils::FileCopy> _existingCopies;
		protected:
			virtual ~Rollbackable() { }

			void rollback();
			void commit();

			bool addCopy(const FilePath& path);
			inline void addCreated(const utils::BinaryFile& f) {
				_created.push_back(f);
			}
		};
	}
}
