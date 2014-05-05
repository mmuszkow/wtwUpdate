#pragma once

#include "../stdinc.h"

namespace wtwUtils {

	class Settings {
		void*			_config;
	public:
		Settings() {
			wchar_t buff[MAX_PATH + 1];
			wtwMyConfigFile configName;
			configName.bufferSize = MAX_PATH;
			configName.pBuffer = buff;
			fn->fnCall(WTW_SETTINGS_GET_MY_CONFIG_FILE, configName, reinterpret_cast<WTW_PARAM>(hInst));

			if (fn->fnCall(WTW_SETTINGS_INIT_EX,
				reinterpret_cast<WTW_PARAM>(configName.pBuffer),
				reinterpret_cast<WTW_PARAM>(&_config)) != S_OK)
				_config = NULL;
			else
				read();
		}

		~Settings() {
			if (_config) {
				fn->fnCall(WTW_SETTINGS_DESTROY, 
					reinterpret_cast<WTW_PARAM>(_config), 
					reinterpret_cast<WTW_PARAM>(hInst));
				_config = NULL;
			}
		}

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
