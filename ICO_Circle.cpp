#include "ICO_Circle.h"
#include <gp_Circ.hxx>
//local
#include "ICO_Ax2.h"

namespace OCCTK {
namespace OCC {
namespace gp {

Circle::Circle(Ax2^ pose, double radius) {
	this->pose = pose;
	Radius = radius;
}

Circle::Circle(gp_Circ Circle) {
	pose = gcnew Ax2(Circle.Position());
	Radius = Circle.Radius();
}

Circle::Circle(gp_Circ* Circle) {
	pose = gcnew Ax2(Circle->Position());
	Radius = Circle->Radius();
}

gp_Circ Circle::GetOCC() {
	return gp_Circ(pose->GetOCC(), Radius);
}

System::Object^ Circle::Clone() {
	return gcnew Circle(pose, Radius);
}

Pnt^ Circle::Location() {
	return pose->Location;
}


}
}
}