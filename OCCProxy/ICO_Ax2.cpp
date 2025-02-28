﻿#include "ICO_Ax2.h"
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