#include "ICO_XShape.h"
//local
#include "..\OCCProxy\ICO_Trsf.h"
#include "..\OCCProxy\ICO_AIS_Shape.h"
#include "..\OCCProxy\ICO_Topo_Shape.h"
#include "ICO_Color.h"

using namespace OCCTK::OCC::gp;

namespace OCCTK {
namespace Extension {
XShape::XShape() {
	{
		IsShape = false;
		Name = nullptr;
		Color = System::Nullable<Extension::Color>();
		Transparence = -1.0;
		Parent = nullptr;
		Children = gcnew System::Collections::Generic::List<XShape^>();
		Layer = -1;
		Transform = gcnew OCC::gp::Trsf();
		TopoShape = nullptr;
		Material = nullptr;
	}
}
XShape::XShape(bool isShape) {
	IsShape = isShape;
	Name = nullptr;
	Color = System::Nullable<Extension::Color>();
	Transparence = -1.0;
	Parent = nullptr;
	Children = gcnew System::Collections::Generic::List<XShape^>();
	Layer = -1;
	Transform = gcnew OCC::gp::Trsf();
	TopoShape = nullptr;
	Material = nullptr;
}
Trsf^ XShape::Location::get() {
	if (Parent != nullptr) {
		return  Parent->Location->Multiplied(Transform);
	}
	else {
		return Transform;
	}
}

void XShape::TopoShape::set(OCC::Topo::TShape^ value) {
	if (value == nullptr) {
		return;
	}
	myTopo = value;
	if (myAIS != nullptr) {
		myAIS->RemoveSelf(false);
	}
	myAIS = gcnew OCC::AIS::AShape(value);
}

OCC::AIS::AShape^ XShape::AISShape::get() {
	if (myAIS == nullptr) {
		myAIS = gcnew OCC::AIS::AShape(TopoShape);
	};
	return myAIS;
}

}
}