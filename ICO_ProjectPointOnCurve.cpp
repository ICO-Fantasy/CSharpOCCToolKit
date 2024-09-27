#include "ICO_ProjectPointOnCurve.h"
#include<GeomAPI_ProjectPointOnCurve.hxx>
//local
#include "ICO_Pnt.h"
#include "ICO_Geom_Curve.h"
#include "ICO_Topo_Edge.h"
#include "ICO_Exception.h"
#include <StdFail_NotDone.hxx>

using namespace OCCTK::OCC::Topo;
using namespace OCCTK::OCC::gp;
using namespace OCCTK::OCC::Geom;
using namespace System;

namespace OCCTK {
namespace OCC {
namespace GeomAPI {

/// <summary>
/// 将点投影到曲线上
/// </summary>
/// <param name="point"></param>
/// <param name="curve"></param>
ProjectPointOnCurve::ProjectPointOnCurve(gp::Pnt^ point, Geom::Curve^ curve) {
	myPPC = new GeomAPI_ProjectPointOnCurve(point->GetOCC(), curve->GetOCC());
}

/// <summary>
/// 将点投影到曲线上
/// </summary>
/// <param name="point">点</param>
/// <param name="curve">曲线</param>
/// <param name="inferior">下限</param>
/// <param name="superior">上限</param>
ProjectPointOnCurve::ProjectPointOnCurve(gp::Pnt^ point, Geom::Curve^ curve, double inferior, double superior) {
	myPPC = new GeomAPI_ProjectPointOnCurve(point->GetOCC(), curve->GetOCC(), inferior, superior);
}

GeomAPI_ProjectPointOnCurve ProjectPointOnCurve::GetOCC() {
	return *myPPC;
}

/// <summary>
/// 返回最近投影点在曲线上的参数
/// </summary>
/// <returns></returns>
double ProjectPointOnCurve::LowerDistanceParameter() {
	try {
		return myPPC->LowerDistanceParameter();
	}
	catch (const StdFail_NotDone& e) {
		throw gcnew Exception(String::Format("计算失败: {0}", gcnew String(e.GetMessageString())));
	}
	CATCH_AND_THROW_OCC_EXCEPTIONS
}
}
}
}
