#include "ICO_Cylinder.h"
#include <gp_Cylinder.hxx>
//local
#include "ICO_Ax1.h"
#include "ICO_Ax3.h"

namespace OCCTK {
namespace OCC {
namespace gp {

Cylinder::Cylinder(Ax3^ pose, double radius) {
	this->pose = pose;
	this->radius = radius;
}
Cylinder::Cylinder(gp_Cylinder cylinder) {
	pose = gcnew Ax3(cylinder.Position());
	radius = cylinder.Radius();
}

Cylinder::Cylinder(gp_Cylinder* cylinder) {
	pose = gcnew Ax3(cylinder->Position());
	radius = cylinder->Radius();
}

gp_Cylinder Cylinder::GetOCC() {
	return gp_Cylinder(pose->GetOCC(), radius);
}

System::Object^ Cylinder::Clone() {
	return gcnew Cylinder(pose, radius);
}

Ax1^ Cylinder::Axis() {
	return pose->Axis();
}

double Cylinder::Radius() {
	return radius;
}


}
}
}