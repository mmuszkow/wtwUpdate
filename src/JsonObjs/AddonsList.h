#pragma once

#include "Addon.h"

namespace wtwUpdate {
	namespace json {
		/// List of all addons in JSON file.
		class AddonsList : public std::vector<json::Addon> {
			void recAddon(wtw::CJson* json) {
				if (!json)
					return;

				if (json->isArray()) {
					size_t len = json->size();
					for (size_t i = 0; i < len; i++)
						recAddon(json->getAt(i));
				} else {
					json::Addon addon(json, "");
					push_back(addon);
				}
			}

			void recSection(wtw::CJson* json) {
				if (!json)
					return;

				if (json->isArray()) {
					size_t len = json->size();
					for (size_t i = 0; i < len; i++) {
						recSection(json->getAt(i)->find("section"));
						recAddon(json->getAt(i)->find("addon"));
					}
				} else {
					recSection(json->find("section"));
					recAddon(json->find("addon"));
				}
			}
		public:
			AddonsList(wtw::CJson* json) {
				recSection(json);
			}
		};
	}
}
