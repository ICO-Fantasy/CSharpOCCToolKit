#pragma once
#include <cstring>
#include <Resource_Unicode.hxx>
#include <Standard_TypeDef.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <vcclr.h> 

namespace OCCTK {
namespace DataExchange {

// 将C#字符串转换为TCollection_AsciiString字符串
inline TCollection_AsciiString ToAsciiString(System::String^ theString) {
	if (theString == nullptr) {
		return TCollection_AsciiString();
	}

	pin_ptr<const wchar_t> aPinChars = PtrToStringChars(theString);
	const wchar_t* aWCharPtr = aPinChars;
	if (aWCharPtr == NULL || *aWCharPtr == L'\0') {
		return TCollection_AsciiString();
	}
	return TCollection_AsciiString(aWCharPtr);
}

// 将C#字符串转换为TCollection_AsciiString字符串
inline Standard_CString ToCString(System::String^ theString) {
	return ToAsciiString(theString).ToCString();
}
// 转换中文字符为Unicode
inline TCollection_ExtendedString ConvertChineseToUnicode(const char* chineseString) {
	TCollection_ExtendedString unicodeString("");
	Resource_Unicode::ConvertGBToUnicode(Standard_CString(chineseString), unicodeString);
	return unicodeString;
}

// 转换中文字符为Unicode
inline TCollection_ExtendedString ConvertChineseToUnicode(System::String^ chineseString) {
	// 使用 Marshal 类将 System::String^ 转换为标准的 const char*
	System::IntPtr ptr = System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(chineseString);
	const char* nativeString = static_cast<const char*>(ptr.ToPointer());

	// 创建一个 TCollection_ExtendedString 来保存 Unicode 字符串
	TCollection_ExtendedString unicodeString("");

	// 调用 Resource_Unicode 的 ConvertGBToUnicode 方法进行转换
	Resource_Unicode::ConvertGBToUnicode(Standard_CString(nativeString), unicodeString);

	// 释放内存
	System::Runtime::InteropServices::Marshal::FreeHGlobal(ptr);

	return unicodeString;
}

inline System::String^ WcharToString(const wchar_t* wcharString) {
	size_t length = wcslen(wcharString);
	if (length == 0) {
		throw gcnew System::Exception("空字符串");
	}
	System::IntPtr bfr = System::IntPtr(const_cast<wchar_t*>(wcharString));
	return System::Runtime::InteropServices::Marshal::PtrToStringUni(bfr, length);
}

}
}
