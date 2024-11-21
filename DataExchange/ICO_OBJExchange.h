#pragma once
#include "..\Extension\ICO_XShape.h"
namespace OCCTK {
namespace Extension {
value struct Color;
ref class XShape;
ref class XShapeNode;
}
namespace OCC {
namespace Topo {
ref class TShape;
}
}
}

namespace OCCTK {
namespace IO {

public ref class OBJExchange {
public:
	//OBJExchange(OCC::Topo::TShape^ theShape);
	OBJExchange(System::String^ filePath);
public:
	//void ReadFile(System::String^ filePath);
	Extension::XShape^ SingleShape();
	//Extension::XShape^ AssemblyShape();
	//! 隐式转换为 TShape
	static operator OCC::Topo::TShape ^ (OBJExchange^ Ex) {
		return Ex->SingleShape()->TopoShape;
	}
	//bool SaveFile(System::String^ filePath);
private:
	Extension::XShape^ mySingleShape;
	Extension::XShape^ myAssemblyShape;
};


}
}

