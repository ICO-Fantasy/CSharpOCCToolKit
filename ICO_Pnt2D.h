#pragma once

class gp_Pnt2d;

namespace OCCTK {
namespace OCC {
namespace gp {

ref class Trsf;

public ref class Pnt2D :System::ICloneable {
public:
	Pnt2D();
	Pnt2D(double theX, double theY);
	Pnt2D(gp_Pnt2d thePnt);
	Pnt2D(gp_Pnt2d* thePnt);
	virtual System::Object^ Clone();
	virtual System::String^ ToString() override;
	gp_Pnt2d GetOCC();
public:
	double Distance(Pnt2D^ otherPnt);
	Pnt2D^ Transformed(Trsf^ T);
	Pnt2D^ SetX(double value);
	Pnt2D^ SetY(double value);
public:
	property double X;
	property double Y;
};

}
}
}