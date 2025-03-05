#include "ICO_Pln.h"
#include <gp_Pln.hxx>
//local
#include "ICO_Trsf.h"
#include "ICO_Ax1.h"
#include "ICO_Ax2.h"
#include "ICO_Ax3.h"
#include "ICO_Dir.h"
#include "ICO_pnt.h"

using namespace System;

namespace OCCTK {
namespace OCC {
namespace gp {

Pln::Pln(gp_Pln pln) {
    position = Ax2(pln.Position());
}

Pln::Pln(Ax2 pose) {
    position = pose;
}

Pln::Pln(Ax3 pose) {
    position = Ax2(pose);
}

Pln::Pln(Pnt location, Dir direction) {
    position = Ax2(location, direction);
}

gp_Pln Pln::GetOCC() {
    return gp_Pln(Ax3(position));
}

Object^ Pln::Clone() {
    return Pln(GetOCC());
}

double Pln::Distance(Pnt point) {
    return GetOCC().Distance(point);
}

System::ValueTuple<double, double, double, double> Pln::Coefficients() {
    //theA * X + theB * Y + theC * Z + theD = 0.
    double a, b, c, d;
    GetOCC().Coefficients(a, b, c, d);
    return System::ValueTuple<double, double, double, double>(a, b, c, d);
}

}
}
}