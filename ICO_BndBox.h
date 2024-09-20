#pragma once
#include <Bnd_Box.hxx>
#include <TopoDS_Shape.hxx>
#include <BRepBndLib.hxx>
//包装C++类到托管类
#include <NCollection_Haft.h> 

namespace OCCTK {
namespace OCC {
namespace gp {
ref class Pnt;
}
namespace Topo {
ref class TShape;
}
}
}

namespace OCCTK {
namespace Extension {

public ref class BoundingBox {
public:
	BoundingBox();
	BoundingBox(OCC::Topo::TShape^ TopoShape);
	void Add(OCC::Topo::TShape^ TopoShape);
	OCC::gp::Pnt^ GetLowerLeftCorner();
	array<double>^ GetBndBox();
private:
	Bnd_Box* myBox;
	TopoDS_Shape* myShape;
protected:
	// 析构函数用于清理非托管资源
	!BoundingBox() {
		if (myBox != 0) {
			delete myBox;
			myBox = 0;
		}
		if (myShape != 0) {
			delete myShape;
			myShape = 0;
		}
	}

	// 终结器（finalizer）用于垃圾回收时的清理
	~BoundingBox() {
		// 调用析构函数来清理非托管资源
		this->!BoundingBox();
	}
};

}
}

