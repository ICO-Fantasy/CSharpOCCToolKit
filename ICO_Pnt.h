#pragma once
#include <gp_Pnt.hxx>

namespace OCCTK {
namespace OCC {
namespace gp {

public ref class Pnt {
public:
	Pnt();
	Pnt(double X, double Y, double Z);
	Pnt(gp_Pnt thePnt);
	Pnt(gp_Pnt* thePnt);
	gp_Pnt GetOCC();
	virtual System::String^ ToString() override;
public:
	property double X {double get() { return myPnt->X(); }; void set(double value) { myPnt->SetX(value); } };
	property double Y {double get() { return myPnt->Y(); }; void set(double value) { myPnt->SetY(value); } };
	property double Z {double get() { return myPnt->Z(); }; void set(double value) { myPnt->SetZ(value); } };
	double Distance(Pnt^ otherPnt);
private:
	gp_Pnt* myPnt;
protected:
	// 析构函数用于清理非托管资源
	!Pnt() {
		if (myPnt != 0) {
			delete myPnt;
			myPnt = 0;
		}
	}

	// 终结器（finalizer）用于垃圾回收时的清理
	~Pnt() {
		// 调用析构函数来清理非托管资源
		this->!Pnt();
	}
};

}
}
}