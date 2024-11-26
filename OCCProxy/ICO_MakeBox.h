#pragma once
#include <BRepPrimAPI_MakeBox.hxx>

namespace OCCTK {
namespace OCC {
namespace gp {
value struct Pnt;
value struct Ax2;
}
namespace Topo {
ref class TShape;
}
}
}

namespace OCCTK {
namespace OCC {
namespace BRepPrimAPI {

public ref class MakeBox {
public:
	MakeBox(double X, double Y, double Z);
	MakeBox(gp::Pnt leftBottom, gp::Pnt rightTop);
	MakeBox(gp::Ax2 axis, double ldX, double ldY, double ldZ, double rdX, double rdY, double rdZ);
	Topo::TShape^ Shape();
	//! 隐式转换为 TShape
	static operator Topo::TShape ^ (MakeBox^ mb) {
		return mb->Shape();  // 隐式调用 Shape 方法
	}
private:
	BRepPrimAPI_MakeBox* myMaker;
protected:
	// 析构函数用于清理非托管资源
	!MakeBox() {
		if (myMaker != 0) {
			delete myMaker;
			myMaker = 0;
		}
	}

	// 终结器（finalizer）用于垃圾回收时的清理
	~MakeBox() {
		// 调用析构函数来清理非托管资源
		this->!MakeBox();
	}
};

}
}
}