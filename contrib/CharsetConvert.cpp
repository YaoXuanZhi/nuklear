#include "CharsetConvert.h"
#include <windows.h>
/*
注意，因为在VC6.0里的std::string并没有clear()成员函数，
因此为了兼容低版本的MSVC，采用resize(0)来释放std::string的内存
*/
namespace CharsetConvert 
{
	std::string MBCSToUTF8(const char * szMbcs, int mbcsLen)
	{
		int nLen = MultiByteToWideChar(CP_ACP, 0, szMbcs, mbcsLen, NULL, 0);

		std::wstring temp;
		temp.resize(nLen);
		int nRtn = MultiByteToWideChar(CP_ACP, 0, szMbcs, mbcsLen, &temp[0], nLen);

		if (nRtn != nLen)
			return "";

		int utf8Len = WideCharToMultiByte(CP_UTF8, 0, &temp[0], nLen, NULL, 0, NULL, NULL);
		if (utf8Len <= 0)
			return "";

		std::string szUtf8;
		szUtf8.resize(utf8Len);
		nRtn = WideCharToMultiByte(CP_UTF8, 0, &temp[0], nLen, &szUtf8[0], utf8Len, NULL, NULL);

		if (nRtn != utf8Len)
			szUtf8.resize(0);
		return szUtf8;
	}

	std::wstring MBCSToUTF16(const char * szMbcs, int mbcsLen)
	{
		int uLen = MultiByteToWideChar(CP_ACP, 0, szMbcs, mbcsLen, NULL, 0);

		if (uLen <= 0)
			return L"";

		std::wstring szUnicode;
		szUnicode.resize(uLen);

		int nRtn = MultiByteToWideChar(CP_ACP, 0, szMbcs, mbcsLen, &szUnicode[0], uLen);

		if (nRtn != uLen)
			szUnicode.resize(0);
		return szUnicode;
	}

	std::string UTF8ToMBCS(const char * szUtf8, int utf8Len)
	{
		int nLen = MultiByteToWideChar(CP_UTF8, 0, szUtf8, utf8Len, NULL, 0);

		std::wstring temp;
		temp.resize(nLen);

		int nRtn = MultiByteToWideChar(CP_UTF8, 0, szUtf8, utf8Len, &temp[0], nLen);

		if (nRtn != nLen)
			return "";

		int MBLen = WideCharToMultiByte(CP_ACP, 0, &temp[0], nLen, NULL, 0, NULL, NULL);
		if (MBLen <= 0)
		{
			return "";
		}
		std::string szMbcs;
		szMbcs.resize(MBLen);
		nRtn = WideCharToMultiByte(CP_ACP, 0, &temp[0], nLen, &*szMbcs.begin(), MBLen, NULL, NULL);

		if (nRtn != MBLen)
			szMbcs.resize(0);
		return szMbcs;
	}

	std::wstring UTF8ToUTF16(const char * szUtf8, int utf8Len)
	{
		int nLen = MultiByteToWideChar(CP_UTF8, 0, szUtf8, utf8Len, NULL, 0);
		if (nLen <= 0)
			return L"";

		std::wstring szUnicode;
		szUnicode.resize(nLen);
		int nRtn = MultiByteToWideChar(CP_UTF8, 0, szUtf8, utf8Len, &szUnicode[0], nLen);

		if (nRtn != nLen)
			szUnicode.resize(0);

		return szUnicode;
	}

	std::string UTF16ToMBCS(const wchar_t * szUtf16, int utf16Len)
	{
		int MBLen = WideCharToMultiByte(CP_ACP, 0, szUtf16, utf16Len, NULL, 0, NULL, NULL);
		if (MBLen <= 0)
			return "";

		std::string szMbcs;
		szMbcs.resize(MBLen);
		int nRtn = WideCharToMultiByte(CP_ACP, 0, szUtf16, utf16Len, &szMbcs[0], MBLen, NULL, NULL);

		if (nRtn != MBLen)
			szMbcs.resize(0);

		return szMbcs;
	}

	std::string UTF16ToUTF8(const wchar_t * szUtf16, int utf16Len)
	{
		int utf8Len = WideCharToMultiByte(CP_UTF8, 0, szUtf16, utf16Len, NULL, 0, NULL, NULL);
		if (utf8Len <= 0)
			return "";

		std::string szUtf8;
		szUtf8.resize(utf8Len);
		int nRtn = WideCharToMultiByte(CP_UTF8, 0, szUtf16, utf16Len, &szUtf8[0], utf8Len, NULL, NULL);

		if (nRtn != utf8Len)
			szUtf8.resize(0);

		return szUtf8;
	}

	std::string MBCSToUTF8(std::string szMbcs)
	{
		return MBCSToUTF8(&szMbcs[0], (int)szMbcs.length());
	}

	std::wstring MBCSToUTF16(std::string szMbcs)
	{
		return MBCSToUTF16(&szMbcs[0], (int)szMbcs.length());
	}

	std::string UTF8ToMBCS(std::string szUtf8)
	{
		return UTF8ToMBCS(&szUtf8[0], (int)szUtf8.length());
	}

	std::wstring UTF8ToUTF16(std::string szUtf8)
	{
		return UTF8ToUTF16(&szUtf8[0], (int)szUtf8.length());
	}

	std::string UTF16ToMBCS(std::wstring szUtf16)
	{
		return UTF16ToMBCS(&szUtf16[0], (int)szUtf16.length());
	}

	std::string UTF16ToUTF8(std::wstring szUtf16)
	{
		return UTF16ToUTF8(&szUtf16[0], (int)szUtf16.length());
	}

	bool HasUTF8(const char * szSource)
	{
		const unsigned char *str = (const unsigned char *)szSource;
		bool hasUTF = false;

		if (str) while (*str)
		{
			unsigned char c = *str++;
			if (c < 0x80) {} // allow 7 bit ascii straight through
			else if (c < 0xC2 || c > 0xF7) return false; // treat overlongs or other values in this range as indicators of non-utf8ness
			else
			{
				hasUTF = true;
				if (str[0] < 0x80 || str[0] > 0xBF) return false;
				else if (c < 0xE0) str++;
				else if (str[1] < 0x80 || str[1] > 0xBF) return false;
				else if (c < 0xF0) str += 2;
				else if (str[2] < 0x80 || str[2] > 0xBF) return false;
				else str += 3;
			}
		}
		return hasUTF;
	}
}