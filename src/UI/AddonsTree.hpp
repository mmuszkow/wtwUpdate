#pragma once

#include <sstream>
#include <CommCtrl.h>
#include "Control.hpp"
#include "JsonObjs/Addon.hpp"
#include "JsonObjs/Section.hpp"
#include "cpp/Conv.h"

namespace wtwUpdate {
	namespace ui {

		class Bitmap {
			HBITMAP _hBmp;
			bool _wtwManaged;
		public:
			Bitmap() : _hBmp(NULL), _wtwManaged(false) { }

			Bitmap(const wchar_t* id, const wchar_t* themeFn, int resId) {
				wtwGraphics graph;
				graph.hInst = hInst;
				graph.graphId = id;
				graph.filePath = themeFn;
				graph.flags = WTW_GRAPH_FLAG_RELATIVE_DEF_PATH;
				if (fn->fnCall(WTW_GRAPH_LOAD, graph, 0) == TRUE) {
					graph.filePath = NULL;
					graph.flags = WTW_GRAPH_FLAG_GENERATE_HBITMAP;
					_hBmp = reinterpret_cast<HBITMAP>(fn->fnCall(WTW_GRAPH_GET_IMAGE, graph, NULL));
					_wtwManaged = true;
				} else {
					_hBmp = LoadBitmap(hInst, MAKEINTRESOURCE(resId));
					_wtwManaged = false;
				}
			}

			void release() {
				// if the HBITMAP was obtained from WTW, it will take care of deleting it
				if (_hBmp && !_wtwManaged)
					DeleteObject(_hBmp);
			}

			HBITMAP getBmp() const {
				return _hBmp;
			}
		};

		class BitmapList : public std::vector<Bitmap> {
		public:
			void add(const wchar_t* id, const wchar_t* themeFn, int resId) {
				this->push_back(Bitmap(id, themeFn, resId));
			}

			void releaseAll() {
				size_t i, len = this->size();
				for (i = 0; i < len; i++)
					this->at(i).release();
			}
		};

		class ImageList {
			HIMAGELIST _hImgList;

			ImageList() : _hImgList(NULL) {}
		public:
			ImageList(const BitmapList& bmps, int w, int h) {
				size_t i, len = bmps.size();
				_hImgList = ImageList_Create(w, h, ILC_COLOR32, len, 0);
				if (!_hImgList) {
					LOG_ERR(L"ImageList_Create failed");
					return;
				}

				for (i = 0; i < len; i++)
					ImageList_Add(_hImgList, bmps[i].getBmp(), NULL);
			}

			~ImageList() {
				if (_hImgList)
					ImageList_Destroy(_hImgList);
			}

			inline HIMAGELIST getHandle() const {
				return _hImgList;
			}
		};

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
			ImageList* _imgList;
			std::map<LPARAM, TreeItem*> _map;
			static LPARAM _uid;
			
			TreeItem* insert(TreeItem* newOne, TreeItem* parent, bool checked = false);
			void insertJsonSection(wtw::CJson* jsonSection, TreeItem* parent, const wtwUtils::Settings& s);
			void insertJsonAddon(wtw::CJson* jsonAddon, TreeItem* parent, const wtwUtils::Settings& s);
			void freeTree(TreeItem* item);
		public:
			// Tree updates the addon installation status
			AddonsTree(HWND handle, wtw::CJson* root);
			~AddonsTree();

			const std::string getDescription(LPARAM id);
			const std::vector<json::Addon> getSelected() const;
			const std::vector<json::Addon> getUnselected() const;
		};
	}
}
