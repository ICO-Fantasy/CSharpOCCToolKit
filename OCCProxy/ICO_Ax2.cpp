#include "ICO_Ax2.h"
#include <gp_Ax3.hxx>
#include <gp_Ax2.hxx>
//local
#include "ICO_Pnt.h"
#include "ICO_Dir.h"
#include "ICO_Ax1.h"
#include "ICO_Trsf.h"

using namespace System;

namespace OCCTK {
namespace OCC {
namespace gp {

/// <summary>
/// X方向任意
/// </summary>
/// <param name="location"></param>
/// <param name="theZAxis"></param>
Ax2::Ax2(Pnt location, Dir zAxis) {
    gp_Ax2 theAx2 = gp_Ax2(location, zAxis);
    Location = Pnt(theAx2.Location());
    ZDir = Dir(theAx2.Direction());
    XDir = Dir(theAx2.XDirection());
}

Ax2::Ax2(Pnt location, Dir zAxis, Dir theXAxis) {
    Location = location;
    ZDir = zAxis;
    XDir = theXAxis;
}

Ax2::Ax2(gp_Ax2 theAx2) {
    Location = Pnt(theAx2.Location());
    ZDir = Dir(theAx2.Direction());
    XDir = Dir(theAx2.XDirection());
}

Ax2::Ax2(gp_Ax2* theAx2) {
    Location = Pnt(theAx2->Location());
    ZDir = Dir(theAx2->Direction());
    XDir = Dir(theAx2->XDirection());
}

Ax2::Ax2(array<double, 2>^ matrix) {
    if (matrix->GetLength(0) != 4 || matrix->GetLength(1) != 4) {
        throw gcnew System::ArgumentException("The matrix must be 4x4.");
    }
    gp_Ax2 newAx2 = gp_Ax2();
    gp_Trsf T = gp_Trsf();
    T.SetValues(
        matrix[0, 0], matrix[0, 1], matrix[0, 2], matrix[0, 3],
        matrix[1, 0], matrix[1, 1], matrix[1, 2], matrix[1, 3],
        matrix[2, 0], matrix[2, 1], matrix[2, 2], matrix[2, 3]
    );
    newAx2.Transform(T);
    Location = Pnt(newAx2.Location());
    ZDir = Dir(newAx2.Direction());
    XDir = Dir(newAx2.XDirection());
}

Ax2::Ax2(array<array<double>^>^ matrix) {
    if (matrix->Length != 4) {
        throw gcnew System::ArgumentException("The matrix must be 4x4.");
    }
    for each (auto var in matrix) {
        if (var->Length != 4) {
            throw gcnew System::ArgumentException("The matrix must be 4x4.");
        }
    }
    gp_Ax2 newAx2 = gp_Ax2();
    gp_Trsf T = gp_Trsf();
    T.SetValues(
        matrix[0][0], matrix[0][1], matrix[0][2], matrix[0][3],
        matrix[1][0], matrix[1][1], matrix[1][2], matrix[1][3],
        matrix[2][0], matrix[2][1], matrix[2][2], matrix[2][3]
    );
    newAx2.Transform(T);
    Location = Pnt(newAx2.Location());
    ZDir = Dir(newAx2.Direction());
    XDir = Dir(newAx2.XDirection());
}

gp_Ax2 Ax2::GetOCC() {
    return gp_Ax2(Location, ZDir, XDir);
}

Object^ Ax2::Clone() {
    return Ax2(Location, ZDir, XDir);
}

System::String^ Ax2::ToString() {
    System::String^ str = "(" + Location.ToString() + ") Z: (" + ZDir.ToString() + ")X: (" + XDir.ToString() + ")";
    return str;
}

void Ax2::GetObjectData(System::Runtime::Serialization::SerializationInfo^ info, System::Runtime::Serialization::StreamingContext context)
{
    if (info == nullptr) throw gcnew System::ArgumentNullException("info");
    // 使用 gp_Ax2 的 DumpJson 方法获取 JSON 字符串
    std::ostringstream oss;
    GetOCC().DumpJson(oss);  // 假设 Ax2 是 gp_Ax2 类型

    // 将 JSON 字符串存储到 SerializationInfo 中
    info->AddValue("Ax2Json", gcnew System::String(oss.str().c_str()));
}

Ax2::Ax2(System::Runtime::Serialization::SerializationInfo^ info, System::Runtime::Serialization::StreamingContext context)
{
    if (info == nullptr) throw gcnew System::ArgumentNullException("info");
    // 从 SerializationInfo 中获取 JSON 字符串
    System::String^ json = (System::String^)info->GetValue("Ax2Json", System::String::typeid);
    const char* jsonChar = (const char*)(System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(json)).ToPointer();

    // 使用 InitFromJson 从字符串恢复 gp_Ax2 对象
    Standard_SStream jsonStream(jsonChar);
    gp_Ax2 a;
    int strPose = 0;
    a.InitFromJson(jsonStream, strPose);  // 假设 0 是起始位置

    // 释放内存
    System::Runtime::InteropServices::Marshal::FreeHGlobal(System::IntPtr((void*)jsonChar));
}


Ax2 Ax2::Transformed(Trsf^ theT) {
    return Ax2(GetOCC().Transformed(theT));
}

Dir Ax2::YDir::get() {
    return Dir(GetOCC().YDirection());
}

Ax1 Ax2::XAxis::get() {
    return Ax1(Location, XDir);
}

Ax1 Ax2::YAxis::get() {
    return Ax1(Location, YDir);
}

Ax1 Ax2::ZAxis::get() {
    return Ax1(Location, ZDir);
}

}
}
}