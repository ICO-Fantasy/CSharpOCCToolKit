#pragma once

//前向声明
class gp_Trsf;
namespace OCCTK {
namespace OCC {
namespace gp {
ref class Ax1;
ref class Pnt;
ref class Ax2;
ref class Quat;
ref class Vec;
}
}
}

namespace OCCTK {
namespace OCC {
namespace gp {

public ref class Trsf :System::ICloneable {
public:
	Trsf();
	Trsf(gp_Trsf theT);
	Trsf(gp_Trsf* theT);
	Trsf(Ax2^ theAx2);
	Trsf(array<double, 2>^ matrix);
	Trsf(array<array<double>^>^ matrix);
	Trsf(Ax2^ fromAx2, Ax2^ toAx2);
	Trsf(Pnt^ fromPoint, Pnt^ toPoint);
	gp_Trsf GetOCC();
	virtual System::Object^ Clone();
	virtual System::String^ ToString() override;
public:
	void SetTranslation(Pnt^ fromPoint, Pnt^ toPoint);
	void SetTranslation(Vec^ vec);
	void SetTranslationPart(Vec^ vec);
	void SetRotationPart(Quat^ quat);
	void SetRotation(Ax1^ axis, double angle);
	void PreMultiply(Trsf^ leftTrsf);
	Trsf^ Multiplied(Trsf^ rightTrsf);
	Trsf^ Inverted();
public:
	property System::ValueTuple<double, double, double> Translation {System::ValueTuple<double, double, double> get(); };
	property Quat^ Rotation {Quat^ get(); };
private:
	gp_Trsf* myTrsf;
protected:
	// 析构函数用于清理非托管资源
	!Trsf() {
		if (myTrsf != 0) {
			delete myTrsf;
			myTrsf = 0;
		}
	}

	// 终结器（finalizer）用于垃圾回收时的清理
	~Trsf() {
		// 调用析构函数来清理非托管资源
		this->!Trsf();
	}
};

}
}
}

