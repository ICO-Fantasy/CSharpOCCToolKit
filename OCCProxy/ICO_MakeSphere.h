#pragma once
#include <BRepPrimAPI_MakeSphere.hxx>

namespace OCCTK {
namespace OCC {
namespace gp {
value struct Pnt;
ref class Ax2;
}
namespace Topo {
ref class TShape;
}
}
}

namespace OCCTK {
namespace OCC {
namespace BRepPrimAPI {

public ref class MakeSphere {
public:
	MakeSphere(double X, double Y, double Z, double R);
	MakeSphere(gp::Pnt originPnt, double R);
	MakeSphere(gp::Ax2^ theAxis, double R);
	Topo::TShape^ Shape();
	//! 隐式转换为 TShape
	static operator Topo::TShape ^ (MakeSphere^ ms) {
		return ms->Shape();  // 隐式调用 Shape 方法
	}
private:
	BRepPrimAPI_MakeSphere* myMaker;
protected:
	// 析构函数用于清理非托管资源
	!MakeSphere() {
		if (myMaker != 0) {
			delete myMaker;
			myMaker = 0;
		}
	}

	// 终结器（finalizer）用于垃圾回收时的清理
	~MakeSphere() {
		// 调用析构函数来清理非托管资源
		this->!MakeSphere();
	}
};

}
}
}