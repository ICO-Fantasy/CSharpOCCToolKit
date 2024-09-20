#include "ICO_BndBox.h"
//local
#include "ICO_Pnt.h"
#include "ICO_Topo_Shape.h"

using namespace OCCTK::OCC::Topo;
using namespace OCCTK::OCC::gp;

namespace OCCTK {
namespace Extension {

BoundingBox::BoundingBox() {
	myBox = new Bnd_Box();
}

BoundingBox::BoundingBox(TShape^ TopoShape) {
	myBox = new Bnd_Box();
	myShape = new TopoDS_Shape(TopoShape->GetOCC());
	BRepBndLib::Add(TopoShape->GetOCC(), *myBox);
}

void BoundingBox::Add(TShape^ TopoShape) {
	myShape = new TopoDS_Shape(TopoShape->GetOCC());
	BRepBndLib::Add(TopoShape->GetOCC(), *myBox);
}

OCC::gp::Pnt^ BoundingBox::GetLowerLeftCorner() {
	if (myShape == nullptr) {
		throw gcnew System::Exception(gcnew System::String("Non Shape Added"));
	}
	double xMin, yMin, zMin, xMax, yMax, zMax;
	myBox->Get(xMin, yMin, zMin, xMax, yMax, zMax);
	return gcnew Pnt(xMin, yMin, zMin);
}
/// <summary>
/// 获取 xMin, yMin, zMin, xMax, yMax, zMax六个值
/// </summary>
/// <returns></returns>
array<double>^ BoundingBox::GetBndBox() {
	if (myShape == nullptr) {
		throw gcnew System::Exception(gcnew System::String("Non Shape Added"));
	}
	double xMin, yMin, zMin, xMax, yMax, zMax;
	myBox->Get(xMin, yMin, zMin, xMax, yMax, zMax);
	return gcnew array<double>(6) { xMin, yMin, zMin, xMax, yMax, zMax };
}

}
}