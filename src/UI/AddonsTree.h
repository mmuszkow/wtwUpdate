#pragma once

#include <sstream>
#include <CommCtrl.h>

#include "Control.h"
#include "../JsonObjs/Addon.h"
#include "../JsonObjs/Section.h"

#include "cpp/Internet.h"
#include "cpp/Conv.h"

namespace wtwUpdate {
	namespace ui {

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

			virtual const std::string& getTitle() const = 0;
			virtual const std::string& getDesc() const = 0;
		};

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

		class AddonsTree : public Control {

			TreeItem* _root;
			std::map<LPARAM, TreeItem*> _map;
			static LPARAM _uid;

			void freeTree(TreeItem* item) {
				//if(!item) return;
				//freeTree(item->child);
				//freeTree(item->next);
				//delete item;
			}

			TreeItem* insert(TreeItem* newOne, TreeItem* parent, bool checked = false) {
				TVINSERTSTRUCT tvis;
				memset(&tvis, 0, sizeof(TVINSERTSTRUCT));
				tvis.hParent = parent->handle;
				tvis.hInsertAfter = TVI_LAST;
				tvis.item.mask = TVIF_TEXT | TVIF_PARAM;
				if (checked) {
					// TODO: not working
					tvis.item.mask |= TVIF_STATE;
					tvis.item.stateMask = TVIS_STATEIMAGEMASK;
					tvis.item.state = INDEXTOSTATEIMAGEMASK(2);					
				}
				tvis.item.lParam = _uid;
				wchar_t* wTitle = wtw::CConv::utow(newOne->getTitle().c_str());
				tvis.item.pszText = wTitle;
				tvis.item.cchTextMax = wcslen(wTitle);
				HTREEITEM ret = TreeView_InsertItem(getHwnd(), &tvis);
				wtw::CConv::release(wTitle);

				if (ret != NULL) {
					newOne->handle = ret;
					newOne->id = _uid++;
					_map[newOne->id] = newOne;
					if (!parent->child)
						parent->child = newOne;
					else {
						TreeItem* last = parent->child;
						while (last->next) {
							last = last->next;
						}
						last->next = newOne;
					}
					return newOne;
				}

				return NULL;
			}

			void insertJsonSection(wtw::CJson* jsonSection, TreeItem* parent) {
				if (!jsonSection)
					return;

				if (jsonSection->isArray()) {
					size_t len = jsonSection->size();
					for (size_t i = 0; i < len; i++)
						insertJsonSection(jsonSection->getAt(i)->find("section"), parent);
				} else {
					json::Section section(jsonSection);
					TreeItem* newRoot = insert(new SectionNode(section), parent);
					if (newRoot) {
						insertJsonSection(jsonSection->find("section"), newRoot);
						insertJsonAddon(jsonSection->find("addon"), newRoot);
					}
				}
			}

			void insertJsonAddon(wtw::CJson* jsonAddon, TreeItem* parent) {
				if (!jsonAddon)
					return;

				if (jsonAddon->isArray()) {
					size_t len = jsonAddon->size();
					for (size_t i = 0; i < len; i++)
						insertJsonAddon(jsonAddon->getAt(i), parent);
				}				else {
					if (parent->type == TreeItem::NODE) {
						SectionNode* sectionNode = static_cast<SectionNode*>(parent);
						json::Addon addon(jsonAddon, sectionNode->getSection().getDir().c_str());
						// TODO: checkbox for installed, some other checkbox for those needing update
						insert(new AddonLeaf(addon), parent, addon.getInstallationState() == json::Addon::INSTALLED);
					} else {
						// TODO: log, addons can be only the children of the sections (excluding root)
					}
				}
			}
		public:
			AddonsTree(HWND handle, wtw::CJson* root) : Control(handle) {
				_root = new RootNode();
				if (!root)
					return;

				clear();
				insertJsonSection(root, _root);
			}

			~AddonsTree() {
				freeTree(_root);
			}

			inline void clear() {
				TreeView_DeleteAllItems(getHwnd());
				_map.clear();
				freeTree(_root);
			}

			const std::string getDescription(LPARAM id) {
				if (_map.find(id) == _map.end())
					return "Error: report this";

				return _map[id]->getDesc();
			}

			const std::vector<json::Addon> getSelected() const {
				std::vector<json::Addon> ret;
				std::map<LPARAM, TreeItem*>::const_iterator it = _map.begin(), end = _map.end();
				while (it != end) {
					if (it->second->type == TreeItem::LEAF) {
						bool checked = (TreeView_GetCheckState(getHwnd(), it->second->handle) == 1);
						if (checked) {
							AddonLeaf* leaf = reinterpret_cast<AddonLeaf*>(it->second);
							ret.push_back(leaf->getAddon());
						}
					}
					++it;
				}
				return ret;
			}
		};
	}
}
