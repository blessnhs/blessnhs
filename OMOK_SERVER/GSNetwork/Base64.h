#pragma once
#include <stdlib.h>
#include <string>
#include <iostream>
#include <sstream>
#include<Windows.h>

class Base64
{
 public:

  static std::string Encode(const std::string data) {
	static constexpr char sEncodingTable[] = {
	  'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
	  'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
	  'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
	  'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
	  'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
	  'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
	  'w', 'x', 'y', 'z', '0', '1', '2', '3',
	  '4', '5', '6', '7', '8', '9', '+', '/'
	};

	size_t in_len = data.size();
	size_t out_len = 4 * ((in_len + 2) / 3);
	std::string ret(out_len, '\0');
	size_t i;
	char* p = const_cast<char*>(ret.c_str());

	for (i = 0; i < in_len - 2; i += 3) {
	  *p++ = sEncodingTable[(data[i] >> 2) & 0x3F];
	  *p++ = sEncodingTable[((data[i] & 0x3) << 4) | ((int)(data[i + 1] & 0xF0) >> 4)];
	  *p++ = sEncodingTable[((data[i + 1] & 0xF) << 2) | ((int)(data[i + 2] & 0xC0) >> 6)];
	  *p++ = sEncodingTable[data[i + 2] & 0x3F];
	}
	if (i < in_len) {
	  *p++ = sEncodingTable[(data[i] >> 2) & 0x3F];
	  if (i == (in_len - 1)) {
		*p++ = sEncodingTable[((data[i] & 0x3) << 4)];
		*p++ = '=';
	  }
	  else {
		*p++ = sEncodingTable[((data[i] & 0x3) << 4) | ((int)(data[i + 1] & 0xF0) >> 4)];
		*p++ = sEncodingTable[((data[i + 1] & 0xF) << 2)];
	  }
	  *p++ = '=';
	}

	return ret;
  }

  static std::string Decode(const std::string & input, std::string & out) {
	static constexpr unsigned char kDecodingTable[] = {
	  64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	  64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	  64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 62, 64, 64, 64, 63,
	  52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 64, 64, 64, 64, 64, 64,
	  64,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
	  15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 64, 64, 64, 64, 64,
	  64, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
	  41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 64, 64, 64, 64, 64,
	  64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	  64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	  64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	  64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	  64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	  64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	  64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	  64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64
	};

	size_t in_len = input.size();
	if (in_len % 4 != 0) return "Input data size is not a multiple of 4";

	size_t out_len = in_len / 4 * 3;
	if (input[in_len - 1] == '=') out_len--;
	if (input[in_len - 2] == '=') out_len--;

	out.resize(out_len);

	for (size_t i = 0, j = 0; i < in_len;) {
	  uint32_t a = input[i] == '=' ? 0 & i++ : kDecodingTable[static_cast<int>(input[i++])];
	  uint32_t b = input[i] == '=' ? 0 & i++ : kDecodingTable[static_cast<int>(input[i++])];
	  uint32_t c = input[i] == '=' ? 0 & i++ : kDecodingTable[static_cast<int>(input[i++])];
	  uint32_t d = input[i] == '=' ? 0 & i++ : kDecodingTable[static_cast<int>(input[i++])];

	  uint32_t triple = (a << 3 * 6) + (b << 2 * 6) + (c << 1 * 6) + (d << 0 * 6);

	  if (j < out_len) out[j++] = (triple >> 2 * 8) & 0xFF;
	  if (j < out_len) out[j++] = (triple >> 1 * 8) & 0xFF;
	  if (j < out_len) out[j++] = (triple >> 0 * 8) & 0xFF;
	}

	return "";
  }

  static DWORD convert_unicode_to_ansi_string(
	  __out std::string& ansi,
	  __in const wchar_t* unicode,
	  __in const size_t unicode_size
  ) {
	  DWORD error = 0;
	  do {
		  if ((nullptr == unicode) || (0 == unicode_size)) {
			  error = ERROR_INVALID_PARAMETER;
			  break;
		  }
		  ansi.clear();
		  //
		  // getting required cch.
		  //
		  int required_cch = ::WideCharToMultiByte(
			  CP_ACP,
			  0,
			  unicode, static_cast<int>(unicode_size),
			  nullptr, 0,
			  nullptr, nullptr
		  );
		  if (0 == required_cch) {
			  error = ::GetLastError();
			  break;
		  }
		  //
		  // allocate.
		  //
		  ansi.resize(required_cch);
		  //
		  // convert.
		  //
		  if (0 == ::WideCharToMultiByte(
			  CP_ACP,
			  0,
			  unicode, static_cast<int>(unicode_size),
			  const_cast<char*>(ansi.c_str()), static_cast<int>(ansi.size()),
			  nullptr, nullptr
		  )) {
			  error = ::GetLastError();
			  break;
		  }
	  } while (false);
	  return error;
  }

  static DWORD convert_unicode_to_utf8_string(
	  __out std::string& utf8,
	  __in const wchar_t* unicode,
	  __in const size_t unicode_size
  ) {
	  DWORD error = 0;
	  do {
		  if ((nullptr == unicode) || (0 == unicode_size)) {
			  error = ERROR_INVALID_PARAMETER;
			  break;
		  }
		  utf8.clear();
		  //
		  // getting required cch.
		  //
		  int required_cch = ::WideCharToMultiByte(
			  CP_UTF8,
			  WC_ERR_INVALID_CHARS,
			  unicode, static_cast<int>(unicode_size),
			  nullptr, 0,
			  nullptr, nullptr
		  );
		  if (0 == required_cch) {
			  error = ::GetLastError();
			  break;
		  }
		  //
		  // allocate.
		  //
		  utf8.resize(required_cch);
		  //
		  // convert.
		  //
		  if (0 == ::WideCharToMultiByte(
			  CP_UTF8,
			  WC_ERR_INVALID_CHARS,
			  unicode, static_cast<int>(unicode_size),
			  const_cast<char*>(utf8.c_str()), static_cast<int>(utf8.size()),
			  nullptr, nullptr
		  )) {
			  error = ::GetLastError();
			  break;
		  }
	  } while (false);
	  return error;
  }

};
