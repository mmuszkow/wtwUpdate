#include "stdinc.h"
#include "Settings.hpp"

namespace wtwUtils {
	Settings::Settings() {
		wchar_t buff[MAX_PATH + 1];
		wtwMyConfigFile configName;
		configName.bufferSize = MAX_PATH;
		configName.pBuffer = buff;
		fn->fnCall(WTW_SETTINGS_GET_MY_CONFIG_FILE, configName, reinterpret_cast<WTW_PARAM>(hInst));

		if (FAILED(fn->fnCall(WTW_SETTINGS_INIT_EX,
			reinterpret_cast<WTW_PARAM>(configName.pBuffer),
			reinterpret_cast<WTW_PARAM>(&_config))))
			_config = NULL;
		else
			read();
	}

	Settings::~Settings() {
		if (_config) {
			fn->fnCall(WTW_SETTINGS_DESTROY,
				reinterpret_cast<WTW_PARAM>(_config),
				reinterpret_cast<WTW_PARAM>(hInst));
			_config = NULL;
		}
	}
}
