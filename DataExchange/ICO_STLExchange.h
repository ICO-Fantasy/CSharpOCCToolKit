﻿#pragma once


namespace OCCTK {
namespace Extension {
value struct Color;
}
namespace OCC {
namespace Topo {
ref class TShape;
ref class TFace;
}
namespace Poly {
ref class PTriangulation;
}
}
}

namespace OCCTK {
namespace IO {

public ref class STLExchange {
public:
	STLExchange(System::String^ filePath);
public:
	OCC::Topo::TShape^ Shape();
	OCC::Poly::PTriangulation^ PolyTriangulation();
	//! 隐式转换为 PTriangulation
	static operator OCC::Topo::TShape ^ (STLExchange^ Ex) {
		return Ex->Shape();
	}
private:
	OCC::Poly::PTriangulation^ myPT;
};

}
}