#pragma once
#include <gp_Dir.hxx>

namespace OCCTK {
namespace OCC {
namespace gp {
ref class Vec;
ref class Pnt;
ref class Trsf;
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
	Dir(Pnt^ fromPoint, Pnt^ toPoint);
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
	void Reverse();
	Dir^ Reversed();
	void Transform(Trsf^ T);
	Dir^ Transformed(Trsf^ T);
public:
	Vec^ ToVec(double factor);
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