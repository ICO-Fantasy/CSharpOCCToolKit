#include "ICO_MakeSphere.h"
#include <BRepPrimAPI_MakeSphere.hxx>
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

/// <summary>
/// 在指定位置创建一个球体
/// </summary>
/// <param name="X"></param>
/// <param name="Y"></param>
/// <param name="Z"></param>
/// <param name="r">半径</param>
MakeSphere::MakeSphere(double X, double Y, double Z, double r) {
	myMaker = new BRepPrimAPI_MakeSphere(gp_Pnt(X, Y, Z), r);
}

/// <summary>
/// 在指定位置创建一个球体
/// </summary>
/// <param name="originPnt">圆心点</param>
/// <param name="r">半径</param>
MakeSphere::MakeSphere(Pnt originPnt, double r) {
	myMaker = new BRepPrimAPI_MakeSphere(originPnt, r);
}

/// <summary>
/// 在指定位姿创建一个球体
/// </summary>
/// <param name="theAxis">圆心坐标</param>
/// <param name="r">半径</param>
MakeSphere::MakeSphere(Ax2 theAxis, double r) {
	myMaker = new BRepPrimAPI_MakeSphere(theAxis, r);
}

TShape^ MakeSphere::Shape() {
	try {
		return gcnew TShape(myMaker->Shape());
	}
	CATCH_AND_THROW_OCC_EXCEPTIONS
}


}
}
}
