#pragma once
#include "ICO_Dir.h"
namespace OCCTK {
namespace OCC {
namespace gp {
ref class Dir;
}
}
}

namespace OCCTK {
namespace Tool {

public ref class DirMaker {
public:
	static OCC::gp::Dir^ AngleWithZ(double degree);
	static OCC::gp::Dir^ AngleWithZ(double value, bool radian);
	static double GetDirAngleWithZ(OCC::gp::Dir^ theDir);
	static double GetDirAngleWithZ(OCC::gp::Dir^ theDir, bool radian);
};

}
}