#pragma once

#include "TreeItem.hpp"
#include "JsonObjs/addon.hpp"

namespace wtwUpdate {
	namespace ui {
		namespace tree {
			class AddonLeaf : public TreeItem {
				json::Addon _addon;
				std::string _fullDesc;
			public:
				AddonLeaf(json::Addon addon) : TreeItem(), _addon(addon) {
					type = LEAF;
					std::stringstream ss;
					ss << "<b>" << addon.getName() << "</b><br>"
						<< "ver " << addon.getVer() << "<br>"
						<< "by " << addon.getAuthor() << "<br><br>"
						<< addon.getDesc();
					_fullDesc = ss.str();
				}

				const std::string& getTitle() const {
					return _addon.getName();
				}

				const std::string& getDesc() const {
					return _fullDesc;
				}

				const json::Addon& getAddon() const {
					return _addon;
				}
			};
		}
	}
}
