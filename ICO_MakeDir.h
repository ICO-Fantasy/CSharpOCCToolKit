#pragma once
#include "ICO_Dir.h"

using namespace OCCTK::OCC::gp;

namespace OCCTK {
namespace Tool {

public ref class DirMaker {
public:
	static Dir^ AngleWithZ(double degree);
	static Dir^ AngleWithZ(double value, bool radian);
	static double GetDirAngleWithZ(Dir^ theDir);
	static double GetDirAngleWithZ(Dir^ theDir, bool radian);
};

}
}