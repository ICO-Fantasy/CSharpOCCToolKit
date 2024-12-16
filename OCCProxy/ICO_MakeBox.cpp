#include "ICO_MakeBox.h"
//local
#include "ICO_Topo_Shape.h"
#include "ICO_Ax2.h"
#include "ICO_Pnt.h"

using namespace OCCTK::OCC::Topo;
using namespace OCCTK::OCC::gp;

namespace OCCTK {
namespace OCC {
namespace BRepPrimAPI {

/// <summary>
/// 从原点构建矩形
/// </summary>
/// <param name="dX">长</param>
/// <param name="dY">宽</param>
/// <param name="dZ">高</param>
MakeBox::MakeBox(double dX, double dY, double dZ) {
	myMaker = new BRepPrimAPI_MakeBox(dX, dY, dZ);
}

/// <summary>
/// 从两角点构建矩形
/// </summary>
/// <param name="leftBottom"></param>
/// <param name="rightTop"></param>
MakeBox::MakeBox(Pnt leftBottom, Pnt rightTop) {
	myMaker = new BRepPrimAPI_MakeBox(leftBottom, rightTop);
}

/// <summary>
/// 以位姿为中心创建矩形
/// </summary>
/// <param name="axis">中心位姿</param>
/// <param name="ldX">-X</param>
/// <param name="ldY">-Y</param>
/// <param name="ldZ">-Z</param>
/// <param name="rdX">X</param>
/// <param name="rdY">Y</param>
/// <param name="rdZ">Z</param>
MakeBox::MakeBox(Ax2 axis, double ldX, double ldY, double ldZ, double rdX, double rdY, double rdZ) {
	double dX = rdX + ldX;
	double dY = rdY + ldY;
	double dZ = rdZ + ldZ;
	gp_Ax2 newAix = axis;
	gp_Vec vecX = gp_Vec(newAix.XDirection());
	gp_Vec vecY = gp_Vec(newAix.YDirection());
	gp_Vec vecZ = gp_Vec(newAix.Direction());
	vecX.Multiply(-ldX);
	vecY.Multiply(-ldY);
	vecZ.Multiply(-ldZ);
	gp_Trsf tX = gp_Trsf();
	gp_Trsf tY = gp_Trsf();
	gp_Trsf tZ = gp_Trsf();
	tX.SetTranslationPart(vecX);
	tY.SetTranslationPart(vecY);
	tZ.SetTranslationPart(vecZ);
	newAix.Transform(tX.Multiplied(tY).Multiplied(tZ));
	myMaker = new BRepPrimAPI_MakeBox(newAix, dX, dY, dZ);
}

TShape^ MakeBox::Shape() {
	return gcnew TShape(myMaker->Shape());
}

}
}
}