#include "ICO_AIS_Axis.h"
#include <AIS_Axis.hxx>
//local
#include "ICO_Ax1.h"

using namespace OCCTK::OCC::gp;

namespace OCCTK {
namespace OCC {
namespace AIS {

AAxis::AAxis(gp::Ax1^ aAxis) {
	NativeHandle = new AIS_Axis(aAxis->GetOCC());
}

AAxis::AAxis(gp::Ax1^ aAxis, double len) {
	NativeHandle = new AIS_Axis(aAxis->GetOCC(), len);
}

void AAxis::SetWidth(double size) {
	myAxis()->SetWidth(size);
}

void AAxis::UnsetWidth() {
	myAxis()->UnsetWidth();
}

}
}
}