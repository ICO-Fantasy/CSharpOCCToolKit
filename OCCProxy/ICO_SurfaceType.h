#pragma once
#include <GeomAbs_SurfaceType.hxx>

namespace OCCTK {
namespace OCC {
namespace GeomAbs {

public enum class SurfaceType {
	Plane = GeomAbs_Plane,
	Cylinder = GeomAbs_Cylinder,
	Cone = GeomAbs_Cone,
	Sphere = GeomAbs_Sphere,
	Torus = GeomAbs_Torus,
	BezierSurface = GeomAbs_BezierSurface,
	BSplineSurface = GeomAbs_BSplineSurface,
	SurfaceOfRevolution = GeomAbs_SurfaceOfRevolution,
	SurfaceOfExtrusion = GeomAbs_SurfaceOfExtrusion,
	OffsetSurface = GeomAbs_OffsetSurface,
	OtherSurface = GeomAbs_OtherSurface
};

}
}
}