#pragma once
#include <GeomAbs_CurveType.hxx>

namespace OCCTK {
namespace OCC {
namespace GeomAbs {

public enum class CurveType {
	Line = GeomAbs_Line,
	Circle = GeomAbs_Circle,
	Ellipse = GeomAbs_Ellipse,
	Hyperbola = GeomAbs_Hyperbola,
	Parabola = GeomAbs_Parabola,
	BezierCurve = GeomAbs_BezierCurve,
	BSplineCurve = GeomAbs_BSplineCurve,
	OffsetCurve = GeomAbs_OffsetCurve,
	OtherCurve = GeomAbs_OtherCurve
};

}
}
}