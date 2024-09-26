#include "ICO_Geom_Curve.h"
#include <Geom_Curve.hxx>

namespace OCCTK {
namespace OCC {
namespace Geom {
Handle(Geom_Curve) Curve::GetOCC() {
	return myCurve();
}
}
}
}