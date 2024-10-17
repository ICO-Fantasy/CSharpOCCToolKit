#pragma once
#include <cstdint>
#include <TopoDS_Shape.hxx>

namespace OCCTK {
namespace OCC {
namespace gp {
ref class Trsf;
ref class Ax2;
}
namespace Topo {
ref class TEdge;
ref class TVertex;
ref class TFace;
}
}
}

namespace OCCTK {
namespace OCC {
namespace Topo {

public ref class TShape {
public:
	TShape();
	TShape(const TopoDS_Shape theShape);
	TShape(System::IntPtr theShapeIntPtr);
	TShape(TopoDS_Shape* theShape);
	bool IsEqual(TShape^ otherShape);
	virtual bool Equals(System::Object^ obj) override;
	TopoDS_Shape GetOCC();
	System::IntPtr GetPtr();
public:
	TVertex^ AsVertex();
	TEdge^ AsEdge();
	TFace^ AsFace();
public:
	void Move(gp::Trsf^ theT);
	gp::Trsf^ Location();
	void Location(gp::Ax2^ newOrigin);
	TShape^ Located(gp::Ax2^ newOrigin);
	virtual int GetHashCode() override {
		return HashCode(INT32_MAX);
	}
	int HashCode(int upperBound);
protected:
	TopoDS_Shape* myShape;
protected:
	// 析构函数用于清理非托管资源
	!TShape() {
		if (myShape != 0) {
			delete myShape;
			myShape = 0;
		}
	}

	// 终结器（finalizer）用于垃圾回收时的清理
	~TShape() {
		// 调用析构函数来清理非托管资源
		this->!TShape();
	}
};

}
}
}