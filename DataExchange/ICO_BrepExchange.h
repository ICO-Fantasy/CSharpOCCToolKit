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

public ref class BrepExchange {
public:
	BrepExchange(OCC::Topo::TShape^ theShape);
	BrepExchange(System::String^ filePath);
public:
	void ReadFile(System::String^ filePath);
	bool SaveFile(System::String^ filePath);
	OCC::Topo::TShape^ Shape();
	//! 隐式转换为 TShape
	static operator OCC::Topo::TShape ^ (BrepExchange^ T) {
		return T->Shape();  // 隐式调用 Shape 方法
	}
private:
	OCC::Topo::TShape^ myShape;
};

}
}