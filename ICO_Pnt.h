#pragma once
#include <gp_Pnt.hxx>

namespace OCCTK {
namespace OCC {
namespace gp {

public ref class Pnt {
public:
	Pnt();
	Pnt(double X, double Y, double Z);
	Pnt(gp_Pnt thePnt);
	Pnt(gp_Pnt* thePnt);
	gp_Pnt GetOCC();
	double X();
	double Y();
	double Z();
private:
	gp_Pnt* _nativeHandle;
};

}
}
}