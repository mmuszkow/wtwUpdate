#pragma once

#include <set>
#include <stack>
#include "Addon.h"
#include "../Utils/Settings.h"

namespace wtwUpdate {
	namespace json {
		/// List of all addons in JSON file. Similar to AddonsTree but flat.
		class AddonsList : public std::vector<json::Addon> {
			void recAddon(wtw::CJson* json, const std::string& dir, const wtwUtils::Settings& s) {
				if (!json)
					return;

				if (json->isArray()) {
					size_t len = json->size();
					for (size_t i = 0; i < len; i++)
						recAddon(json->getAt(i), dir, s);
				} else {
					json::Addon addon(json, dir);
					addon.updateInstallationState(s);
					push_back(addon);
				}
			}

			void recSection(wtw::CJson* json, const std::string& dir, const wtwUtils::Settings& s) {
				if (!json)
					return;

				if (json->isArray()) {
					size_t len = json->size();
					for (size_t i = 0; i < len; i++) {
						recSection(json->getAt(i)->find("section"), dir, s);
						recAddon(json->getAt(i)->find("addon"), dir, s);
					}
				} else {
					wtw::CJson* newSection = json->find("section");
					std::string dir = newSection->find("dir")->c_str();
					recSection(newSection, dir, s);
					recAddon(json->find("addon"), dir, s);
				}
			}
		public:
			AddonsList() { }

			/// This class uses values stored in setting and needs valid WTWFUNCTIONS* pointer.
			AddonsList(wtw::CJson* json) {
				wtwUtils::Settings s;
				recSection(json, std::string(), s);
			}

			AddonsList(const std::vector<json::Addon>& list) : std::vector<json::Addon>(list) {	}



			std::vector<json::Addon> removeConflicted() {
				std::vector<json::Addon> ret;
				std::vector<json::Addon> ok;
				std::set<std::string> allFiles;

				// find conflicting addons
				size_t i, len = size();
				for (i = 0; i < len; i++) {
					const json::Addon& addon = this->at(i);
					const std::vector<json::File> files = addon.getFiles();
					size_t j, fLen = files.size();
					bool conflict = false;
					for (j = 0; j < fLen; j++) {
						const std::string& path = files[j].getPath();
						// conflict = file exists in other addon
						if (allFiles.find(path) != allFiles.end())
							conflict = true;
						allFiles.insert(path);
					}

					if (conflict)
						ret.push_back(addon);
					else
						ok.push_back(addon);
				}

				this->clear();
				this->assign(ok.begin(), ok.end());

				return ret;
			}
		};
	}
}
