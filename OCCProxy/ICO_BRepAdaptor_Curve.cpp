#include "ICO_BRepAdaptor_Curve.h"
#include <BRepAdaptor_Curve.hxx>
//local
#include "ICO_BaseObject.h"
#include "ICO_Pnt.h"
#include "ICO_Topo_Edge.h"
#include "ICO_Topo_Face.h"
#include "ICO_CurveType.h"
#include "ICO_Circle.h"

using namespace OCCTK::OCC::Topo;
using namespace OCCTK::OCC::gp;
using namespace OCCTK::OCC::GeomAbs;

namespace OCCTK {
namespace OCC {
namespace BRepAdaptor {

/// <summary>
/// 创建空的构造器
/// </summary>
Curve::Curve() :BaseObject() {
	NativeHandle = new BRepAdaptor_Curve();
}

/// <summary>
/// 从TopoEdge构建3D曲线
/// </summary>
/// <param name="edge"></param>
Curve::Curve(TEdge^ edge) :BaseObject() {
	NativeHandle = new BRepAdaptor_Curve(edge->GetOCC());
}

Curve::Curve(TEdge^ edge, TFace^ face) :BaseObject() {
	NativeHandle = new BRepAdaptor_Curve(edge->GetOCC(), face->GetOCC());
}

/// <summary>
/// 获取对应参数处的点
/// </summary>
/// <param name="UVValue"></param>
/// <returns></returns>
Pnt^ Curve::Value(double UVValue) {
	return gcnew Pnt(myCure()->Value(UVValue));
}

/// <summary>
/// 首端参数
/// </summary>
/// <returns></returns>
double Curve::FirstParameter() {
	return myCure()->FirstParameter();
}

/// <summary>
/// 末端参数
/// </summary>
/// <returns></returns>
double Curve::LastParameter() {
	return myCure()->LastParameter();
}

CurveType Curve::GetType() {
	return CurveType(myCure()->GetType());
}

gp::Circle^ Curve::Circle() {
	return gcnew gp::Circle(myCure()->Circle());
}

}
}
}