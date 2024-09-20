#pragma once

namespace OCCTK {
namespace OCC {
namespace Topo {
ref class TShape;
}
}
}

namespace OCCTK {
namespace IO {

public ref class STEPExchange {
public:
	STEPExchange(OCC::Topo::TShape^ theShape);
	STEPExchange(System::String^ filePath);
public:
	void ReadFile(System::String^ filePath);
	OCC::Topo::TShape^ Shape();
	//! 隐式转换为 TShape
	static operator OCC::Topo::TShape ^ (STEPExchange^ T) {
		return T->Shape();  // 隐式调用 Shape 方法
	}
	bool SaveFile(System::String^ filePath);
private:
	OCC::Topo::TShape^ myShape;
};

}
}

