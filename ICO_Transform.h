#pragma once
#include <BRepBuilderAPI_Transform.hxx>
//包装C++类到托管类
#include <NCollection_Haft.h> 
namespace OCCTK {
namespace OCC {
namespace gp {
ref class Trsf;
}
namespace Topo {
ref class TShape;
}
}
}

namespace OCCTK {
namespace OCC {
namespace BRepBuilderAPI {

public ref class Transform {
public:
	Transform(Topo::TShape^ theShape, gp::Trsf^ theTrsf);
public:
	Topo::TShape^ Shape();
	//! 隐式转换为 TShape
	static operator Topo::TShape ^ (Transform^ T) {
		return T->Shape();  // 隐式调用 Shape 方法
	}
private:
	BRepBuilderAPI_Transform* myT;
protected:
	// 析构函数用于清理非托管资源
	!Transform() {
		if (myT != 0) {
			delete myT;
			myT = 0;
		}
	}

	// 终结器（finalizer）用于垃圾回收时的清理
	~Transform() {
		// 调用析构函数来清理非托管资源
		this->!Transform();
	}
};

}
}
}