#include "ICO_Geom_Curve.h"

namespace OCCTK {
namespace OCC {
namespace Geom {
Handle(Geom_Curve) Curve::GetOCC() {
	return myCurve();
}
}
}
}