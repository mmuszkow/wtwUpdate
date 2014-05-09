#pragma once

#include "stdinc.h"
#include <CommCtrl.h>

namespace wtwUpdate {
	namespace ui {
		namespace tree {
			class TreeItem {
			public:
				enum Type { UNK, ROOT, NODE, LEAF };
				Type type;
				HTREEITEM handle;
				LPARAM    id;
				TreeItem* child;
				TreeItem* next;

				TreeItem() {
					type = UNK;
					handle = NULL;
					id = 0;
					child = next = NULL;
				}

				virtual ~TreeItem() { }

				virtual const std::string& getTitle() const = 0;
				virtual const std::string& getDesc() const = 0;
			};
		}
	}
}
