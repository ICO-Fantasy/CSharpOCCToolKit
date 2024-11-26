﻿#pragma once

namespace OCCTK {
namespace Extension {
value struct Color;
ref class XShape;
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
	Extension::XShape^ Shape();
	//Extension::XShape^ AssemblyShape();
	//! 隐式转换为 TShape
	static operator Extension::XShape ^ (OBJExchange^ Ex) {
		return Ex->Shape();
	}
	//bool SaveFile(System::String^ filePath);
private:
	Extension::XShape^ myShape;
};


}
}
