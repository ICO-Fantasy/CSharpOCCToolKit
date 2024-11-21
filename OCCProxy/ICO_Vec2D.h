#pragma once
class gp_Vec2d;

namespace OCCTK {
namespace OCC {
namespace gp {
ref class Pnt2D;
public ref class Vec2D :System::ICloneable {
public:
	Vec2D();
	Vec2D(double theX, double theY);
	Vec2D(Pnt2D^ fromPnt, Pnt2D^ toPnt);
	Vec2D(gp_Vec2d theVec);
	Vec2D(gp_Vec2d* theVec);
	gp_Vec2d GetOCC();
	virtual System::Object^ Clone();
	virtual System::String^ ToString() override;
public:
	bool IsParallel(Vec2D^ otherVec, double theAngularTolerance);
	void Normalize();
	Vec2D^ Normalized();
	void Multiply(double value);
	Vec2D^ Multiplied(double value);
	double Crossed(Vec2D^ other);
public:
	property double X;
	property double Y;
};

}
}
}