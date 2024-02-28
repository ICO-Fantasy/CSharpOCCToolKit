#pragma once
#include <Standard_Handle.hxx>
//��װC++�ൽ�й���
#include <NCollection_Haft.h> 

namespace OCCTK::OCC::TopoDS {
ref class Shape
{
public:
	Shape();
	Shape(TopoDS_Shape aShape);
	Handle(TopoDS_Shape) GetOCC();
private:
	NCollection_Haft<Handle(TopoDS_Shape)> myOCCClass;
};
}