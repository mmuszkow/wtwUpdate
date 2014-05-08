#pragma once

#include <Windows.h>
#include <WinInet.h>
#include "BinaryFile.h"

namespace wtwUpdate {
	namespace utils {
		/// HTTP or HTTPS connection.
		class Http {
		public:
			// TODO: reserve
			class Buffer {
				BYTE* _data;
				size_t _len;

				void dealloc() {
					if (_data)
						delete[] _data;
					_data = NULL;
					_len = 0;
				}

				void assign(const Buffer& b) {
					dealloc();
					_len = b._len;
					if (_len) {
						_data = new BYTE[_len];
						memcpy_s(_data, _len, b._data, _len);
					} else
						_data = NULL;
				}
			public:
				Buffer() : _data(NULL), _len(0) { }
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
					// TODO
				}
			};

			HINTERNET _hInternet;
			HINTERNET _hConnect;
		public:
			Http() {
				_hInternet = InternetOpen(L"wtwUpdate", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
			}

			~Http() {
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

			bool download2buff(Buffer& ret, const std::wstring& path) {
				if (!_hConnect)
					return false;

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

				// TODO

				return true;
			}

			bool download2file(BinaryFile& ret, const std::wstring& path) {
				if (!_hConnect)
					return false;

				if(!ret.isOpen() && !ret.openTmp())
					return false;

				// TODO

				ret.close();
				return true;
			}
		};
	}
}
