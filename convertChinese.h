#pragma once
#include <TCollection_ExtendedString.hxx>
#include <Resource_Unicode.hxx>
static TCollection_ExtendedString ConvertChineseToUnicode(const char* chineseString)
{
	TCollection_ExtendedString unicodeString("");
	Resource_Unicode::ConvertGBToUnicode(Standard_CString(chineseString), unicodeString);
	return unicodeString;
}

