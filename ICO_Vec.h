#pragma once
class gp_Vec;

namespace OCCTK {
namespace OCC {
namespace gp {

public ref class Vec :System::ICloneable {
public:
	Vec();
	Vec(double theX, double theY, double theZ);
	Vec(gp_Vec theVec);
	Vec(gp_Vec* theVec);
	gp_Vec GetOCC();
	virtual System::Object^ Clone();
	virtual System::String^ ToString() override;
public:
	bool IsParallel(Vec^ otherVec, double theAngularTolerance);
	void Cross(Vec^ other);
	Vec^ Crossed(Vec^ other);
public:
	property double X;
	property double Y;
	property double Z;
};

}
}
}