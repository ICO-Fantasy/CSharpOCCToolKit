#include "ICO_Ax3.h"
#include <gp_Ax3.hxx>
//local
#include "ICO_Trsf.h"
#include "ICO_Ax1.h"

using namespace System;

namespace OCCTK {
namespace OCC {
namespace gp {

Ax3::Ax3(Pnt location, Dir zAxis) {
    gp_Ax3 ax3(location, zAxis);
    Location = Pnt(ax3.Location());
    ZDir = Dir(ax3.Direction());
    XDir = Dir(ax3.XDirection());
}

Ax3::Ax3(Pnt location, Dir zAxis, Dir xAxis) {
    Location = location;
    ZDir = zAxis;
    XDir = xAxis;
}

Ax3::Ax3(gp_Ax3 theAx3) {
    Location = Pnt(theAx3.Location());
    ZDir = Dir(theAx3.Direction());
    XDir = Dir(theAx3.XDirection());
}

Ax3::Ax3(gp_Ax3* theAx3) {
    Location = Pnt(theAx3->Location());
    ZDir = Dir(theAx3->Direction());
    XDir = Dir(theAx3->XDirection());
}

gp_Ax3 Ax3::GetOCC() {
    return gp_Ax3(Location, ZDir, XDir);
}

Object^ Ax3::Clone() {
    return Ax3(Location, ZDir, XDir);
}

System::String^ Ax3::ToString() {
    TCollection_AsciiString theString;
    System::String^ str = "(" + Location.X.ToString("F1") + ", " + Location.Y.ToString("F1") + ", " + Location.Z.ToString("F1") + "), (" + ZDir.X.ToString("F1") + ", " + ZDir.Y.ToString("F1") + ", " + ZDir.Z.ToString("F1") + ")";
    return str;
}

void Ax3::GetObjectData(System::Runtime::Serialization::SerializationInfo^ info, System::Runtime::Serialization::StreamingContext context)
{
    if (info == nullptr) throw gcnew System::ArgumentNullException("info");
    // 使用 gp_Ax3 的 DumpJson 方法获取 JSON 字符串
    std::ostringstream oss;
    GetOCC().DumpJson(oss);  // 假设 Ax3 是 gp_Ax3 类型

    // 将 JSON 字符串存储到 SerializationInfo 中
    info->AddValue("Ax3Json", gcnew System::String(oss.str().c_str()));
}

Ax3::Ax3(System::Runtime::Serialization::SerializationInfo^ info, System::Runtime::Serialization::StreamingContext context)
{
    if (info == nullptr) throw gcnew System::ArgumentNullException("info");
    // 从 SerializationInfo 中获取 JSON 字符串
    System::String^ json = (System::String^)info->GetValue("Ax3Json", System::String::typeid);
    const char* jsonChar = (const char*)(System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(json)).ToPointer();

    // 使用 InitFromJson 从字符串恢复 gp_Ax3 对象
    Standard_SStream jsonStream(jsonChar);
    gp_Ax3 a;
    int strPose = 0;
    a.InitFromJson(jsonStream, strPose);  // 假设 0 是起始位置

    // 释放内存
    System::Runtime::InteropServices::Marshal::FreeHGlobal(System::IntPtr((void*)jsonChar));
}

//void Ax3::Transform(Trsf^ theT) {
//	gp_Ax3 ax3(Location, ZDir, XDir);
//	ax3.Transform(theT);
//	Location = Pnt(ax3.Location());
//	ZDir = Dir(ax3.Direction());
//	XDir = Dir(ax3.XDirection());
//}

Ax3 Ax3::Transformed(Trsf^ theT) {
    gp_Ax3 ax3(Location, ZDir, XDir);
    return Ax3(ax3.Transformed(theT));
}
Dir Ax3::YDir::get() {
    return Dir(GetOCC().YDirection());
}

Ax1 Ax3::XAxis::get() {
    return Ax1(Location, XDir);
}

Ax1 Ax3::YAxis::get() {
    return Ax1(Location, YDir);
}

Ax1 Ax3::ZAxis::get() {
    return Ax1(Location, ZDir);
}

}
}
}