#pragma once
#include <AIS_ViewCube.hxx>
#include <Standard_Handle.hxx>
//包装C++类到托管类
//#include <NCollection_Haft.h> 
namespace OCCTK::OCC::AIS {
ref class ViewCube
{
public:
	ViewCube(float axisSize);
	Handle(AIS_ViewCube)* GetOCC();
private:
	Handle(AIS_ViewCube)* myViewCube;
	Handle(Standard_Transient)* _ptransient;
};
}

