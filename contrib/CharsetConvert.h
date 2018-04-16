/*****************************************************************************
*  @file     CharsetConvert.h
*  @brief    字符串编码转换
*  @note
*  通过MultiByteToWideChar()、WideCharToMultiByte()这Windows API
*  来实现UIF16、MBCS、UTF8字符串编码的相互转换的
*  拓展链接：
*  文件编码检测：https://github.com/BYVoid/uchardet
*  文件编码转换：http://www.gnu.org/software/libiconv/
*****************************************************************************/
#ifndef __CHARSETCONVERT_H
#define __CHARSETCONVERT_H
#include <string>

namespace CharsetConvert {

/////////////////////////转换字符编码/////////////////////////
/** 
* @brief: Mbcs编码转为Utf8编码
* @param[in] const char*  szMbcs  待转换的Mbcs字符编码字符串
* @param[in] int  mbcsLen  待转换的字符串的长度
* @return  返回转换后的字符串
* @note   
* 
*/
std::string MBCSToUTF8(const char* szMbcs, int mbcsLen);

/** 
* @brief: Mbcs编码转为Utf16编码
* @param[in] const char*  szMbcs  待转换的Mbcs字符编码字符串
* @param[in] int  mbcsLen  待转换的字符串的长度
* @return  返回转换后的字符串
* @note   
*/
std::wstring MBCSToUTF16(const char* szMbcs, int mbcsLen);

/** 
* @brief: Utf8编码转为Mbcs编码
* @param[in] const char*  szUtf8  待转换的szUtf8字符编码字符串
* @param[in] int  utf8Len  待转换的字符串的长度
* @return  返回转换后的字符串
* @note   
*/
std::string UTF8ToMBCS(const char* szUtf8, int utf8Len);

/** 
* @brief: Utf8编码转为Utf16编码
* @param[in] const char*  szUtf8  待转换的Utf8字符编码字符串
* @param[in] int  utf8Len  待转换的字符串的长度
* @return  返回转换后的字符串
* @note   
*/
std::wstring UTF8ToUTF16(const char* szUtf8, int utf8Len);

/** 
* @brief: Utf16编码转为Mbcs编码
* @param[in] const wchar_t*  szUtf16  待转换的Utf16字符编码字符串
* @param[in] int  utf16Len  待转换的字符串的长度
* @return  返回转换后的字符串
* @note 
*/
std::string UTF16ToMBCS(const wchar_t* szUtf16, int utf16Len);

/** 
* @brief: Utf16编码转为Utf8编码
* @param[in] const wchar_t*  szUtf16  待转换的Utf16字符编码字符串
* @param[in] int  utf16Len  待转换的字符串的长度
* @return  返回转换后的字符串
* @note 
*/
std::string UTF16ToUTF8(const wchar_t* szUtf16, int utf16Len);

/** 
* @brief: Mbcs编码转为Utf8编码
* @param[in] std::string  szMbcs  待转换的Mbcs字符编码字符串
* @return  返回转换后的字符串
* @note 
*/
std::string MBCSToUTF8(std::string szMbcs);

/** 
* @brief: Mbcs编码转为Utf16编码
* @param[in] std::string  szMbcs  待转换的Mbcs字符编码字符串
* @return  返回转换后的字符串
* @note 
*/
std::wstring MBCSToUTF16(std::string szMbcs);

/** 
* @brief: Utf8编码转为Mbcs编码
* @param[in] std::string  szUtf8  待转换的Utf8字符编码字符串
* @return  返回转换后的字符串
* @note 
*/
std::string UTF8ToMBCS(std::string szUtf8);

/** 
* @brief: UTF8编码转为Utf16编码
* @param[in] std::string  szUtf8  待转换的Utf8字符编码字符串
* @return  返回转换后的字符串
* @note 
*/
std::wstring UTF8ToUTF16(std::string szUtf8);

/** @brief: Utf16编码转为Mbcs编码
* @param[in] std::string  szUtf16  待转换的Utf16字符编码字符串
* @return  返回转换后的字符串
* @note 
*/
std::string UTF16ToMBCS(std::wstring szUtf16);

/** 
* @brief: Utf16编码转为UTF8
* @param[in] std::string  szUtf16  待转换的Utf16字符编码字符串
* @return  返回转换后的字符串
* @note 
*/
std::string UTF16ToUTF8(std::wstring szUtf16);

/////////////////////////检测字符编码/////////////////////////
/** 
* @brief: 判断字符串内是否包含utf8编码字符，
*		  以此判断此字符串是否为utf8编码
* @param[in] const  char*  待检测的字符串
* @return  如果返回为true，则此字符串属于utf8编码		   
* @note 
* 此函数提取自licecap项目
*/
bool HasUTF8(const char *szSource);

}
#endif

/*********************************调用演示************************************
#include <string>
void main()
{
	std::string szSrc1 = "你好，世界！";
	std::string szDst_Utf8 = CharsetConvert::MBCSToUTF8(szSrc1);
	std::wstring szDst_Utf16 = CharsetConvert::MBCSToUTF16(szSrc1);

	std::wstring szSrc2 = L"你好，世界！";
	std::string szDst_Mbcs = CharsetConvert::UTF16ToMBCS(szSrc2);
	std::string szDst_Utf82 = CharsetConvert::UTF16ToUTF8(szSrc2);

	if (szDst_Utf8 == szDst_Utf82)
	{
		OutputDebugStringA("Utf8匹配\n");
	}

	if (szSrc1 == szDst_Mbcs)
	{
		OutputDebugStringA("Mbcs匹配\n");
	}

	if (szDst_Utf16 == szSrc2)
	{
		OutputDebugStringA("Utf16匹配\n");
	}
}
*****************************************************************************/