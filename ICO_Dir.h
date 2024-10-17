#pragma once
#include <gp_Dir.hxx>

namespace OCCTK {
namespace OCC {
namespace gp {
ref class Vec;
}
}
}

namespace OCCTK {
namespace OCC {
namespace gp {

public ref class Dir :System::ICloneable {
public:
	Dir();
	Dir(double theX, double theY, double theZ);
	Dir(Vec^ theDir);
	Dir(gp_Dir theDir);
	Dir(gp_Dir* theDir);
	gp_Dir GetOCC();
	virtual System::Object^ Clone();
	virtual System::String^ ToString() override;
public:
	bool IsParallel(Dir^ otherDir, double theAngularTolerance);
	double Angle(Dir^ otherDir);
	double AngleWithRef(Dir^ otherDir, Dir^ refDir);
	void Cross(Dir^ other);
	Dir^ Crossed(Dir^ other);
	double Dot(Dir^ other);
public:
	property double X;
	property double Y;
	property double Z;
private:
	void Normalize();
};

}
}
}