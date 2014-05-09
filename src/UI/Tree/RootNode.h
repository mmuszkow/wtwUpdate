#pragma once

#include "TreeItem.hpp"

namespace wtwUpdate {
	namespace ui {
		namespace tree {
			class RootNode : public TreeItem {
				std::string _title;
			public:
				RootNode() : TreeItem() {
					handle = TVI_ROOT;
					type = ROOT;
					_title = "root";
				}

				const std::string& getTitle() const {
					return _title;
				}

				const std::string& getDesc() const {
					return _title;
				}
			};
		}
	}
}
