#include "ICO_BRep_Tool.h"
#include <BRep_Tool.hxx>
#include<TopoDS_Vertex.hxx>
//local
#include "ICO_Pnt.h"
#include "ICO_Geom_Curve.h"
#include "ICO_Topo_Vertex.h"
#include "ICO_Topo_Edge.h"
#include "ICO_Exception.h"

using namespace OCCTK::OCC::gp;
using namespace OCCTK::OCC::Topo;
using namespace System;

namespace OCCTK {
namespace OCC {
namespace BRep {

/// <summary>
/// 返回TopoDS_Vertex对应的gp_Pnt点
/// </summary>
/// <param name="vertex"></param>
/// <returns></returns>
gp::Pnt^ Tool::Pnt(TVertex^ vertex) {
	return gcnew gp::Pnt(BRep_Tool::Pnt(vertex->GetOCC()));
}

/// <summary>
/// 获取参数化曲线
/// </summary>
/// <param name="edge"></param>
/// <param name="first"></param>
/// <param name="last"></param>
/// <returns></returns>
Geom::Curve^ Tool::Curve(Topo::TEdge^ edge, double% first, double% last) {
	// 使用 pin_ptr 固定托管 double% 的位置并获取其非托管指针
	pin_ptr<double> firstPtr = &first;
	pin_ptr<double> lastPtr = &last;
	TopLoc_Location l = TopLoc_Location();
	try {
		return gcnew Geom::Curve(BRep_Tool::Curve(edge->GetOCC(), l, *firstPtr, *lastPtr));
	}
	CATCH_AND_THROW_OCC_EXCEPTIONS
}

}
}
}

