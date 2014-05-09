#pragma once

#include <Windows.h>
#include <WinInet.h>
#include "BinaryFile.hpp"

namespace wtwUpdate {
	namespace utils {
		/// HTTP or HTTPS connection.
		class Http {
			class HttpUrl {
				std::wstring _host;
				int _port;
				std::wstring _object;
				bool _ssl;
			public:
				HttpUrl(const std::wstring& url);

				inline std::wstring getHost() const {
					return _host;
				}

				inline int getPort() const {
					return _port;
				}

				inline std::wstring getObj() const {
					return _object;
				}

				inline bool isSsl() const {
					return _ssl;
				}
			};
		public:
			class Buffer {
				BYTE* _data;
				size_t _len;
				size_t _reserved;

				void dealloc();

				void assign(const Buffer& b);
			public:
				Buffer();
				Buffer(int reserved);
				Buffer(const Buffer& other);
				~Buffer();

				inline Buffer& operator = (const Buffer& b) {
					assign(b);
					return *this;
				}

				void reserve(size_t reserved);
				void append(const BYTE* data, size_t len);

				inline BYTE* getBuffer() const {
					return _data;
				}

				inline size_t getLen() const {
					return _len;
				}
			};

			static bool download2buffHandler(BYTE* buff, size_t len, size_t total, void* data);
			static bool download2fileHandler(BYTE* buff, size_t len, size_t total, void* data);

			HINTERNET _hInternet;
			HINTERNET _hConnect;
		public:
			Http();
			~Http();

			bool connect(const std::wstring& server, int port);
			void disconnect();

			// return false to abort download
			typedef bool(*pDownloadHandler)(BYTE* buff, size_t len, size_t total, void* data);

			bool download(const std::wstring& urlStr, pDownloadHandler handler, void* handlerData);
			bool download2file(const std::wstring& path, BinaryFile* f);
			bool download2buff(const std::wstring& path, Buffer* buff);
		};
	}
}
