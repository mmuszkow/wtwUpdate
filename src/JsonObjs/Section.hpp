#pragma once

#include "Obj.hpp"

namespace wtwUpdate {
	namespace json {
		class Section : public Obj {
			std::string _title;
			std::string _desc;
			std::string _dir;

			void init() {
				_title = getStr("title");
				_desc = getStr("desc");
				_dir = getStr("dir");
			}
		public:
			Section() {	}

			Section(bds_node* bson) : Obj(bson) {
				init();
			}

			Section(wtw::CJson* json) : Obj(json) {
				init();
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
