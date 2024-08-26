#pragma once
#include <TCollection_ExtendedString.hxx>
#include <Resource_Unicode.hxx>
namespace OCCTK {
namespace Tool {

// 转换中文字符为Unicode
static TCollection_ExtendedString ConvertChineseToUnicode(const char* chineseString) {
	TCollection_ExtendedString unicodeString("");
	Resource_Unicode::ConvertGBToUnicode(Standard_CString(chineseString), unicodeString);
	return unicodeString;
}

// 转换中文字符为Unicode
static TCollection_ExtendedString ConvertChineseToUnicode(System::String^ chineseString) {
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

}
}

