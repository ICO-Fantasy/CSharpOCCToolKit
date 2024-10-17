#pragma once

class BRepExtrema_DistShapeShape;
namespace OCCTK {
namespace OCC {
namespace Topo {
ref class TShape;
}
}
}


namespace OCCTK {
namespace OCC {
namespace BRepExtrema {

public ref class DistanceShapeShape {
public:
	DistanceShapeShape();
	DistanceShapeShape(Topo::TShape^ shape, Topo::TShape^ otherShape);
public:
	double Value();
	//! 隐式转换为 double
	static operator double(DistanceShapeShape^ theDSS) {
		return theDSS->Value();  // 隐式调用 Edge 方法
	}
private:
	BRepExtrema_DistShapeShape* DSS;
protected:
	// 析构函数用于清理非托管资源
	!DistanceShapeShape() {
		if (DSS != 0) {
			delete DSS;
			DSS = 0;
		}
	}

	// 终结器（finalizer）用于垃圾回收时的清理
	~DistanceShapeShape() {
		// 调用析构函数来清理非托管资源
		this->!DistanceShapeShape();
	}
};

}
}
}