#include "ICO_BRepGProp_Face.h"
#include <BRepGProp_Face.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
//local
#include "ICO_Topo_Face.h"
#include "ICO_Pnt.h"
#include "ICO_Vec.h"

using namespace OCCTK::OCC::Topo;
using namespace OCCTK::OCC::gp;

namespace OCCTK {
namespace OCC {
namespace BRepGProp {

BRepGlobalProperties_Face::BRepGlobalProperties_Face(Topo::TFace^ face) {
	myGPFace = new BRepGProp_Face(face->GetOCC());
}

Vec BRepGlobalProperties_Face::Normal(double U, double V) {
	gp_Pnt theP = gp_Pnt();
	gp_Vec theV = gp_Vec();
	myGPFace->Normal(U, V, theP, theV);
	return Vec(theV);
}
}
}
}