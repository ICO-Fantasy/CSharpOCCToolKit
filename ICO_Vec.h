#pragma once
#include <gp_Vec.hxx>


namespace OCCTK {
namespace OCC {
namespace gp {

public ref class Vec {
public:
	Vec();
	Vec(double X, double Y, double Z);
	Vec(gp_Vec theVec);
	Vec(gp_Vec* theVec);
	gp_Vec GetOCC();
	property double X {double get() { return myVec->X(); }; void set(double value) { myVec->SetX(value); } };
	property double Y {double get() { return myVec->Y(); }; void set(double value) { myVec->SetY(value); } };
	property double Z {double get() { return myVec->Z(); }; void set(double value) { myVec->SetZ(value); } };
	bool IsParallel(Vec^ otherVec, double theAngularTolerance);
protected:
	gp_Vec* myVec;
protected:
	// 析构函数用于清理非托管资源
	!Vec() {
		if (myVec != 0) {
			delete myVec;
			myVec = 0;
		}
	}

	// 终结器（finalizer）用于垃圾回收时的清理
	~Vec() {
		// 调用析构函数来清理非托管资源
		this->!Vec();
	}
};

}
}
}