#include "ICO_MakeCylinder.h"
#include <BRepPrimAPI_MakeCylinder.hxx>
//local
#include "ICO_Pnt.h"
#include "ICO_Ax2.h"
#include "ICO_Topo_Shape.h"
#include "ICO_Exception.h"

using namespace OCCTK::OCC::Topo;
using namespace OCCTK::OCC::gp;

namespace OCCTK {
namespace OCC {
namespace BRepPrimAPI {

#define CHECKRH(r,h) 	if (r <= 0) {\
System::String^ str = "半径必须为非零正数，输入值：" + r;\
throw gcnew Exception(str);\
}\
if (h <= 0) {\
	System::String^ str = "高度必须为非零正数，输入值：" + h;\
	throw gcnew Exception(str);\
}

/// <summary>
/// 在原点创建一个圆柱
/// </summary>
/// <param name="r">半径</param>
/// <param name="h">高</param>
MakeCylinder::MakeCylinder(double r, double h) {
	CHECKRH(r, h)
		myMaker = new BRepPrimAPI_MakeCylinder(r, h);
}

/// <summary>
/// 在原点创建一个圆柱，并指定角度
/// </summary>
/// <param name="r">半径</param>
/// <param name="h">高</param>
/// <param name="angle">底面扇形角度</param>
MakeCylinder::MakeCylinder(double r, double h, double angle) {
	CHECKRH(r, h)
		myMaker = new BRepPrimAPI_MakeCylinder(r, h, angle);
}

/// <summary>
/// 在指定位姿创建一个圆柱
/// </summary>
/// <param name="theAxis">底面坐标系</param>
/// <param name="r">半径</param>
/// <param name="h">高</param>
MakeCylinder::MakeCylinder(gp::Ax2 theAxis, double r, double h) {
	CHECKRH(r, h)
		myMaker = new BRepPrimAPI_MakeCylinder(theAxis, r, h);
}

/// <summary>
/// 在指定位姿创建一个圆柱，并指定角度
/// </summary>
/// <param name="r">半径</param>
/// <param name="h">高</param>
/// <param name="angle">底面扇形角度</param>
MakeCylinder::MakeCylinder(gp::Ax2 theAxis, double r, double h, double angle) {
	CHECKRH(r, h)
		myMaker = new BRepPrimAPI_MakeCylinder(theAxis, r, h, angle);
}

TShape^ MakeCylinder::Shape() {
	try {
		return gcnew TShape(myMaker->Shape());
	}
	CATCH_AND_THROW_OCC_EXCEPTIONS
}


}
}
}
