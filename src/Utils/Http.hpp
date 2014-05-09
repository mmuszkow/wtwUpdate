#pragma once

#include <algorithm>
#include <Windows.h>
#include <WinInet.h>
#include "BinaryFile.hpp"

namespace wtwUpdate {
	namespace utils {
		/// HTTP or HTTPS connection.
		class Http {
		public:
			class Buffer {
				BYTE* _data;
				size_t _len;
				size_t _reserved;

				void dealloc() {
					if (_data)
						delete[] _data;
					_data = NULL;
					_reserved = 0;
					_len = 0;
				}

				void assign(const Buffer& b) {
					dealloc();
					_len = b._len;
					_reserved = b._reserved;
					if (_reserved) {
						_data = new BYTE[_reserved];
						memcpy_s(_data, _reserved, b._data, _len);
					} else
						_data = NULL;
				}
			public:
				Buffer() : _data(NULL), _len(0), _reserved(0) { }

				Buffer(int reserved) : _len(0), _reserved(reserved) {
					_data = new BYTE[_reserved];
				}

				Buffer(const Buffer& other) : _data(NULL), _len(0) {
					assign(other);
				}

				~Buffer() {
					dealloc();
				}

				inline Buffer& operator = (const Buffer& b) {
					assign(b);
					return *this;
				}

				void append(const BYTE* data, size_t len) {
					if (_len + len > _reserved) {
						_reserved = max(_len + len, _reserved << 1);
						BYTE* newData = new BYTE[_reserved];
						memcpy_s(newData, _reserved, _data, _len);
						delete[]_data;
						_data = newData;
					}
					memcpy_s(&_data[_len], _reserved - _len, data, len);
					_len += len;
				}

				BYTE* getBuffer() const {
					return _data;
				}
			};

			static bool download2buffHandler(BYTE* buff, size_t len, void* data) {
				Buffer* total = static_cast<Buffer*>(data);
				total->append(buff, len);
				return true;
			}

			static bool download2fileHandler(BYTE* buff, size_t len, void* data) {
				BinaryFile* f = static_cast<BinaryFile*>(data);
				return f->write(buff, len);
			}

			HINTERNET _hInternet;
			HINTERNET _hConnect;
		public:
			Http() {
				_hInternet = InternetOpen(L"wtwUpdate", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
			}

			~Http() {
				disconnect();
				if (_hInternet)
					InternetCloseHandle(_hInternet);
			}

			bool connect(const std::wstring& server, int port) {
				if (_hConnect)
					disconnect();
				_hConnect = InternetConnect(_hInternet,	server.c_str(), port, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
				return _hConnect != NULL;
			}

			void disconnect() {
				if (_hConnect)
					InternetCloseHandle(_hConnect);
			}

			// TODO
			size_t getFileSize() {
				return -1;
			}

			// return false to abort download
			typedef bool(*pDownloadHandler)(BYTE* buff, size_t len, void* data);

			bool download(const std::wstring& path, pDownloadHandler handler, void* handlerData) {
				if (!_hConnect) {
					// TODO: connect
					return false;
				}

				DWORD dwOpenRequestFlags = INTERNET_FLAG_IGNORE_REDIRECT_TO_HTTP |
					INTERNET_FLAG_IGNORE_REDIRECT_TO_HTTPS |
					INTERNET_FLAG_KEEP_CONNECTION |
					INTERNET_FLAG_NO_AUTH |
					INTERNET_FLAG_NO_AUTO_REDIRECT |
					INTERNET_FLAG_NO_COOKIES |
					INTERNET_FLAG_NO_UI |
					INTERNET_FLAG_RELOAD;
				HINTERNET hRequest = HttpOpenRequest(_hConnect, L"GET", path.c_str(), NULL, NULL, NULL, dwOpenRequestFlags, 0);
				if (!hRequest)
					return false;

				if (HttpSendRequest(hRequest, NULL, 0, NULL, 0) == FALSE) {
					InternetCloseHandle(hRequest);
					return false;
				}

				BYTE buff[16384];
				DWORD bytesRead;
				while (1) {
					if (InternetReadFile(hRequest, buff, 16384, &bytesRead) == FALSE) {
						InternetCloseHandle(hRequest);
						return false;
					}

					if (bytesRead == 0)
						break;

					if (!handler(buff, bytesRead, handlerData)) {
						InternetCloseHandle(hRequest);
						return false;
					}
				}

				InternetCloseHandle(hRequest);
				return true;
			}

			bool download2file(const std::wstring& path, BinaryFile* f) {
				if(!f->isOpen() && !f->openTmp())
					return false;

				bool ret = download(path, download2fileHandler, f);
				f->close();
				return ret;
			}

			inline bool download2buff(const std::wstring& path, Buffer* buff) {				
				return download(path, download2buffHandler, buff);
			}
		};
	}
}
