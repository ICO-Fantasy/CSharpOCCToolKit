#pragma once
#include <Standard_Handle.hxx>
#include <AIS_ViewCube.hxx>
#include <vcclr.h>
//包装C++类到托管类
//#include <NCollection_Haft.h> 
namespace OCCTK::Visualization {
ref class ViewCube
{
public:
	ViewCube(float axisSize);
	Handle(AIS_ViewCube)* GetOCC();
private:
	Handle(AIS_ViewCube)* myViewCube;
	//gcroot<AIS_ViewCube*> myViewCube;
	//NCollection_Haft<Handle(AIS_ViewCube)> myViewCube;
	Handle(Standard_Transient)* _ptransient;
};
}

