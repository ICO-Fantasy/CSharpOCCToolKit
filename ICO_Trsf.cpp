#include "ICO_Trsf.h"
#include <gp_Ax3.hxx>

namespace OCCTK {
namespace OCC {
namespace gp {

Trsf::Trsf() {
	_nativeHandle = new gp_Trsf();
}

Trsf::Trsf(Ax2^ theAx2) {
	_nativeHandle = new gp_Trsf();
	_nativeHandle->SetTransformation(gp_Ax3(), gp_Ax3(theAx2->GetOCC()));
}
Trsf::Trsf(Ax2^ fromAx2, Ax2^ toAx2) {
	_nativeHandle = new gp_Trsf();
	_nativeHandle->SetTransformation(gp_Ax3(toAx2->GetOCC()), gp_Ax3(fromAx2->GetOCC()));
	//T.SetTransformation(gp_Ax3(fromAx2->GetOCC()), gp_Ax3(toAx2->GetOCC()));
}

Trsf::Trsf(Pnt^ fromPoint, Pnt^ toPoint) {
	_nativeHandle = new gp_Trsf();
	_nativeHandle->SetTranslation(fromPoint->GetOCC(), toPoint->GetOCC());
}
void Trsf::SetTranslation(Pnt^ fromPoint, Pnt^ toPoint) {
	_nativeHandle->SetTranslation(fromPoint->GetOCC(), toPoint->GetOCC());
}

gp_Trsf Trsf::GetOCC() {
	return *_nativeHandle;
}

}
}
}