#pragma once

class gp_Pnt;
class gp_XYZ;

namespace OCCTK {
namespace OCC {
namespace gp {

ref class Trsf;

public ref class Pnt :System::ICloneable {
public:
	Pnt();
	Pnt(double theX, double theY, double theZ);
	Pnt(gp_Pnt thePnt);
	Pnt(gp_XYZ theXYZ);
	Pnt(gp_Pnt* thePnt);
	gp_Pnt GetOCC();
	virtual System::Object^ Clone();
	virtual System::String^ ToString() override;
#pragma region 重载操作符
	Pnt^ Add(Pnt^ other);
	// 重载 + 操作符
	static Pnt^ operator+(Pnt^ a, Pnt^ b) {
		// 创建一个新的 Pnt 对象，表示相加的结果
		return gcnew Pnt(a->X + b->X, a->Y + b->Y, a->Z + b->Z);
	}
	Pnt^ Minus(Pnt^ other);
	// 重载 - 操作符
	static Pnt^ operator-(Pnt^ a, Pnt^ b) {
		// 创建一个新的 Pnt 对象，表示相加的结果
		return gcnew Pnt(a->X - b->X, a->Y - b->Y, a->Z - b->Z);
	}
#pragma endregion
public:
	double Distance(Pnt^ otherPnt);
	Pnt^ Transformed(Trsf^ T);
	Pnt^ SetX(double value);
	Pnt^ SetY(double value);
	Pnt^ SetZ(double value);
public:
	property double X;
	property double Y;
	property double Z;
};

}
}
}