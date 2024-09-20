#include "ICO_BRepAdaptor_Curve.h"
#include "ICO_Topo_Edge.h"
#include "ICO_Topo_Face.h"
#include "ICO_Pnt.h"

using namespace OCCTK::OCC::Topo;
using namespace OCCTK::OCC::gp;

namespace OCCTK {
namespace OCC {
namespace BRepAdaptor {

/// <summary>
/// 创建空的构造器
/// </summary>
Curve::Curve() {
	myCure() = new BRepAdaptor_Curve();
}

/// <summary>
/// 从TopoEdge构建3D曲线
/// </summary>
/// <param name="edge"></param>
Curve::Curve(TEdge^ edge) {
	myCure() = new BRepAdaptor_Curve(edge->GetOCC());
}

Curve::Curve(TEdge^ edge, TFace^ face) {
	myCure() = new BRepAdaptor_Curve(edge->GetOCC(), face->GetOCC());
}

Pnt^ Curve::Value(double UVValue) {
	return gcnew Pnt(myCure()->Value(UVValue));
}

double Curve::FirstParameter() {
	return myCure()->FirstParameter();
}

double Curve::LastParameter() {
	return myCure()->LastParameter();
}

}
}
}