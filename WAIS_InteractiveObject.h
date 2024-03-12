#pragma once
#include <AIS_InteractiveObject.hxx>
#include <Standard_Handle.hxx>
//using namespace OCCTK::OCC;
namespace OCCTK::OCC::AIS {
ref class InteractiveObject
{
public:
	InteractiveObject(Handle(Standard_Transient)* nativeHandle);
	bool IsNull();
	virtual Handle(Standard_Transient)* GetOCC();
	//Wgp_Trsf^ GetOCC();
	System::String^ ObjectID;
	int ObjectType; // 0 for Point, 1 for Line, 2 for Circle, 3 for Spline
protected:
	Handle(Standard_Transient)* nativeHandle;
};
}

