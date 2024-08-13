#include "Wgp_Trsf.h"

namespace OCCTK {
namespace OCC {
namespace gp {

Wgp_Trsf::Wgp_Trsf() {
	_nativeHandle = new gp_Trsf();
}

gp_Trsf* Wgp_Trsf::GetOCC() {
	return _nativeHandle;
}

}
}
}