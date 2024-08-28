#pragma once
#include <Bnd_Box.hxx>
#include <BRepBndLib.hxx>
//包装C++类到托管类
#include <NCollection_Haft.h> 
//Local
#include "ICO_Pnt.h"
#include "ICO_TopoDS_Shape.h"
namespace OCCTK {
namespace Extension {

public ref class BoundingBox {
public:
	BoundingBox();
	BoundingBox(OCC::TopoDS::TShape^ TopoShape);
	void Add(OCC::TopoDS::TShape^ TopoShape);
	OCC::gp::Pnt^ GetLowerLeftCorner();
private:
	Bnd_Box* myBox;
	//NCollection_Haft<Handle(Bnd_Box)> myBox;
};

}
}

