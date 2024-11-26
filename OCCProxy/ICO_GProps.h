#pragma once

//前向声明
class GProp_GProps;
namespace OCCTK {
namespace OCC {
namespace gp {
value struct Pnt;
}
}
}


namespace OCCTK {
namespace OCC {
namespace GProp {

public ref class GProps {
public:
	GProps();
	GProps(GProp_GProps g);
	GProps(GProp_GProps* intptr);
	GProp_GProps GetOCC();
public:
	double Mass();
	gp::Pnt CentreOfMass();
	array<double, 2>^ MatrixOfInertia();
private:
	GProp_GProps* myGProps;
protected:
	// 析构函数用于清理非托管资源
	!GProps() {
		if (myGProps != 0) {
			delete myGProps;
			myGProps = 0;
		}
	}

	// 终结器（finalizer）用于垃圾回收时的清理
	~GProps() {
		// 调用析构函数来清理非托管资源
		this->!GProps();
	}
};

}
}
}

