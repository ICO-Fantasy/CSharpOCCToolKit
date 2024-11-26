#pragma once
#include <BRepBuilderAPI_MakePolygon.hxx>

namespace OCCTK {
namespace OCC {
namespace gp {
value struct Trsf;
value struct Pnt;
}
namespace Topo {
ref class TShape;
ref class TVertex;
ref class TEdge;
ref class TWire;
}
namespace Geom {
ref class Curve;
}
}
}

namespace OCCTK {
namespace OCC {
namespace BRepBuilderAPI {

public ref class MakePolygon {
public:
	MakePolygon();
	MakePolygon(gp::Pnt p1, gp::Pnt p2);
	MakePolygon(gp::Pnt p1, gp::Pnt p2, gp::Pnt p3);
	MakePolygon(gp::Pnt p1, gp::Pnt p2, gp::Pnt p3, bool close);
	MakePolygon(gp::Pnt p1, gp::Pnt p2, gp::Pnt p3, gp::Pnt p4);
	MakePolygon(gp::Pnt p1, gp::Pnt p2, gp::Pnt p3, gp::Pnt p4, bool close);
public:
	Topo::TShape^ Shape();
	Topo::TEdge^ Edge();
	Topo::TWire^ Wire();
	//! 隐式转换为 TEdge
	static operator Topo::TEdge ^ (MakePolygon^ makePolygon) {
		return makePolygon->Edge();  // 隐式调用 Edge 方法
	}
private:
	BRepBuilderAPI_MakePolygon* myMaker;
protected:
	// 析构函数用于清理非托管资源
	!MakePolygon() {
		if (myMaker != 0) {
			delete myMaker;
			myMaker = 0;
		}
	}

	// 终结器（finalizer）用于垃圾回收时的清理
	~MakePolygon() {
		// 调用析构函数来清理非托管资源
		this->!MakePolygon();
	}
};

}
}
}