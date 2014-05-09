#pragma once

#include "Obj.hpp"

namespace wtwUpdate {
	namespace json {
		class File : public Obj {
			std::string _path;
			__int64 _size;
			__int64 _time;
			//__int32 _crc32;
		public:
			File() {
				_size = _time = 0;
			}

			File(wtw::CJson* json) : Obj(json) {
				_path = getStr("path");
				_size = getInt("size");
				_time = getInt("time");
			}

			inline const std::string& getPath() const {
				return _path;
			}

			inline __int64 getTime() const {
				return _time;
			}

			inline __int64 getSize() const {
				return _size;
			}
		};
	}
}
