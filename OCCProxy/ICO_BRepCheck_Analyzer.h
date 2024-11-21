#pragma once

class BRepCheck_Analyzer;
namespace OCCTK {
namespace OCC {
namespace Topo {
ref class TShape;
}
}
}
namespace OCCTK {
namespace OCC {
namespace BRepCheck {

public ref class Analyzer {
public:
	Analyzer(Topo::TShape^ shape);
	Analyzer(Topo::TShape^ shape, bool GeomControls, bool isParallel);
	//Analyzer(Topo::TShape^ shape, bool GeomControls, bool isParallel, bool isExact);
public:
	bool IsValid();
private:
	BRepCheck_Analyzer* myAna;
protected:
	// 析构函数用于清理非托管资源
	!Analyzer() {
		if (myAna != 0) {
			delete myAna;
			myAna = 0;
		}
	}

	// 终结器（finalizer）用于垃圾回收时的清理
	~Analyzer() {
		// 调用析构函数来清理非托管资源
		this->!Analyzer();
	}
};

}
}
}