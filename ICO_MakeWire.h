#pragma once
#include <BRepBuilderAPI_MakeWire.hxx>

namespace OCCTK {
namespace OCC {
namespace Topo {
ref class TShape;
ref class TWire;
ref class TEdge;
}
}
}

namespace OCCTK {
namespace OCC {
namespace BRepBuilderAPI {

public ref class MakeWire {
public:
	MakeWire();
	MakeWire(System::Collections::Generic::List<Topo::TEdge^>^ edges);
public:
	void Add(Topo::TEdge^ edge);
	Topo::TShape^ Shape();
	Topo::TWire^ Wire();
	//! 隐式转换为 TWire
	static operator Topo::TWire ^ (MakeWire^ makeWire) {
		return makeWire->Wire();  // 隐式调用 Wire 方法
	}
	bool Error();
private:
	BRepBuilderAPI_MakeWire* myMaker;
protected:
	// 终结器（finalizer）用于垃圾回收时的清理
	!MakeWire() {
		if (myMaker != 0) {
			delete myMaker;
			myMaker = 0;
		}
	}

	// 析构函数用于清理非托管资源
	~MakeWire() {
		//if (_isDisposed)
		//	return;
		// 调用析构函数来清理非托管资源
		this->!MakeWire();
		//_isDisposed = true;
	}
};

}
}
}