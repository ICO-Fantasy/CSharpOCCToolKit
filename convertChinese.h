#pragma once
#include <TCollection_ExtendedString.hxx>
#include <Resource_Unicode.hxx>

/// <summary>
/// 转换中文字符为Unicode
/// </summary>
/// <param name="chineseString"></param>
/// <returns>TCollection_ExtendedString</returns>
static TCollection_ExtendedString ConvertChineseToUnicode(const char* chineseString)
{
	TCollection_ExtendedString unicodeString("");
	Resource_Unicode::ConvertGBToUnicode(Standard_CString(chineseString), unicodeString);
	return unicodeString;
}

