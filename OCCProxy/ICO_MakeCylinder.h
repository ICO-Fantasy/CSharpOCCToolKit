#pragma once
#include <BRepPrimAPI_MakeCylinder.hxx>

namespace OCCTK {
namespace OCC {
namespace gp {
ref class Pnt;
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

public ref class MakeCylinder {
public:
	MakeCylinder(double r, double h);
	MakeCylinder(double r, double h, double angle);
	MakeCylinder(gp::Ax2^ theAxis, double r, double h);
	MakeCylinder(gp::Ax2^ theAxis, double r, double h, double angle);
	Topo::TShape^ Shape();
	//! 隐式转换为 TShape
	static operator Topo::TShape ^ (MakeCylinder^ ms) {
		return ms->Shape();  // 隐式调用 Shape 方法
	}
private:
	BRepPrimAPI_MakeCylinder* myMaker;
protected:
	// 析构函数用于清理非托管资源
	!MakeCylinder() {
		if (myMaker != 0) {
			delete myMaker;
			myMaker = 0;
		}
	}

	// 终结器（finalizer）用于垃圾回收时的清理
	~MakeCylinder() {
		// 调用析构函数来清理非托管资源
		this->!MakeCylinder();
	}
};

}
}
}