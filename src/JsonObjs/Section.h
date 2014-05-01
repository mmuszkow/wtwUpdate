#pragma once

#include "Obj.h"

namespace wtwUpdate {
	namespace json {
		class Section : public Obj {
			std::string _title;
			std::string _desc;
			std::string _dir;
		public:
			Section() {	}

			Section(wtw::CJson* json) : Obj(json) {
				_title = getStr("title");
				_desc = getStr("desc");
				_dir = getStr("dir");
			}

			inline const std::string& getTitle() const {
				return _title;
			}

			inline const std::string& getDesc() const {
				return _desc;
			}

			inline const std::string& getDir() const {
				return _dir;
			}
		};
	}
}
