#pragma once
#include <gp_Dir.hxx>


namespace OCCTK {
namespace OCC {
namespace gp {
public ref class Wgp_Dir {
public:
	Wgp_Dir();
	Wgp_Dir(double X, double Y, double Z);
	Wgp_Dir(gp_Dir theDir);
	Wgp_Dir(gp_Dir* theDir);
	bool IsParallel(Wgp_Dir^ otherDir, double theAngularTolerance);
	gp_Dir* GetOCC();
	double X();
	double Y();
	double Z();
protected:
	gp_Dir* myDir;
};

}
}
}