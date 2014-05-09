#pragma once

#include <plInterface.h>

namespace wtwUtils {

	class Settings {
		void*			_config;
	public:
		Settings();
		~Settings();

		inline void Settings::setInt64(const wchar_t* name, __int64 const val) {
			if (_config) wtwSetInt64(fn, _config, name, val);
		}

		inline __int64 Settings::getInt64(const wchar_t* name, __int64 defVal) const {
			if(_config) return wtwGetInt64(fn, _config, name, defVal);
			return defVal;
		}

		inline void Settings::read() {
			if(_config)	fn->fnCall(WTW_SETTINGS_READ, reinterpret_cast<WTW_PARAM>(_config), 0);
		}

		inline void Settings::write() {
			if(_config) fn->fnCall(WTW_SETTINGS_WRITE, reinterpret_cast<WTW_PARAM>(_config), 0);
		}
	};
};
