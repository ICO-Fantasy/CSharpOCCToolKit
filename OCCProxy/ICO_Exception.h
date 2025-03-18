#pragma once
#include <exception>
#include <Standard_Failure.hxx>
using namespace System;

// 定义宏来简化异常处理
#define THROW_MANAGED_EXCEPTION(e) throw gcnew System::Exception(gcnew System::String(e.what()));

#define THROW_STANDARD_FAILURE(e) throw gcnew System::Exception(gcnew System::String(e.GetMessageString()));

// 定义宏来捕获 std::exception 和 Standard_Failure 异常，并抛出托管的 System::Exception
#define CATCH_AND_THROW_OCC_EXCEPTIONS                                    \
    catch (const std::exception& e) {                                           \
        throw gcnew System::Exception(gcnew System::String(e.what())); \
    }                                                                           \
    catch (const Standard_Failure& e) {                                         \
        throw gcnew System::Exception(gcnew System::String(e.GetMessageString())); \
    }                                                                           \
    catch (...) {\
        throw gcnew System::Exception("未知错误");\
    }

// 定义宏来执行语句，并捕获 std::exception 和 Standard_Failure 异常，抛出托管的 System::Exception
#define TRY_OCC(func) \
    try { \
        func \
    } \
    CATCH_AND_THROW_OCC_EXCEPTIONS
