#pragma once
//值类型作为构造函数成员需要完整定义
#include "ICO_Pnt.h"
#include "ICO_Ax2.h"
//前向声明
class BRepPrimAPI_MakeSphere;

namespace OCCTK {
namespace OCC {
//前向声明
namespace Topo {
ref class TShape;
}
namespace BRepPrimAPI {

public ref class MakeSphere {
public:
	MakeSphere(double X, double Y, double Z, double R);
	MakeSphere(gp::Pnt originPnt, double R);
	MakeSphere(gp::Ax2 theAxis, double R);
	Topo::TShape^ Shape();
	//! 隐式转换为 TShape
	static operator Topo::TShape ^ (MakeSphere^ maker) { return maker->Shape(); }
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