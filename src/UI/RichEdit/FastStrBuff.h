#pragma once

#include <string>

namespace MyRichEdit {
	class FastStrBuff {
		wchar_t*	_data;
		size_t		_len;
		size_t		_reserved;
	public:
		FastStrBuff();
		~FastStrBuff() {
			delete [] _data;
		}

		void reserve(size_t length);

		inline void clear() {
			_data[0] = 0;
			_len = 0;
		}

		inline void push_back(wchar_t c) {
			if(_len >= _reserved - 1)
				reserve(_reserved<<1);
			_data[_len] = c;
			_data[++_len] = 0;
		}

		inline size_t size() const {
			return _len;
		}

		inline wchar_t* c_str() const {
			return _data;
		}

		std::wstring firstChars(int count);

		// Compare with ignoring case
		inline bool equalIC(const wchar_t* str) const {
			return (_wcsicmp(_data, str) == 0);
		}

		inline bool operator==(const wchar_t* str) const {
			return (wcscmp(_data, str) == 0);
		}

		inline bool operator!=(const wchar_t* str) const {
			return (wcscmp(_data, str) != 0);
		}

		inline wchar_t operator[](int index) {
			return _data[index];
		}
	};
}
