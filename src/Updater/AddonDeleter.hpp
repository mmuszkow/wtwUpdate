#pragma once

#include "Rollbackable.hpp"
#include "JsonObjs/Addon.hpp"

namespace wtwUpdate {
	namespace updater {
		class AddonDeleter : public Rollbackable {
			const json::Addon& _addon;
		public:
			AddonDeleter(const json::Addon& addon);

			bool del(wtwUtils::Settings& s);
		};
	}
}
