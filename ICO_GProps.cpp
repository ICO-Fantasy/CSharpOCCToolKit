#include "ICO_GProps.h"
#include <GProp_GProps.hxx>
#include <BRepGProp.hxx>
//local
#include "ICO_Pnt.h"

using namespace OCCTK::OCC::gp;

namespace OCCTK {
namespace OCC {
namespace GProp {

GProps::GProps() {
	myGProps = new GProp_GProps();
}

GProps::GProps(GProp_GProps g) {
	myGProps = new GProp_GProps(g);
}

GProps::GProps(GProp_GProps* intptr) {
	myGProps = intptr;
}

GProp_GProps GProps::GetOCC() {
	return *myGProps;
}

double GProps::Mass() {
	return myGProps->Mass();
}

Pnt^ GProps::CentreOfMass() {
	return gcnew Pnt(myGProps->CentreOfMass());
}

}
}
}