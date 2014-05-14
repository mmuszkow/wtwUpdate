#include "stdinc.h"
#include "AddonsTree.hpp"
#include "RootNode.h"
#include "SectionNode.hpp"
#include "AddonLeaf.hpp"

namespace wtwUpdate {
	namespace ui {
		namespace tree {
			LPARAM AddonsTree::_uid = 10;

			// Tree updates the addon installation status
			AddonsTree::AddonsTree(HWND handle, wtw::CJson* root) : Control(handle) {
				initImages();
				_root = new RootNode();
				wtwUtils::Settings s;
				insertJsonSection(root, _root, s);
			}

			// Tree updates the addon installation status
			AddonsTree::AddonsTree(HWND handle, bds_node* root) : Control(handle) {
				initImages();
				_root = new RootNode();
				wtwUtils::Settings s;
				insertBsonSection(root, _root, s);
			}

			AddonsTree::~AddonsTree() {
				freeTree(_root);
				delete _imgList;
			}

			void AddonsTree::initImages() {
				BitmapList images;
				images.add(L"wtwUpdate/unchecked", L"wtwUpdateUnchecked.png", IDB_UNCHECKED);
				images.add(L"wtwUpdate/checked", L"wtwUpdateChecked.png", IDB_CHECKED);
				images.add(L"wtwUpdate/modified", L"wtwUpdateModified.png", IDB_MODIFIED);
				_imgList = new ImageList(images, 13, 13);
				TreeView_SetImageList(getHwnd(), _imgList->getHandle(), TVSIL_NORMAL);
				images.releaseAll();
			}

			void AddonsTree::freeTree(TreeItem* item) {
				if (!item) return;
				freeTree(item->child);
				freeTree(item->next);
				delete item;
			}

			TreeItem* AddonsTree::insert(TreeItem* newOne, TreeItem* parent, json::Addon::InstallationState state) {
				TVINSERTSTRUCT tvis;
				memset(&tvis, 0, sizeof(TVINSERTSTRUCT));
				tvis.hParent = parent->handle;
				tvis.hInsertAfter = TVI_LAST;
				tvis.item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
				if (state == json::Addon::MODIFIED || state == json::Addon::INSTALLED) {
					tvis.item.mask |= TVIF_STATE;
					tvis.item.stateMask = TVIS_STATEIMAGEMASK;
					tvis.item.state = INDEXTOSTATEIMAGEMASK(2);
				}
				tvis.item.iImage = tvis.item.iSelectedImage = state;
				tvis.item.lParam = _uid;
				std::wstring wTitle = stow(newOne->getTitle());
				tvis.item.pszText = const_cast<wchar_t*>(wTitle.c_str());
				tvis.item.cchTextMax = wTitle.size();
				HTREEITEM ret = TreeView_InsertItem(getHwnd(), &tvis);

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

			void AddonsTree::insertJsonSection(wtw::CJson* jsonSection, TreeItem* parent, const wtwUtils::Settings& s) {
				if (!jsonSection)
					return;

				if (jsonSection->isArray()) {
					size_t len = jsonSection->size();
					for (size_t i = 0; i < len; i++)
						insertJsonSection(jsonSection->getAt(i)->find("section"), parent, s);
				}
				else {
					json::Section section(jsonSection);
					TreeItem* newRoot = insert(new SectionNode(section), parent, json::Addon::NOT_INSTALLED);
					if (newRoot) {
						insertJsonSection(jsonSection->find("section"), newRoot, s);
						insertJsonAddon(jsonSection->find("addon"), newRoot, s);
					}
				}
			}

			void AddonsTree::insertJsonAddon(wtw::CJson* jsonAddon, TreeItem* parent, const wtwUtils::Settings& s) {
				if (!jsonAddon)
					return;

				if (jsonAddon->isArray()) {
					size_t len = jsonAddon->size();
					for (size_t i = 0; i < len; i++)
						insertJsonAddon(jsonAddon->getAt(i), parent, s);
				}
				else {
					if (parent->type == TreeItem::NODE) {
						SectionNode* sectionNode = static_cast<SectionNode*>(parent);
						json::Addon addon(jsonAddon, sectionNode->getSection().getDir().c_str());
						addon.updateInstallationState(s);
						insert(new AddonLeaf(addon), parent, addon.getState());
					}
					else {
						// TODO: log, addons can be only the children of the sections (excluding root)
					}
				}
			}

			void AddonsTree::insertBsonSection(bds_node* bsonSection, TreeItem* parent, const wtwUtils::Settings& s) {
				switch (bdf_elem_type(bsonSection)) {
				case E_DOC: {
					json::Section section(bsonSection);
					TreeItem* newRoot = insert(new SectionNode(section), parent, json::Addon::NOT_INSTALLED);
					if (newRoot) {
						insertBsonSection(bdf_elem_child(bsonSection, "section"), newRoot, s);
						insertBsonAddon(bdf_elem_child(bsonSection, "addon"), newRoot, s);
					}
					break;
				}
				case E_ARRAY:
					bsonSection = bsonSection->elem.data.node;
					while (bsonSection) {
						insertBsonSection(bdf_elem_child(bsonSection, "section"), parent, s);
						bsonSection = bsonSection->next;
					}
					break;
				}
			}

			void AddonsTree::insertBsonAddon(bds_node* bsonAddon, TreeItem* parent, const wtwUtils::Settings& s) {
				switch (bdf_elem_type(bsonAddon)) {
				case E_DOC: {
					SectionNode* sectionNode = static_cast<SectionNode*>(parent);
					json::Addon addon(bsonAddon, sectionNode->getSection().getDir().c_str());
					addon.updateInstallationState(s);
					insert(new AddonLeaf(addon), parent, addon.getState());
					break;
				}
				case E_ARRAY:
					bsonAddon = bsonAddon->elem.data.node;
					while (bsonAddon) {
						insertBsonAddon(bsonAddon, parent, s);
						bsonAddon = bsonAddon->next;
					}
					break;
				}
			}

			const std::string AddonsTree::getDescription(LPARAM id) {
				if (_map.find(id) == _map.end())
					return "Error: report this";

				return _map[id]->getDesc();
			}

			const std::vector<json::Addon> AddonsTree::getSelected() const {
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

			const std::vector<json::Addon> AddonsTree::getUnselected() const {
				std::vector<json::Addon> ret;
				std::map<LPARAM, TreeItem*>::const_iterator it = _map.begin(), end = _map.end();
				while (it != end) {
					if (it->second->type == TreeItem::LEAF) {
						bool checked = (TreeView_GetCheckState(getHwnd(), it->second->handle) == 1);
						AddonLeaf* leaf = reinterpret_cast<AddonLeaf*>(it->second);
						if (!checked && leaf->getAddon().getState() != json::Addon::NOT_INSTALLED)
							ret.push_back(leaf->getAddon());
					}
					++it;
				}
				return ret;
			}
		}
	}
}
