#pragma once

#include <vector>
#include "Bitmap.hpp"

namespace wtwUpdate {
	namespace ui {
		class BitmapList : public std::vector<Bitmap> {
		public:
			inline void add(const wchar_t* id, const wchar_t* themeFn, int resId) {
				this->push_back(Bitmap(id, themeFn, resId));
			}

			inline void releaseAll() {
				size_t i, len = this->size();
				for (i = 0; i < len; i++)
					this->at(i).release();
			}
		};
	}
}
