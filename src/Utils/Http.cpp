#include "stdinc.h"
#include "Http.hpp"
#include <algorithm>

namespace wtwUpdate {
	namespace utils {
		////////////////// HttpUrl //////////////////

		Http::Url::Url(const std::wstring& url) {
			size_t			pos;
			std::wstring	str(url);

			_ssl = false;
			_port = 80;

			// skip http:// or https://
			if ((pos = str.find(L"://")) != std::wstring::npos) {
				std::wstring schema = str.substr(0, pos);
				std::transform(schema.begin(), schema.end(), schema.begin(), ::tolower);
				if (schema == L"https") {
					_ssl = true;
					_port = 443;
				}
				str = str.substr(pos + 3);
			}

			// remove anything after host name
			if ((pos = str.find_first_of('/')) != std::wstring::npos) {
				_object = str.substr(pos);
				str = str.substr(0, str.size() - _object.size());
			}
			else
				_object = L"/";

			// No port info, set to default
			if ((pos = str.find_last_of(L':')) == std::wstring::npos) {
				_host = str;
				return;
			}

			// Avoid seg fault if user puts "host:"
			if (pos + 1 >= str.size()) {
				_host = str.substr(0, pos);
				return;
			}

			// Finally cut the host part and convert the port part of the string
			_host = str.substr(0, pos);
			_port = _wtoi(&str.c_str()[pos + 1]);
		}

		////////////////// Buffer //////////////////

		Http::Buffer::Buffer() : _data(NULL), _len(0), _reserved(0) { }

		Http::Buffer::Buffer(int reserved) : _len(0) {
			reserve(reserved);
		}

		Http::Buffer::Buffer(const Http::Buffer& other) : _data(NULL), _len(0) {
			assign(other);
		}

		Http::Buffer::~Buffer() {
			dealloc();
		}

		void Http::Buffer::dealloc() {
			if (_data)
				delete[] _data;
			_data = NULL;
			_reserved = 0;
			_len = 0;
		}

		void Http::Buffer::assign(const Buffer& b) {
			dealloc();
			_len = b._len;
			_reserved = b._reserved;
			if (_reserved) {
				_data = new BYTE[_reserved];
				memcpy_s(_data, _reserved, b._data, _len);
			}
			else
				_data = NULL;
		}

		void Http::Buffer::reserve(size_t reserved) {
			if (reserved < _reserved)
				return;

			BYTE* newData = new BYTE[reserved];
			if (_data) {
				memcpy_s(newData, reserved, _data, _len);
				delete[]_data;
			}
			_data = newData;
			_reserved = reserved;
		}

		void Http::Buffer::append(const BYTE* data, size_t len) {
			if (_len + len > _reserved)
				reserve(max(_len + len, _reserved << 1));
			memcpy_s(&_data[_len], _reserved - _len, data, len);
			_len += len;
		}

		////////////////// Http //////////////////

		Http::Http() : _hConnect(NULL) {
			_hInternet = InternetOpen(L"wtwUpdate", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
		}

		Http::~Http() {
			disconnect();
			if (_hInternet)
				InternetCloseHandle(_hInternet);
		}

		bool Http::download2buffHandler(BYTE* buff, size_t len, size_t total, void* data) {
			Buffer* b = static_cast<Buffer*>(data);
			if (total != -1)
				b->reserve(total);
			b->append(buff, len);
			return true;
		}

		bool Http::download2fileHandler(BYTE* buff, size_t len, size_t total, void* data) {
			BinaryFile* f = static_cast<BinaryFile*>(data);
			return f->write(buff, len);
		}

		bool Http::connect(const std::wstring& server, int port) {
			if (_hConnect)
				disconnect();
			_hConnect = InternetConnect(_hInternet, server.c_str(), port, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
			return _hConnect != NULL;
		}

		void Http::disconnect() {
			if (_hConnect)
				InternetCloseHandle(_hConnect);
		}

		bool Http::download(const std::wstring& urlStr, pDownloadHandler handler, void* handlerData) {
			Url url(urlStr);

			if (!_hConnect && !connect(url.getHost(), url.getPort()))
				return false;

			DWORD flags = INTERNET_FLAG_DONT_CACHE;
			if (url.isSsl())
				flags |= INTERNET_FLAG_SECURE;

			HINTERNET hRequest = HttpOpenRequest(
				_hConnect, L"GET", url.getObj().c_str(), HTTP_VERSION, NULL, NULL, flags, 0);
			if (!hRequest)
				return false;

			wchar_t queryInfo[64];
			DWORD queryInfoLen = 64;
			size_t total = -1;
			if (HttpQueryInfo(hRequest, HTTP_QUERY_CONTENT_LENGTH, queryInfo, &queryInfoLen, NULL) == TRUE)
				total = _wtol(queryInfo);

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

				if (!handler(buff, bytesRead, total, handlerData)) {
					InternetCloseHandle(hRequest);
					return false;
				}
			}

			InternetCloseHandle(hRequest);
			return true;
		}

		bool Http::download2file(const std::wstring& path, BinaryFile* f) {
			if (!f->isOpen() && !f->openTmp())
				return false;

			bool ret = download(path, download2fileHandler, f);
			f->close();
			return ret;
		}

		bool Http::download2buff(const std::wstring& path, Buffer* buff) {
			bool ret = download(path, download2buffHandler, buff);
			if (ret) {
				BYTE zero[1] = { 0 };
				buff->append(zero, 1);
			}
			return ret;
		}
	}
}
