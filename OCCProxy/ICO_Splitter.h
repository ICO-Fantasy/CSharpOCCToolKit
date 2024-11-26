#pragma once
#include <BRepAlgoAPI_Splitter.hxx>

namespace OCCTK {
namespace OCC {
namespace gp {
value struct Pnt;
ref class Ax2;
}
namespace Topo {
ref class TShape;
ref class TEdge;
}
}
}

namespace OCCTK {
namespace OCC {
namespace BRepAlgoAPI {

public ref class Splitter {
public:
	Splitter();
public:
	void SetArguments(System::Collections::Generic::List<Topo::TShape^>^ shapeList);
	void SetArguments(Topo::TShape^ singleShape);
	void SetTools(System::Collections::Generic::List<Topo::TShape^>^ shapeList);
	void SetTools(Topo::TShape^ singleTool);
	void SetFuzzyValue(double fuzz);
	void Build();
	System::Collections::Generic::List<Topo::TEdge^>^ SectionEdges();
private:
	BRepAlgoAPI_Splitter* mySplitter;
protected:
	// 析构函数用于清理非托管资源
	!Splitter() {
		if (mySplitter != 0) {
			delete mySplitter;
			mySplitter = 0;
		}
	}

	// 终结器（finalizer）用于垃圾回收时的清理
	~Splitter() {
		// 调用析构函数来清理非托管资源
		this->!Splitter();
	}
};

}
}
}