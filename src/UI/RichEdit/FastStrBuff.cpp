#pragma once

#include "FastStrBuff.h"

namespace MyRichEdit {
	FastStrBuff::FastStrBuff() {
		_len = 0;
		_reserved = 5;
		_data = new wchar_t[_reserved];
		_data[0] = 0;
	}

	void FastStrBuff::reserve(size_t length) {
		wchar_t* newData = new wchar_t[length];
		wcscpy_s(newData, length, _data);
		delete [] _data;
		_data = newData;
		_reserved = length;
	}

	std::wstring FastStrBuff::firstChars(int count) {
		wchar_t c = _data[count];
		_data[count] = 0;
		std::wstring sub(_data);
		_data[count] = c;
		return sub;
	}
}
