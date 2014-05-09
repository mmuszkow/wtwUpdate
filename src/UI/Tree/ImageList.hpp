#pragma once

#include <Windows.h>
#include <CommCtrl.h>
#include "UI/BitmapList.hpp"

namespace wtwUpdate {
	namespace ui {
		namespace tree {
			class ImageList {
				HIMAGELIST _hImgList;

				ImageList();
			public:
				ImageList(const BitmapList& bmps, int w, int h);
				~ImageList();

				inline HIMAGELIST getHandle() const {
					return _hImgList;
				}
			};
		}
	}
}
