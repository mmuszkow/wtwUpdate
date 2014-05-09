#pragma once

#include "stdinc.h"
#include "InstallThread.hpp"
#include "Utils/BinaryFile.hpp"
#include "Utils/Settings.hpp"
#include "Utils/Http.hpp"
#include "AddonDeleter.hpp"
#include "ZipFile.hpp"

namespace wtwUpdate {
	namespace updater {
		DWORD InstallThread::proc(LPVOID args) {
			InstallThread* thread = static_cast<InstallThread*>(args);
			thread->setRunning(true);

			// TODO: check dependencies

			// check conflicts
			int failed = 0;
			std::vector<json::Addon> conflicts1 = thread->_toRemove.removeConflicted(); // this will probably never happen
			size_t i, len = conflicts1.size();
			failed += len;
			for (i = 0; i < len; i++)
				LOG_ERR(L"Addon %s conflicts with other addon", utow(conflicts1[i].getId()).c_str());

			std::vector<json::Addon> conflicts2 = thread->_toInstall.removeConflicted();
			i, len = conflicts2.size();
			failed += len;
			for (i = 0; i < len; i++)
				LOG_ERR(L"Addon %s conflicts with other addon", utow(conflicts2[i].getId()).c_str());

			// remove
			wtwUtils::Settings s;
			i, len = thread->_toRemove.size();
			int removed = 0;
			for (i = 0; i < len; i++) {
				const json::Addon& addon = thread->_toRemove[i];

				if (addon.getState() == json::Addon::INSTALLED) {
					AddonDeleter deleter(addon);
					deleter.del(s) ? removed++ : failed++;
				}

				if (thread->isAborted()) {
					thread->setRunning(false);
					return 0;
				}
			}

			// install
			len = thread->_toInstall.size();
			int installed = 0, updated = 0;
			for (i = 0; i < len; i++) {
				const json::Addon& addon = thread->_toInstall[i];

				if (addon.getState() == json::Addon::INSTALLED)
					continue;

				utils::BinaryFile f;
				utils::Http http;
				if (http.download2file(utow(addon.getZipUrl()).c_str(), &f)) {
					ZipFile zip(f.getPath());
					if (!zip.isValid()) {
						std::wstring addonId = utow(addon.getId());
						LOG_ERR(L"Zip file for %s is invalid", addonId.c_str());
						failed++;
					}
					else if (!zip.unzip()) {
						std::wstring addonId = utow(addon.getId());
						LOG_ERR(L"Failed to install (unzip) %s", addonId.c_str());
						failed++;
					}
					else {
						s.setInt64(utow(addon.getId()).c_str(), addon.getTime());
						s.write();
						switch (addon.getState()) {
						case json::Addon::NOT_INSTALLED:
							installed++;
							break;
						case json::Addon::MODIFIED:
							updated++;
							break;
						}
					}
				}
				else
					LOG_ERR(L"Failed to download %s", addon.getZipUrl().c_str());

				f.del();

				if (thread->isAborted()) {
					thread->setRunning(false);
					return 0;
				}
			}

			notify(L"Zainstalowano %u, zaktualizowano %u, usunięto %u, błędów %u", installed, updated, removed, failed);
			thread->_toInstall.clear();
			thread->_toRemove.clear();
			thread->setRunning(false);
			return 0;
		}

		void InstallThread::setArg(const std::vector<json::Addon>& toInstall, const std::vector<json::Addon>& toRemove) {
			if (isRunning())
				return;

			_toInstall = toInstall;
			_toRemove = toRemove;
		}
	}
}
