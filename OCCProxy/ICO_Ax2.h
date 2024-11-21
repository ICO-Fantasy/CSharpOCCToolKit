#pragma once

//前向声明
class gp_Ax2;
namespace OCCTK {
namespace OCC {
namespace gp {
ref class Ax1;
ref class Pnt;
ref class Trsf;
ref class Dir;
}
}
}

namespace OCCTK {
namespace OCC {
namespace gp {
/// <summary>
/// Z方向为主方向，X方向为另一个方向，Y方向自动定义
/// </summary>
public ref class Ax2 :System::ICloneable {
public:
	Ax2();
	Ax2(Pnt^ theLocation, Dir^ theZAxis);
	Ax2(Pnt^ theLocation, Dir^ theZAxis, Dir^ theXAxis);
	Ax2(gp_Ax2 theAx2);
	Ax2(gp_Ax2* theAx2);
	Ax2(array<double, 2>^ matrix);
	Ax2(array<array<double>^>^ matrix);
	gp_Ax2 GetOCC();
	virtual System::Object^ Clone();
	virtual System::String^ ToString() override;

	void Transform(Trsf^ theT);
	Ax2^ Transformed(Trsf^ theT);

public:
	property Pnt^ Location;
	property Dir^ XDir;
	property Dir^ YDir {Dir^ get(); }
	property Dir^ ZDir;
	property Ax1^ XAxis {Ax1^ get(); }
	property Ax1^ YAxis {Ax1^ get(); }
	property Ax1^ ZAxis {Ax1^ get(); }
};

}
}
}