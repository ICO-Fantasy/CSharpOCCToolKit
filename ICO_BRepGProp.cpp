#include "ICO_BRepGProp.h"
#include <BRepGProp.hxx>
#include <GProp_GProps.hxx>
//local
#include "ICO_Topo_Shape.h"
#include "ICO_GProps.h"


namespace OCCTK {
namespace OCC {
namespace BRepGProp {

void BRepGProp::BRepGlobalProperties::SurfaceProperties(Topo::TShape^ shape, GProp::GProps^% globalProperties, bool skipShared, bool useTriangulation) {
	GProp_GProps temp = GProp_GProps();
	::BRepGProp::SurfaceProperties(shape->GetOCC(), temp, skipShared, useTriangulation);
	globalProperties = gcnew GProp::GProps(temp);

}
void BRepGlobalProperties::SurfaceProperties(Topo::TShape^ shape, GProp::GProps^% globalProperties) {
	GProp_GProps temp = GProp_GProps();
	::BRepGProp::SurfaceProperties(shape->GetOCC(), temp);
	globalProperties = gcnew GProp::GProps(temp);
}

void BRepGlobalProperties::VolumeProperties(Topo::TShape^ shape, GProp::GProps^% globalProperties) {
	GProp_GProps temp = GProp_GProps();
	::BRepGProp::VolumeProperties(shape->GetOCC(), temp);
	globalProperties = gcnew GProp::GProps(temp);
}

}
}
}