#pragma once
#include "ICO_GProps.h"

namespace OCCTK {
namespace OCC {
namespace Topo {
ref class TShape;
}
namespace Gprop {
ref class GProps;
}
}
}


namespace OCCTK {
namespace OCC {
namespace BRepGProp {

public ref class BRepGlobalProperties {
private:
	BRepGlobalProperties() {}
public:
	static void SurfaceProperties(Topo::TShape^ shape, GProp::GProps^% globalProperties, bool skipShared, bool useTriangulation);
	//static void SurfaceProperties(Topo::TShape^ shape, GProp::GProps^% globalProperties, bool skipShared, bool useTriangulation) {
	//	::BRepGProp::SurfaceProperties(shape->GetOCC(), globalProperties->GetOCC(), skipShared, useTriangulation);
	//};
	static void SurfaceProperties(Topo::TShape^ shape, GProp::GProps^% globalProperties);
	//static void SurfaceProperties(Topo::TShape^ shape, GProp::GProps^% globalProperties) {
	//	::BRepGProp::SurfaceProperties(shape->GetOCC(), globalProperties->GetOCC());
	//};
	static void VolumeProperties(Topo::TShape^ shape, GProp::GProps^% globalProperties);
};

}
}
}