#include "ICO_BRepExtrema_DistShapeShape.h"
#include <BRepExtrema_DistShapeShape.hxx>
//local
#include "ICO_Topo_Shape.h"
#include "ICO_Exception.h"

using namespace OCCTK::OCC::Topo;
using namespace System;

namespace OCCTK {
namespace OCC {
namespace BRepExtrema {

DistanceShapeShape::DistanceShapeShape() {
	DSS = new BRepExtrema_DistShapeShape();
}

DistanceShapeShape::DistanceShapeShape(TShape^ shape, TShape^ otherShape) {
	DSS = new BRepExtrema_DistShapeShape(shape->GetOCC(), otherShape->GetOCC());
}

/// <summary>
/// 返回两Shape间的最小距离
/// </summary>
/// <returns></returns>
double DistanceShapeShape::Value() {
	try {
		return DSS->Value();
	}
	CATCH_AND_THROW_OCC_EXCEPTIONS
}

}
}
}