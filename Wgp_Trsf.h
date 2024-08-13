#pragma once
#include <gp_Trsf.hxx>

namespace OCCTK {
namespace OCC {
namespace gp {

public ref class Wgp_Trsf {
public:
	Wgp_Trsf();
	gp_Trsf* GetOCC();
private:
	gp_Trsf* _nativeHandle;
};

}
}
}

