#include "ICO_Ax3.h"
#include <gp_Ax3.hxx>
//local
#include "ICO_Trsf.h"
#include "ICO_Ax1.h"
#include "ICO_Ax2.h"

using namespace System;

namespace OCCTK {
namespace OCC {
namespace gp {

Ax3::Ax3(gp_Ax3 theAx3) {
    location = Pnt(theAx3.Location());
    zDir = Dir(theAx3.Direction());
    xDir = Dir(theAx3.XDirection());
}

/// <summary>
/// 左手坐标系
/// </summary>
/// <param name="location"></param>
/// <param name="zAxis"></param>
Ax3::Ax3(Pnt location, Dir zAxis) {
    gp_Ax3 ax3(location, zAxis);
    this->location = Pnt(ax3.Location());
    zDir = Dir(ax3.Direction());
    xDir = Dir(ax3.XDirection());
}

Ax3::Ax3(Pnt location, Dir zAxis, Dir xAxis) {
    this->location = location;
    zDir = zAxis;
    xDir = xAxis;
}

Ax3::Ax3(Ax2 theAx2) {
    gp_Ax3 ax3(theAx2);
    this->location = Pnt(ax3.Location());
    zDir = Dir(ax3.Direction());
    xDir = Dir(ax3.XDirection());
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

//void Ax3::Transform(Trsf theT) {
//	gp_Ax3 ax3(Location, ZDir, XDir);
//	ax3.Transform(theT);
//	location = Pnt(ax3.Location());
//	zDir = Dir(ax3.Direction());
//	xDir = Dir(ax3.XDirection());
//}

Ax3 Ax3::Transformed(Trsf theT) {
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