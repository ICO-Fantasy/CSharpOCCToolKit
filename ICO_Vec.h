#pragma once
#include <cmath>

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
#pragma region  重载操作符
	// 重载 + 操作符
	static Vec^ operator+(Vec^ a, Vec^ b) {
		// 创建一个新的 Pnt 对象，表示相加的结果
		return gcnew Vec(a->X + b->X, a->Y + b->Y, a->Z + b->Z);
	}
	// 重载 + 操作符
	static Vec^ operator-(Vec^ a, Vec^ b) {
		// 创建一个新的 Pnt 对象，表示相加的结果
		return gcnew Vec(a->X - b->X, a->Y - b->Y, a->Z - b->Z);
	}
#pragma endregion

public:
	bool IsParallel(Vec^ otherVec, double theAngularTolerance);
	void Normalize();
	Vec^ Normalized();
	void Multiply(double value);
	Vec^ Multiplied(double value);
	double Dot(Vec^ other);
	void Cross(Vec^ other);
	Vec^ Crossed(Vec^ other);
	Vec^ CrossProduct(Vec^ other);
public:
	property double X;
	property double Y;
	property double Z;
	property double Length {double get() { return std::sqrt(X * X + Y * Y + Z * Z); }};
	//property double Z {double get() { return myBP().Z; }};
};

}
}
}