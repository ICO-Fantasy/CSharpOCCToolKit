#pragma once
#include <BRepBuilderAPI_MakeWire.hxx>
#include <TopTools_ListOfShape.hxx>

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
    MakeWire(Topo::TEdge^ edge);
    MakeWire(System::Collections::Generic::List<Topo::TEdge^>^ edges);
public:
	void Add(Topo::TEdge^ edge);
	void Add(Topo::TWire^ wire);
	void Add(System::Collections::Generic::List<Topo::TEdge^>^ edges);
	property bool IsDone {bool get(); }
	Topo::TShape^ Shape();
	Topo::TWire^ Wire();
	//! 隐式转换为 TWire
	static operator Topo::TWire ^ (MakeWire^ makeWire) {
		return makeWire->Wire();  // 隐式调用 Wire 方法
	}
	bool Error();
private:
    BRepBuilderAPI_MakeWire* myMaker;
private:
    // 终结器（finalizer）用于垃圾回收时的清理
    !MakeWire() {
        delete myMaker;
        myMaker = nullptr;
    }

    // 析构函数用于清理非托管资源
    ~MakeWire() {
        // 调用析构函数来清理非托管资源
        this->!MakeWire();
    }
};

}
}
}