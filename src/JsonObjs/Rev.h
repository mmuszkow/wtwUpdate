#pragma once

#include "Obj.h"

namespace wtwUpdate {
	namespace json {
		class Rev : public Obj {
			__int64 _time;
			std::string _msg;
		public:
			Rev() {
				_time = 0;
			}

			Rev(wtw::CJson* json) : Obj(json) {
				_time = getInt("time");
				_msg = getStr("msg");
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
