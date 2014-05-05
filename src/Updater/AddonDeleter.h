#pragma once

#include "Rollbackable.h"
#include "../JsonObjs/Addon.h"

namespace wtwUpdate {
	namespace updater {
		class AddonDeleter : public Rollbackable {
			const json::Addon& _addon;
		public:
			AddonDeleter(const json::Addon& addon) : _addon(addon) { }

			bool del(wtwUtils::Settings& s) {
				const std::vector<json::File>& files = _addon.getFiles();
				size_t j, fLen = files.size();
				for (j = 0; j < fLen; j++) {
					FilePath fp(files[j].getPath().c_str());
					if (!addCopy(fp)) {
						LOG_ERR(L"Making copy of %s failed", fp.getPath().c_str());
						rollback();
						return false;
					}
					
					if (DeleteFile(fp.getPath().c_str()) != TRUE) {
						LOG_ERR(L"Deleting %s failed", fp.getPath().c_str());
						rollback();
						return false;
					}
				}

				s.setInt64(utow(_addon.getId()).c_str(), 0);
				s.write();
				commit();
				return true;
			}
		};
	}
}
