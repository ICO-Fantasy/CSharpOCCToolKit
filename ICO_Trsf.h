#pragma once
#include <gp_Trsf.hxx>
//Local
#include "ICO_Pnt.h"

//前向声明
namespace OCCTK {
namespace OCC {
namespace gp {
ref class Ax2;
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
	Trsf(Ax2^ fromAx2, Ax2^ toAx2);
	Trsf(Pnt^ fromPoint, Pnt^ toPoint);
	gp_Trsf GetOCC();
	virtual System::Object^ Clone();
	virtual System::String^ ToString() override;
	void SetTranslation(Pnt^ fromPoint, Pnt^ toPoint);
	void PreMultiply(Trsf^ leftTrsf);
	Trsf^ Multiplied(Trsf^ rightTrsf);
	Trsf^ Inverted();

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

