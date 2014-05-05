#pragma once

#include <vector>
#include "FilePath.h"
#include "../Utils/BinaryFile.h"

namespace wtwUpdate {
	namespace updater {
		class Rollbackable {
			std::vector<utils::BinaryFile> _created;
			std::vector<utils::FileCopy> _existingCopies;
		protected:
			void rollback() {
				size_t i, len = _created.size();
				for (i = 0; i < len; i++)
					_created[i].del();
				len = _existingCopies.size();
				for (i = 0; i < len; i++)
					_existingCopies[i].bringBack();
			}

			void commit() {
				size_t j, len = _existingCopies.size();
				for (j = 0; j < len; j++)
					_existingCopies[j].del();
				_existingCopies.clear();
				_created.clear();
			}

			bool addCopy(const FilePath& path) {
				utils::FileCopy copy(path.getPath());
				if (!copy.created())
					return false;

				_existingCopies.push_back(copy);
				return true;
			}

			inline void addCreated(const utils::BinaryFile& f) {
				_created.push_back(f);
			}
		};
	}
}
