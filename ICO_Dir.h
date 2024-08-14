#pragma once
#include <gp_Dir.hxx>


namespace OCCTK {
namespace OCC {
namespace gp {

public ref class Dir {
public:
	Dir();
	Dir(double X, double Y, double Z);
	Dir(gp_Dir theDir);
	Dir(gp_Dir* theDir);
	bool IsParallel(Dir^ otherDir, double theAngularTolerance);
	gp_Dir GetOCC();
	double X();
	double Y();
	double Z();
protected:
	gp_Dir* myDir;
};

}
}
}