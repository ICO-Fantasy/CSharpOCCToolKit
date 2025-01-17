﻿#include "ICO_Ax1.h"
#include <gp_Ax1.hxx>
#include <gp_Ax3.hxx>
//local
#include "ICO_Dir.h"
#include "ICO_Pnt.h"
#include "ICO_Trsf.h"

namespace OCCTK {
namespace OCC {
namespace gp {

Ax1::Ax1(Pnt location, Dir direction) {
    Location = location;
    Direction = direction;
}

Ax1::Ax1(Pnt fromPnt, Pnt toPnt) {
    Location = fromPnt;
    Direction = Dir(fromPnt, toPnt);
}

Ax1::Ax1(gp_Ax1 theAx1) {
    Location = Pnt(theAx1.Location());
    Direction = Dir(theAx1.Direction());
}

Ax1::Ax1(gp_Ax1* theAx1) {
    Location = Pnt(theAx1->Location());
    Direction = Dir(theAx1->Direction());
}

gp_Ax1 Ax1::GetOCC() { return gp_Ax1(Location.GetOCC(), Direction); }

System::Object^ Ax1::Clone() {
    return Ax1(Location, Direction);
}

System::String^ Ax1::ToString() {
    return "(" + Location.X.ToString("F1") + ", " + Location.Y.ToString("F1") + ", " + Location.Z.ToString("F1") + "), (" + Direction.X.ToString("F1") + ", " + Direction.Y.ToString("F1") + ", " + Direction.Z.ToString("F1") + ")";
}

void Ax1::GetObjectData(System::Runtime::Serialization::SerializationInfo^ info, System::Runtime::Serialization::StreamingContext context)
{
    if (info == nullptr) throw gcnew System::ArgumentNullException("info");
    // 使用 gp_Ax1 的 DumpJson 方法获取 JSON 字符串
    std::ostringstream oss;
    GetOCC().DumpJson(oss);  // 假设 Ax1 是 gp_Ax1 类型

    // 将 JSON 字符串存储到 SerializationInfo 中
    info->AddValue("Ax1Json", gcnew System::String(oss.str().c_str()));
}

Ax1::Ax1(System::Runtime::Serialization::SerializationInfo^ info, System::Runtime::Serialization::StreamingContext context)
{
    if (info == nullptr) throw gcnew System::ArgumentNullException("info");
    // 从 SerializationInfo 中获取 JSON 字符串
    System::String^ json = (System::String^)info->GetValue("Ax1Json", System::String::typeid);
    const char* jsonChar = (const char*)(System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(json)).ToPointer();

    // 使用 InitFromJson 从字符串恢复 gp_Ax1 对象
    Standard_SStream jsonStream(jsonChar);
    gp_Ax1 a;
    int strPose = 0;
    a.InitFromJson(jsonStream, strPose);  // 假设 0 是起始位置

    // 释放内存
    System::Runtime::InteropServices::Marshal::FreeHGlobal(System::IntPtr((void*)jsonChar));
}

//void Ax1::Transform(Trsf^ theT) {
//	gp_Ax1 a = gp_Ax1(Location.GetOCC(), Direction).Transformed(theT);
//	Location = Pnt(a.Location());
//	Direction = Dir(a.Direction());
//}

Ax1 Ax1::Transformed(Trsf^ theT) {
    return Ax1(gp_Ax1(Location.GetOCC(), Direction).Transformed(theT));
}

//void Ax1::Reverse() {
//	Direction.Reverse();
//}

Ax1 Ax1::Reversed() {
    return Ax1(Location, Direction.Reversed());
}

/// <summary>
/// 是否同轴（Direction方向相同）
/// </summary>
/// <param name="other"></param>
/// <param name="AngularTOL"></param>
/// <param name="LinearTOL"></param>
/// <returns></returns>
bool Ax1::IsCoaxial(Ax1 other, double AngularTOL, double LinearTOL) {
    return GetOCC().IsCoaxial(other, AngularTOL, LinearTOL);
}

/// <summary>
/// 是否共线（Direction方向可以相反）
/// </summary>
/// <param name="other"></param>
/// <param name="AngularTOL"></param>
/// <returns></returns>
bool Ax1::IsCollinear(Ax1 other, double AngularTOL) {
    if (Location.Distance(other.Location) > 1e-4) {
        Dir tempDir = Dir(Location, other.Location);
        if (!tempDir.IsParallel(Direction, AngularTOL)) {
            return false;
        }
    }
    return Direction.IsParallel(other.Direction, AngularTOL);
}

}
}
}