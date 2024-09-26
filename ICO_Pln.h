#pragma once

//前向声明
class gp_Pln;
namespace OCCTK {
namespace OCC {
namespace gp {
ref class Trsf;
ref class Ax3;
ref class Pnt;
ref class Dir;
}
}
}

namespace OCCTK {
namespace OCC {
namespace gp {

public ref class Pln :System::ICloneable {
public:
	Pln();
	Pln(gp_Pln pln);
	Pln(gp_Pln* pln);
	Pln(Ax3^ axis);
	Pln(Pnt^ location, Dir^ direction);
	gp_Pln GetOCC();
	virtual System::Object^ Clone();
protected:
	gp_Pln* myPln;
protected:
	// 析构函数用于清理非托管资源
	!Pln() {
		if (myPln != 0) {
			delete myPln;
			myPln = 0;
		}
	}
	// 终结器（finalizer）用于垃圾回收时的清理
	~Pln() {
		// 调用析构函数来清理非托管资源
		this->!Pln();
	}
};

}
}
}

