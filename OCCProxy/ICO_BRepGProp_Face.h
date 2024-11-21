#pragma once
#pragma once
#include "ICO_GProps.h"

class BRepGProp_Face;
namespace OCCTK {
namespace OCC {
namespace Topo {
ref class TFace;
}
namespace Gprop {
ref class GProps;
}
namespace gp {
ref class Pnt;
ref class Vec;
}
}
}


namespace OCCTK {
namespace OCC {
namespace BRepGProp {

public ref class BRepGlobalProperties_Face {
public:
	BRepGlobalProperties_Face(Topo::TFace^ face);
public:
	gp::Vec^ Normal(double U, double V);
private:
	BRepGProp_Face* myGPFace;
protected:
	// 析构函数用于清理非托管资源
	!BRepGlobalProperties_Face() {
		if (myGPFace != 0) {
			delete myGPFace;
			myGPFace = 0;
		}
	}

	// 终结器（finalizer）用于垃圾回收时的清理
	~BRepGlobalProperties_Face() {
		// 调用析构函数来清理非托管资源
		this->!BRepGlobalProperties_Face();
	}
};

}
}
}