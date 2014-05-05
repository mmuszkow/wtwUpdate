#pragma once

#include "UniqueThread.h"
#include "../JsonObjs/Addon.h"
#include "../Utils/BinaryFile.h"
#include "../Utils/Settings.h"
#include "ZipFile.h"

#include "cpp/Internet.h"

namespace wtwUpdate {
	namespace updater {
		class InstallThread : public UniqueThread {
			// it's a singleton so this is used for currently running
			std::vector<json::Addon> _toInstall;
			std::vector<json::Addon> _toRemove;

			// TODO: in WTW cache? download buffering?
			bool download2cache(utils::BinaryFile& f, const wchar_t* url) {
				wtw::CInternetHttp http;
				wtw::CBuffer buff;

				if (!f.openTmp()) {
					LOG_ERR(L"Failed to create temporary file for downloaded file");
					return false;
				}

				if (FAILED(http.downloadFile(url, buff))) {
					f.del();
					LOG_ERR(L"Failed to download %s", url);
					return false;
				}
				
				if (!f.write(buff.getBuffer(), buff.getLength())) {
					f.del();
					LOG_ERR(L"Failed to write to %s", f.getPath().c_str());
					return false;
				}

				f.close();
				return true;
			}

			static DWORD WINAPI proc(LPVOID args) {
				InstallThread* thread = static_cast<InstallThread*>(args);
				thread->setRunning(true);

				size_t toInstallLen = thread->_toInstall.size();

				// TODO: check dependencies

				// TODO: check conflicts

				// TODO: remove

				// install
				wtwUtils::Settings s;
				int installed = 0, updated = 0, failed = 0;
				for (size_t i = 0; i < toInstallLen; i++) {
					const json::Addon& addon = thread->_toInstall[i];

					if (addon.getState() == json::Addon::INSTALLED)
						continue;

					utils::BinaryFile f;
					if (thread->download2cache(f, utow(addon.getZipUrl()).c_str())) {
						ZipFile zip(f.getPath());
						if (!zip.isValid()) {
							std::wstring addonId = utow(addon.getId());
							LOG_ERR(L"Zip file for %s is invalid", addonId.c_str());
							failed++;
						} else if (!zip.unzip()) {
							std::wstring addonId = utow(addon.getId());
							LOG_ERR(L"Failed to install (unzip) %s", addonId.c_str());
							failed++;
						} else {
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
						f.del();
					}

					if (thread->isAborted()) {
						thread->setRunning(false);
						return 0;
					}
				}

				notify(L"Zainstalowano %u, zaktualizowano %u, błędów %u", installed, updated, failed);
				thread->_toInstall.clear();
				thread->setRunning(false);
				return 0;
			}
		public:
			static InstallThread& get() {
				static InstallThread instance;
				return instance;
			}
			
			bool start() {
				return UniqueThread::start(proc);
			}

			void setArg(const std::vector<json::Addon>& toInstall, const std::vector<json::Addon>& toRemove) {
				if (isRunning())
					return;

				_toInstall = toInstall;
				_toRemove = toRemove;
			}
		};
	}
}
