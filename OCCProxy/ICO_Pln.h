#pragma once
#include "ICO_Ax2.h"
#include "ICO_Ax3.h"
#include "ICO_Dir.h"
#include "ICO_Pnt.h"
#include <gp_Pln.hxx>

namespace OCCTK {
namespace OCC {
namespace gp {
//前向声明
value struct Trsf;
value struct Ax1;
public value struct Pln :System::ICloneable {
public:
    const static Dir Default = Dir(1.0, 0.0, 0.0);
public:
    Pln(gp_Pln pln);
    Pln(Ax2 pose);
    Pln(Ax3 pose);
    Pln(Pnt location, Dir direction);
    gp_Pln GetOCC();
    virtual System::Object^ Clone();
public:
    double Distance(Pnt point);
    System::ValueTuple<double, double, double, double> Coefficients();
private:
    Ax2 position;
public:
    property Ax2 Position {Ax2 get() { return position; }};
    property Dir Normal {Dir get() { return Position.ZDir; }};
};

}
}
}

