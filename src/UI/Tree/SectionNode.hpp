#pragma once

#include "TreeItem.hpp"
#include "JsonObjs/section.hpp"

namespace wtwUpdate {
	namespace ui {
		namespace tree {
			class SectionNode : public TreeItem {
				json::Section _section;
			public:
				SectionNode(json::Section section) : TreeItem(), _section(section) {
					type = NODE;
				}

				const std::string& getTitle() const {
					return _section.getTitle();
				}

				const std::string& getDesc() const {
					return _section.getDesc();
				}

				const json::Section& getSection() const {
					return _section;
				}
			};
		}
	}
}
