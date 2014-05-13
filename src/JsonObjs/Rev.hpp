#pragma once

#include "Obj.hpp"

namespace wtwUpdate {
	namespace json {
		class Rev : public Obj {
			__int64 _time;
			std::string _msg;

			void init() {
				_time = getInt("time");
				_msg = getStr("msg");
			}
		public:
			Rev() {
				_time = 0;
			}

			Rev(wtw::CJson* json) : Obj(json) {
				init();
			}

			Rev(bds_node* bson) : Obj(bson) {
				init();
			}

			inline __int64 getTime() const {
				return _time;
			}

			inline const std::string& getMsg() const {
				return _msg;
			}
		};
	}
}
