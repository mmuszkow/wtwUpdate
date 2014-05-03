#pragma once

#include "UniqueThread.h"
#include "../JsonObjs/Addon.h"
#include "TmpFile.h"
#include "ZipFile.h"

#include "cpp/Internet.h"

namespace wtwUpdate {
	namespace updater {
		class InstallThread : public UniqueThread {
			// it's a singleton so this is used for currently running
			std::vector<json::Addon> _toInstall;
			std::vector<json::Addon> _toRemove;

			// TODO: in WTW cache? download buffering?
			TmpFile download2cache(const wchar_t* url) {
				wtw::CInternetHttp http;
				wtw::CBuffer buff;
				if (FAILED(http.downloadFile(url, buff))) {
					LOG_ERR(fn, L"Failed to download %s", url);
					return TmpFile();
				}

				TmpFile f;
				if (!f.write(buff.getBuffer(), buff.getLength())) {
					f.remove();
					LOG_ERR(fn, L"Failed to write to %s", f.getPath().c_str());
					return TmpFile();
				}

				return f;
			}

			static DWORD WINAPI proc(LPVOID args) {
				InstallThread* thread = static_cast<InstallThread*>(args);
				thread->setRunning(true);

				size_t toInstallLen = thread->_toInstall.size();

				// TODO: check dependencies

				// TODO: check conflicts

				// TODO: remove

				// install
				for (size_t i = 0; i < toInstallLen; i++) {
					const json::Addon& addon = thread->_toInstall[i];

					if (addon.getInstallationState() == json::Addon::INSTALLED)
						continue;

					TmpFile f = thread->download2cache(utow(addon.getZipUrl()).c_str());

					if (f.isValid()) {
						ZipFile zip(f.getPath());
						if (!zip.isValid()) {
							std::wstring addonId = utow(addon.getId());
							LOG_ERR(thread->fn, L"Zip file for %s is invalid", addonId.c_str());
						} else if (!zip.unzip()) {
							std::wstring addonId = utow(addon.getId());
							LOG_ERR(thread->fn, L"Failed to install (unzip) %s", addonId.c_str());
						}
						f.remove();

						if (thread->isAborted()) {
							thread->setRunning(false);
							return 0;
						}
					}
				}

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
