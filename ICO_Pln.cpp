#include "ICO_Pln.h"
#include "ICO_Trsf.h"
#include "ICO_Ax3.h"
#include "ICO_Dir.h"
#include "ICO_pnt.h"

namespace OCCTK {
namespace OCC {
namespace gp {

Pln::Pln() {
	myPln = new gp_Pln();
}

Pln::Pln(Ax3^ axis) {
	myPln = new gp_Pln(axis->GetOCC());
}

Pln::Pln(Pnt^ location, Dir^ direction) {
	myPln = new gp_Pln(location->GetOCC(), direction->GetOCC());
}

gp_Pln Pln::GetOCC() {
	return *myPln;
}

}
}
}