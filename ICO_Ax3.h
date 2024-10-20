﻿#pragma once
#include <gp_Ax3.hxx>

//前向声明
class gp_Cylinder;
namespace OCCTK {
namespace OCC {
namespace gp {
ref class Trsf;
ref class Ax1;
ref class Pnt;
ref class Dir;
}
}
}

namespace OCCTK {
namespace OCC {
namespace gp {

public ref class Ax3 :System::ICloneable {
public:
	Ax3();
	Ax3(gp_Ax3 theAx3);
	Ax3(gp_Ax3* theAx3);
	gp_Ax3 GetOCC();
	virtual System::Object^ Clone();
	virtual System::String^ ToString() override;
public:
	void Transform(Trsf^ theT);
	Ax3^ Transformed(Trsf^ theT);
	Ax1^ Axis();

protected:
	gp_Ax3* myAx3;
protected:
	// 析构函数用于清理非托管资源
	!Ax3() {
		if (myAx3 != 0) {
			delete myAx3;
			myAx3 = 0;
		}
	}

	// 终结器（finalizer）用于垃圾回收时的清理
	~Ax3() {
		// 调用析构函数来清理非托管资源
		this->!Ax3();
	}
};

}
}
}