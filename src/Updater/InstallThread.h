#pragma once

#include "UniqueThread.h"
#include "../JsonObjs/Addon.h"
#include "ZipFile.h"

#include "cpp/Internet.h"

namespace wtwUpdate {
	namespace updater {
		class InstallThread : public UniqueThread {

			static std::vector<json::Addon> toInstall;

			class TmpFile {
				std::wstring _path;
			public:
				TmpFile() {
					wchar_t path[MAX_PATH + 1], fn[MAX_PATH + 1];
					if (!GetTempPath(MAX_PATH, path))
						return;

					if (GetTempFileName(path, L"wtwUpdate-", 0, fn))
						_path = fn;
				}

				const std::wstring& getPath() const {
					return _path;
				}

				bool isValid() const {
					return _path.size() > 0;
				}

				bool write(const void* buff, size_t len) {
					FILE* f = _wfopen(_path.c_str(), L"wb");
					if (!f)
						return false;

					if (fwrite(buff, 1, len, f) != len) {
						fclose(f);
						return false;
					}

					fclose(f);
					return true;
				}

				void remove() {
					if (_path.size() > 0) {
						DeleteFile(_path.c_str());
						_path = L"";
					}
				}
			};

			// TODO: in WTW cache? download buffering?
			static TmpFile download2cache(const wchar_t* url) {
				wtw::CInternetHttp http;
				wtw::CBuffer buff;
				if (FAILED(http.downloadFile(url, buff)))
					return TmpFile();

				TmpFile f;
				if (!f.write(buff.getBuffer(), buff.getLength())) {
					f.remove();
					return TmpFile();
				}

				return f;
			}

			static DWORD WINAPI proc(LPVOID args) {
				size_t len = toInstall.size();
				for (size_t i = 0; i < len; i++) {
					const json::Addon& addon = toInstall[i];

					if (addon.getInstallationState() == json::Addon::INSTALLED)
						continue;

					TmpFile f = download2cache(utow(addon.getZipUrl()).c_str());

					if (f.isValid()) {
						ZipFile zip(f.getPath());
						if (!zip.isValid() || !zip.unzip()) {
							// TODO: log
						}
						f.remove();
					}
				}

				return 0;
			}
		public:
			InstallThread() : UniqueThread(proc) {
			}
		};
	}
}