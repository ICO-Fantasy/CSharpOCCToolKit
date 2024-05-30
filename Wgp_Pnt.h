#pragma once
#include <gp_Pnt.hxx>

namespace OCCTK::OCC::gp
{
public ref class Wgp_Pnt {
public:
	Wgp_Pnt();
	Wgp_Pnt(double X, double Y, double Z);
	Wgp_Pnt(gp_Pnt thePnt);
	Wgp_Pnt(gp_Pnt* thePnt);
	gp_Pnt* GetOCC();
	double X();
	double Y();
	double Z();
private:
	gp_Pnt* _nativeHandle;
};
}


