#pragma once

#include "stdinc.h"
#include "ImageList.hpp"

namespace wtwUpdate {
	namespace ui {
		namespace tree {
			ImageList::ImageList() : _hImgList(NULL) { }

			ImageList::ImageList(const BitmapList& bmps, int w, int h) {
				size_t i, len = bmps.size();
				_hImgList = ImageList_Create(w, h, ILC_COLOR32, len, 0);
				if (!_hImgList) {
					LOG_ERR(L"ImageList_Create failed");
					return;
				}

				for (i = 0; i < len; i++)
					ImageList_Add(_hImgList, bmps[i].getBmp(), NULL);
			}

			ImageList::~ImageList() {
				if (_hImgList)
					ImageList_Destroy(_hImgList);
			}
		}
	}
}
