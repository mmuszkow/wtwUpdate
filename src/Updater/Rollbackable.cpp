#include "stdinc.h"
#include "Rollbackable.hpp"

namespace wtwUpdate {
	namespace updater {		
		void Rollbackable::rollback() {
			size_t i, len = _created.size();
			for (i = 0; i < len; i++)
				_created[i].del();
			len = _existingCopies.size();
			for (i = 0; i < len; i++)
				_existingCopies[i].bringBack();
		}

		void Rollbackable::commit() {
			size_t j, len = _existingCopies.size();
			for (j = 0; j < len; j++)
				_existingCopies[j].del();
			_existingCopies.clear();
			_created.clear();
		}

		bool Rollbackable::addCopy(const FilePath& path) {
			utils::FileCopy copy(path.getPath());
			if (!copy.created())
				return false;

			_existingCopies.push_back(copy);
			return true;
		}
	}
}
