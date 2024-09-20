#pragma once
#include <TopExp_Explorer.hxx>
#include"ICO_ShapeEnum.h"

namespace OCCTK {
namespace OCC {
namespace Topo {
ref class TShape;
}
}
}

namespace OCCTK {
namespace OCC {
namespace TopExp {
public ref class Explorer {
public:
	Explorer();
	Explorer(Topo::TShape^ shape, TopoAbs::ShapeEnum toFind);
	Explorer(Topo::TShape^ shape, TopoAbs::ShapeEnum toFind, TopoAbs::ShapeEnum toAvoid);
	bool More();
	void Next();
	Topo::TShape^ Current();
private:
	TopExp_Explorer* myExplorer;
private:
	// 析构函数用于清理非托管资源
	!Explorer() {
		if (myExplorer != 0) {
			delete myExplorer;
			myExplorer = 0;
		}
	}

	// 终结器（finalizer）用于垃圾回收时的清理
	~Explorer() {
		// 调用析构函数来清理非托管资源
		this->!Explorer();
	}
};

}
}
}

