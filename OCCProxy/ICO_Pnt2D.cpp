#include "ICO_Pnt2D.h"
#include <cmath>
#include <gp_Pnt2d.hxx>
//local
#include "ICO_Trsf.h"

namespace OCCTK {
namespace OCC {
namespace gp {

Pnt2D::Pnt2D(double theX, double theY) {
    x = theX;
    y = theY;
}

Pnt2D::Pnt2D(gp_Pnt2d thePnt) {
    x = thePnt.X();
    y = thePnt.Y();
}

System::Object^ Pnt2D::Clone() {
    return Pnt2D(this->X, this->Y);;
}

gp_Pnt2d Pnt2D::GetOCC() {
    return gp_Pnt2d(X, Y);
}

System::String^ Pnt2D::ToString() {
    return X.ToString("F3") + ", " + Y.ToString("F3");
}

double Pnt2D::Distance(Pnt2D otherPnt) {
    return std::sqrt(
        std::pow(otherPnt.X - X, 2) +
        std::pow(otherPnt.Y - Y, 2)
    );
}

Pnt2D Pnt2D::Transformed(Trsf T) {
    return Pnt2D(gp_Pnt2d(X, Y).Transformed(T.GetOCC()));
}

/// <summary>
/// 返回修改了X值之后的新点
/// </summary>
/// <param name="value"></param>
/// <returns></returns>
Pnt2D Pnt2D::UpdateX(double value) {
    return Pnt2D(value, Y);
}

/// <summary>
/// 返回修改了Y值之后的新点
/// </summary>
/// <param name="value"></param>
/// <returns></returns>
Pnt2D Pnt2D::UpdateY(double value) {
    return Pnt2D(X, value);
}

}
}
}