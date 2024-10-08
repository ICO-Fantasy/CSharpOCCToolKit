#pragma once
class gp_Vec;

namespace OCCTK {
namespace OCC {
namespace gp {
ref class Pnt;
public ref class Vec :System::ICloneable {
public:
	Vec();
	Vec(double theX, double theY, double theZ);
	Vec(Pnt^ fromPnt, Pnt^ toPnt);
	Vec(gp_Vec theVec);
	Vec(gp_Vec* theVec);
	gp_Vec GetOCC();
	virtual System::Object^ Clone();
	virtual System::String^ ToString() override;
public:
	bool IsParallel(Vec^ otherVec, double theAngularTolerance);
	void Normalize();
	Vec^ Normalized();
	void Multiply(double value);
	Vec^ Multiplied(double value);
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