#include "ICO_MakeDir.h"
#include <gp_Dir.hxx>
#include <gp_Trsf.hxx>
#include <gp_Quaternion.hxx>
namespace OCCTK {
namespace Tool {

Dir^ DirMaker::AngleWithZ(double degree) {
	double angle = degree * M_PI / 180.0;
	gp_Trsf T = gp_Trsf();
	gp_Quaternion R = gp_Quaternion(gp_Vec(0, 0, 1), angle);
	T.SetRotation(R);
	gp_Dir theDir = gp_Dir(1, 0, 0);
	theDir.Transform(T);
	return gcnew Dir(theDir);
}
Dir^ DirMaker::AngleWithZ(double value, bool radian) {
	double angle;
	if (radian) {
		angle = value * M_PI / 180.0;
	}
	else {
		angle = value;
	}
	gp_Trsf T = gp_Trsf();
	gp_Quaternion R = gp_Quaternion(gp_Vec(0, 0, 1), angle);
	T.SetRotation(R);
	gp_Dir theDir = gp_Dir(1, 0, 0);
	theDir.Transform(T);
	return gcnew Dir(theDir);
}

double DirMaker::GetDirAngleWithZ(Dir^ theDir) {
	return theDir->GetOCC().AngleWithRef(gp_Dir(1, 0, 0), gp_Dir(0, 0, -1)) * 180.0 / M_PI;
}
double DirMaker::GetDirAngleWithZ(Dir^ theDir, bool radian) {
	if (radian) {
		return theDir->GetOCC().AngleWithRef(gp_Dir(1, 0, 0), gp_Dir(0, 0, -1));
	}
	else {
		return theDir->GetOCC().AngleWithRef(gp_Dir(1, 0, 0), gp_Dir(0, 0, -1)) * 180.0 / M_PI;
	}
}

}
}