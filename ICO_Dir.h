#pragma once
#include <gp_Dir.hxx>

namespace OCCTK {
namespace OCC {
namespace gp {
ref class Vec;
}
}
}

namespace OCCTK {
namespace OCC {
namespace gp {

public ref class Dir {
public:
	Dir();
	Dir(double X, double Y, double Z);
	Dir(Vec^ theDir);
	Dir(gp_Dir theDir);
	Dir(gp_Dir* theDir);
	bool IsParallel(Dir^ otherDir, double theAngularTolerance);
	gp_Dir GetOCC();
	property double X {double get() { return myDir->X(); }; void set(double value) { myDir->SetX(value); } };
	property double Y {double get() { return myDir->Y(); }; void set(double value) { myDir->SetY(value); } };
	property double Z {double get() { return myDir->Z(); }; void set(double value) { myDir->SetZ(value); } };
protected:
	gp_Dir* myDir;
protected:
	// 析构函数用于清理非托管资源
	!Dir() {
		if (myDir != 0) {
			delete myDir;
			myDir = 0;
		}
	}

	// 终结器（finalizer）用于垃圾回收时的清理
	~Dir() {
		// 调用析构函数来清理非托管资源
		this->!Dir();
	}
};

}
}
}