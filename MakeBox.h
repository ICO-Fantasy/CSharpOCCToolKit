#pragma once
#include<TopoDS_Shape.hxx>
#include<BRepPrimAPI_MakeBox.hxx>
#include <Standard_Handle.hxx>
//包装C++类到托管类
#include <NCollection_Haft.h> 
//namespace OCCTK::OCC::BRepPrimAPI {
//	ref class MakeBox
//	{
//	public:
//		 MakeBox();
//		 MakeBox(const double dx, const double dy, const double dz);
//		 Handle(TopoDS_Shape) Shape();
//	private:
//		NCollection_Haft<Handle(BRepPrimAPI_MakeBox)> myOCCClass;
//		NCollection_Haft<Handle(TopoDS_Shape)> myShape;
//	};
//}