#pragma once
#pragma once
#include <BRepPrimAPI_MakeRevol.hxx>

//前向声明
namespace OCCTK {
namespace OCC {
namespace gp {
ref class Ax1;
}
namespace Topo {
ref class TShape;
}
}
}

namespace OCCTK {
namespace OCC {
namespace BRepPrimAPI {

public ref class MakeRevol {
public:
	MakeRevol(Topo::TShape^ shape, gp::Ax1^ axis);
	MakeRevol(Topo::TShape^ shape, gp::Ax1^ axis, bool copy);
	MakeRevol(Topo::TShape^ shape, gp::Ax1^ axis, double angle);
	MakeRevol(Topo::TShape^ shape, gp::Ax1^ axis, double angle, bool copy);
public:
	Topo::TShape^ Shape();
	//! 隐式转换为 TShape
	static operator Topo::TShape ^ (MakeRevol^ makeRevol) {
		return makeRevol->Shape();  // 隐式调用 Edge 方法
	}
private:
	BRepPrimAPI_MakeRevol* myMaker;
protected:
	// 析构函数用于清理非托管资源
	!MakeRevol() {
		if (myMaker != 0) {
			delete myMaker;
			myMaker = 0;
		}
	}

	// 终结器（finalizer）用于垃圾回收时的清理
	~MakeRevol() {
		// 调用析构函数来清理非托管资源
		this->!MakeRevol();
	}
};

}
}
}

