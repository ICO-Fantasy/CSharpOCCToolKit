#include "ICO_BRepAdaptor_Surface.h"
#include <BRepAdaptor_Surface.hxx>
//local
#include "ICO_BaseObject.h"
#include "ICO_Pnt.h"
#include "ICO_Cylinder.h"
#include "ICO_SurfaceType.h"
#include "ICO_Topo_Face.h"

using namespace OCCTK::OCC::Topo;
using namespace OCCTK::OCC::gp;
using namespace OCCTK::OCC::GeomAbs;

namespace OCCTK {
namespace OCC {
namespace BRepAdaptor {

/// <summary>
/// 创建空的构造器
/// </summary>
Surface::Surface() :BaseObject() {
	NativeHandle = new BRepAdaptor_Surface();
}

/// <summary>
/// 从TopoFace构建Brep曲面
/// </summary>
/// <param name="edge"></param>
Surface::Surface(TFace^ face) :BaseObject() {
	NativeHandle = new BRepAdaptor_Surface(face->GetOCC());
}

/// <summary>
/// 获取对应参数处的点
/// </summary>
/// <param name="U"></param>
/// <param name="V"></param>
/// <returns></returns>
Pnt^ Surface::Value(double U, double V) {
	return gcnew Pnt(mySurface()->Value(U, V));
}

SurfaceType Surface::GetType() {
	return SurfaceType(mySurface()->GetType());
}

gp::Cylinder^ Surface::Cylinder() {
	return gcnew gp::Cylinder(mySurface()->Cylinder());
}

double Surface::FirstUParameter() {
	return mySurface()->FirstUParameter();
}

double Surface::FirstVParameter() {
	return mySurface()->FirstVParameter();
}

double Surface::LastUParameter() {
	return mySurface()->LastUParameter();
}

double Surface::LastVParameter() {
	return mySurface()->LastVParameter();
}

}
}
}