#include "ICO_Ax2.h"
#include <gp_Ax2.hxx>
//local
#include "ICO_Pnt.h"
#include "ICO_Dir.h"
#include "ICO_Ax1.h"
#include "ICO_Trsf.h"
#include "../Extension/ICO_WPR.h"
#include "ICO_Vec.h"

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

Ax2::Ax2(Ax2 fromAx2, Trsf^ transfrom) {
    Ax2 toAx2 = fromAx2.Transformed(transfrom);
    Location = toAx2.Location;
    ZDir = toAx2.ZDir;
    XDir = toAx2.XDir;
}

/// <summary>
/// 从原点应用变换创建Ax2
/// </summary>
/// <param name="transfrom"></param>
Ax2::Ax2(Trsf^ transfrom)
{
    gp_Ax2 ax2;
    ax2.Transform(transfrom);
    Location = Pnt(ax2.Location());
    ZDir = Dir(ax2.Direction());
    XDir = Dir(ax2.XDirection());
}

Ax2::Ax2(Pnt location, WPR direction) {
    gp_Ax2 ax2;
    ax2.Transform(gcnew Trsf(Vec(Pnt(0, 0, 0), location), direction));

    Location = Pnt(ax2.Location());
    ZDir = Dir(ax2.Direction());
    XDir = Dir(ax2.XDirection());
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