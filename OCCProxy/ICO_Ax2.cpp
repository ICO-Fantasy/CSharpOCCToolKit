﻿#include "ICO_Ax2.h"
#include <gp_Ax2.hxx>
#include <gp_Ax3.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <gp_Pnt.hxx>
#include <gp_Quaternion.hxx>
#include <gp_EulerSequence.hxx>
//local
#include "ICO_Pnt.h"
#include "ICO_Dir.h"
#include "ICO_Ax1.h"
#include "ICO_Trsf.h"
#include "ICO_XYZ.h"
#include "../Extension/ICO_WPR.h"

using namespace System;

namespace OCCTK {
namespace OCC {
namespace gp {

/// <summary>
/// 
/// </summary>
/// <param name="location"></param>
/// <param name="theZAxis"></param>
/// <remarks>X方向为任意轴</remarks>
Ax2::Ax2(Pnt location, Dir zDir) {
    gp_Ax2 theAx2 = gp_Ax2(location, zDir);
    myLocation = Pnt(theAx2.Location());
    myZDir = Dir(theAx2.Direction());
    myXDir = Dir(theAx2.XDirection());
}

Ax2::Ax2(Pnt location, Dir zDir, Dir theXDir) {
    myLocation = location;
    myZDir = zDir;
    myXDir = theXDir;
}

Ax2::Ax2(Ax1 zAxis)
{
    gp_Ax2 theAx2 = gp_Ax2(zAxis.Location, zAxis.Direction);
    myLocation = Pnt(theAx2.Location());
    myZDir = Dir(theAx2.Direction());
    myXDir = Dir(theAx2.XDirection());
}

Ax2::Ax2(Ax1 zAxis, Dir XDir)
{
    myLocation = zAxis.Location;
    myZDir = zAxis.Direction;
    myXDir = XDir;
}

Ax2::Ax2(gp_Ax2 theAx2) {
    myLocation = Pnt(theAx2.Location());
    myZDir = Dir(theAx2.Direction());
    myXDir = Dir(theAx2.XDirection());
}

Ax2::Ax2(gp_Ax3 theAx3)
{
    gp_Ax2 theAx2 = theAx3.Ax2();
    myLocation = Pnt(theAx2.Location());
    myZDir = Dir(theAx2.Direction());
    myXDir = Dir(theAx2.XDirection());
}
//
//Ax2::Ax2(array<double, 2>^ matrix) {
//    if (matrix->GetLength(0) != 4 || matrix->GetLength(1) != 4) {
//        throw gcnew System::ArgumentException("The matrix must be 4x4.");
//    }
//    gp_Ax2 newAx2 = gp_Ax2();
//    gp_Trsf T = gp_Trsf();
//    T.SetValues(
//        matrix[0, 0], matrix[0, 1], matrix[0, 2], matrix[0, 3],
//        matrix[1, 0], matrix[1, 1], matrix[1, 2], matrix[1, 3],
//        matrix[2, 0], matrix[2, 1], matrix[2, 2], matrix[2, 3]
//    );
//    newAx2.Transform(T);
//    location = Pnt(newAx2.Location());
//    zDir = Dir(newAx2.Direction());
//    xDir = Dir(newAx2.XDirection());
//}
//
//Ax2::Ax2(array<array<double>^>^ matrix) {
//    if (matrix->Length != 4) {
//        throw gcnew System::ArgumentException("The matrix must be 4x4.");
//    }
//    for each (auto var in matrix) {
//        if (var->Length != 4) {
//            throw gcnew System::ArgumentException("The matrix must be 4x4.");
//        }
//    }
//    gp_Ax2 newAx2 = gp_Ax2();
//    gp_Trsf T = gp_Trsf();
//    T.SetValues(
//        matrix[0][0], matrix[0][1], matrix[0][2], matrix[0][3],
//        matrix[1][0], matrix[1][1], matrix[1][2], matrix[1][3],
//        matrix[2][0], matrix[2][1], matrix[2][2], matrix[2][3]
//    );
//    newAx2.Transform(T);
//    location = Pnt(newAx2.Location());
//    zDir = Dir(newAx2.Direction());
//    xDir = Dir(newAx2.XDirection());
//}

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

/// <summary>
/// 以自身为基准，返回变换后的坐标系
/// </summary>
/// <param name="theT"></param>
/// <returns></returns>
/// <remarks>!!! 传入的 Trsf 也必须以自身为基准 !!!</remarks>
Ax2 Ax2::LocalTransformed(Trsf theT)
{
    gp_Trsf a;
    a.SetDisplacement(gp_Ax3(), gp_Ax3(GetOCC()));
    return Ax2(gp_Ax2().Transformed(a * theT));
}

Ax2 Ax2::LocalTransformed(XYZ xyz, WPR wpr)
{
    gp_Trsf t;
    t.SetTranslationPart(gp_Vec(xyz));
    t.SetRotationPart(wpr);
    Trsf theT=Trsf(t);
    Trsf myT = Trsf(Ax2(Location, ZDir, XDir));
    return Ax2(gp_Ax2().Transformed(myT *theT));
}

Ax2 Ax2::Transformed(Trsf theT) {
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