#include "stdinc.h"
#include "AddonsList.hpp"
#include <set>
#include <stack>

namespace wtwUpdate {
	namespace json {
		AddonsList::AddonsList(wtw::CJson* json) {
			wtwUtils::Settings s;
			recSection(json, std::string(), s);
		}

		AddonsList::AddonsList(bds_node* bson) {
			wtwUtils::Settings s;
			recSection(bson, std::string(), s);
		}

		void AddonsList::recAddon(wtw::CJson* json, const std::string& dir, const wtwUtils::Settings& s) {
			if (!json)
				return;

			if (json->isArray()) {
				size_t len = json->size();
				for (size_t i = 0; i < len; i++)
					recAddon(json->getAt(i), dir, s);
			}
			else {
				json::Addon addon(json, dir);
				addon.updateInstallationState(s);
				push_back(addon);
			}
		}

		void AddonsList::recSection(wtw::CJson* json, const std::string& dir, const wtwUtils::Settings& s) {
			if (!json)
				return;

			if (json->isArray()) {
				size_t len = json->size();
				for (size_t i = 0; i < len; i++) {
					recSection(json->getAt(i)->find("section"), dir, s);
					recAddon(json->getAt(i)->find("addon"), dir, s);
				}
			}
			else {
				wtw::CJson* newSection = json->find("section");
				std::string newDir = newSection->find("dir")->c_str();
				if (newDir.size() == 0)
					newDir = dir;
				recSection(newSection, newDir, s);
				recAddon(json->find("addon"), newDir, s);
			}
		}	

		void AddonsList::recAddon(bds_node* bson, const std::string& dir, const wtwUtils::Settings& s) {
			switch (bdf_elem_type(bson)) {
			case E_DOC: {
				json::Addon addon(bson, dir);
				addon.updateInstallationState(s);
				push_back(addon);
				break;
			}
			case E_ARRAY:
				bson = bson->elem.data.node;
				while (bson) {
					recAddon(bson, dir, s);
					bson = bson->next;
				}
				break;
			}
		}

		void AddonsList::recSection(bds_node* bson, const std::string& dir, const wtwUtils::Settings& s) {
			switch (bdf_elem_type(bson)) {
			case E_DOC: {
				char* newDir;
				if (bdf_elem_str(bdf_elem_child(bson, "dir"), &newDir)) {
					recSection(bdf_elem_child(bson, "section"), newDir, s);
					recAddon(bdf_elem_child(bson, "addon"), newDir, s);
				} else {
					recSection(bdf_elem_child(bson, "section"), dir, s);
					recAddon(bdf_elem_child(bson, "addon"), dir, s);
				}
				break;
			}
			case E_ARRAY:
				bson = bson->elem.data.node;
				while (bson) {
					recSection(bdf_elem_child(bson, "section"), dir, s);
					bson = bson->next;
				}
				break;
			}
		}

		std::vector<json::Addon> AddonsList::removeConflicted() {
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
	}
}
