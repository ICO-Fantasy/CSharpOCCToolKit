#include "ICO_Poly_Triangulation.h"
#include<Poly_Triangulation.hxx>
//local
#include "ICO_BaseObject.h"

namespace OCCTK {
namespace OCC {
namespace Poly {

Handle(Poly_Triangulation) PTriangulation::myPT() { return Handle(Poly_Triangulation)::DownCast(NativeHandle); }

PTriangulation::PTriangulation(const Handle(Poly_Triangulation) polyTriangulation) : BaseObject(polyTriangulation) {
	NativeHandle = polyTriangulation;
}

}
}
}