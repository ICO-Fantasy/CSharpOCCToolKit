#pragma once
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <Resource_Unicode.hxx>
#include <vcclr.h> 

using namespace System;
namespace OCCTK {
namespace DataExchange
{

/// <summary>
/// 将C#字符串转换为TCollection_AsciiString字符串
/// </summary>
/// <param name="theString"></param>
/// <returns></returns>
TCollection_AsciiString toAsciiString(System::String^ theString) {
	if (theString == nullptr)
	{
		return TCollection_AsciiString();
	}

	pin_ptr<const wchar_t> aPinChars = PtrToStringChars(theString);
	const wchar_t* aWCharPtr = aPinChars;
	if (aWCharPtr == NULL || *aWCharPtr == L'\0')
	{
		return TCollection_AsciiString();
	}
	return TCollection_AsciiString(aWCharPtr);
}

}
}
