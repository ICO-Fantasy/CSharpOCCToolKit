#pragma once

class gp_Pnt;

namespace OCCTK {
namespace OCC {
namespace gp {

ref class Trsf;

public ref class Pnt :System::ICloneable {
public:
	Pnt();
	Pnt(double theX, double theY, double theZ);
	Pnt(gp_Pnt thePnt);
	Pnt(gp_Pnt* thePnt);
	virtual System::Object^ Clone();
	virtual System::String^ ToString() override;
	gp_Pnt GetOCC();
public:
	double Distance(Pnt^ otherPnt);
	Pnt^ Transformed(Trsf^ T);
public:
	property double X;
	property double Y;
	property double Z;
};

}
}
}