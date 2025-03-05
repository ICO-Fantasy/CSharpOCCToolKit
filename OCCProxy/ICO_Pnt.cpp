#include "ICO_Pnt.h"
#include <cmath>
#include <Geom_CartesianPoint.hxx>
#include <gp_Pnt.hxx>
#include <gp_XYZ.hxx>
//local
#include "ICO_Trsf.h"
#include "ICO_XYZ.h"

using namespace System;

namespace OCCTK {
namespace OCC {
namespace gp {

Pnt::Pnt(double theX, double theY, double theZ) {
    x = theX;
    y = theY;
    z = theZ;
}

Pnt::Pnt(System::ValueTuple<double, double, double> theXYZ) {
    x = theXYZ.Item1;
    y = theXYZ.Item2;
    z = theXYZ.Item3;
}

Pnt::Pnt(gp_Pnt thePnt) {
    x = thePnt.X();
    y = thePnt.Y();
    z = thePnt.Z();
}

Pnt::Pnt(gp_XYZ theXYZ) {
    x = theXYZ.X();
    y = theXYZ.Y();
    z = theXYZ.Z();
}

Pnt::Pnt(XYZ theXYZ) {
    x = theXYZ.X;
    y = theXYZ.Y;
    z = theXYZ.Z;
}

Object^ Pnt::Clone() {
    return Pnt(this->X, this->Y, this->Z);;
}

gp_Pnt Pnt::GetOCC() {
    return gp_Pnt(X, Y, Z);
}

System::String^ Pnt::ToString() {
    return X.ToString("F3") + ", " + Y.ToString("F3") + ", " + Z.ToString("F3");
}

double Pnt::Distance(Pnt otherPnt) {
    return std::sqrt(
        std::pow(otherPnt.X - X, 2) +
        std::pow(otherPnt.Y - Y, 2) +
        std::pow(otherPnt.Z - Z, 2)
    );
}

Pnt Pnt::Transformed(Trsf T) {
    return Pnt(gp_Pnt(X, Y, Z).Transformed(T));
}

/// <summary>
/// 返回修改了X值之后的新点
/// </summary>
/// <param name="value"></param>
/// <returns></returns>
Pnt Pnt::SetX(double value) {
    return Pnt(gp_Pnt(value, Y, Z));
}

/// <summary>
/// 返回修改了Y值之后的新点
/// </summary>
/// <param name="value"></param>
/// <returns></returns>
Pnt Pnt::SetY(double value) {
    return Pnt(gp_Pnt(X, value, Z));
}

/// <summary>
/// 返回修改了Z值之后的新点
/// </summary>
/// <param name="value"></param>
/// <returns></returns>
Pnt Pnt::SetZ(double value) {
    return Pnt(gp_Pnt(X, Y, value));
}

}
}
}