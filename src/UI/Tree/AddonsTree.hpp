#pragma once

#include "UI/Control.hpp"
#include "TreeItem.hpp"
#include "ImageList.hpp"
#include "JsonObjs/addon.hpp"

namespace wtwUpdate {
	namespace ui {
		namespace tree {
			class AddonsTree : public Control {
				TreeItem* _root;
				ImageList* _imgList;
				std::map<LPARAM, TreeItem*> _map;
				static LPARAM _uid;

				TreeItem* insert(TreeItem* newOne, TreeItem* parent, json::Addon::InstallationState state);
				void insertJsonSection(wtw::CJson* jsonSection, TreeItem* parent, const wtwUtils::Settings& s);
				void insertJsonAddon(wtw::CJson* jsonAddon, TreeItem* parent, const wtwUtils::Settings& s);
				void freeTree(TreeItem* item);
			public:
				// Tree updates the addon installation status
				AddonsTree(HWND handle, wtw::CJson* root);
				~AddonsTree();

				void check() {
					TV_HITTESTINFO hitTest;
					DWORD dwpos = GetCursorPos(&hitTest.pt);
					MapWindowPoints(HWND_DESKTOP, getHwnd(), &hitTest.pt, 1);
					HTREEITEM hItem = TreeView_HitTest(getHwnd(), &hitTest);
					if (hItem != NULL && (hitTest.flags & TVHT_ONITEMICON)) {
						bool checked = (TreeView_GetCheckState(getHwnd(), hItem) == TRUE);

						TVITEM tvi;
						tvi.mask = TVIF_HANDLE | TVIF_STATE | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
						tvi.hItem = hItem;
						tvi.state = INDEXTOSTATEIMAGEMASK((!checked ? 2 : 1));
						tvi.stateMask = TVIS_STATEIMAGEMASK;
						tvi.iImage = tvi.iSelectedImage = !checked ? json::Addon::INSTALLED : json::Addon::NOT_INSTALLED;
						TreeView_SetItem(getHwnd(), &tvi);
					}
				}

				const std::string getDescription(LPARAM id);
				const std::vector<json::Addon> getSelected() const;
				const std::vector<json::Addon> getUnselected() const;
			};
		}
	}
}
