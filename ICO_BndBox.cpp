#include "ICO_BndBox.h"

namespace OCCTK {
namespace Extension {

BoundingBox::BoundingBox() {
	myBox = new Bnd_Box();
}

BoundingBox::BoundingBox(OCC::TopoDS::TShape^ TopoShape) {
	myBox = new Bnd_Box();
	BRepBndLib::Add(TopoShape->GetOCC(), *myBox);
}

void BoundingBox::Add(OCC::TopoDS::TShape^ TopoShape) {
	BRepBndLib::Add(TopoShape->GetOCC(), *myBox);
}

OCC::gp::Pnt^ BoundingBox::GetLowerLeftCorner() {
	double xMin, yMin, zMin, xMax, yMax, zMax;
	myBox->Get(xMin, yMin, zMin, xMax, yMax, zMax);
	return gcnew Pnt(xMin, yMin, zMin);
}

}
}