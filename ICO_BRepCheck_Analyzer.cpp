#include "ICO_BRepCheck_Analyzer.h"
#include<BRepCheck_Analyzer.hxx>
//local
#include"ICO_Topo_Shape.h"
#include"ICO_Exception.h"

using namespace OCCTK::OCC::Topo;

namespace OCCTK {
namespace OCC {
namespace BRepCheck {

/// <summary>
/// 执行几何检查
/// </summary>
/// <param name="shape"></param>
Analyzer::Analyzer(TShape^ shape) {
	try {
		myAna = new BRepCheck_Analyzer(shape->GetOCC());
	}CATCH_AND_THROW_OCC_EXCEPTIONS
}

/// <summary>
/// 执行几何检查
/// </summary>
/// <param name="shape">需要检查的形状</param>
/// <param name="GeomControls">是否执行几何检查。如果为 True，则会进行几何检查（如边的参数化、面的光顺性等）；如果为 False，则仅检查拓扑信息（如边是否闭合、壳体是否封闭等）。</param>
/// <param name="isParallel">是否使用并行模式来提高检查速度。</param>
/// <param name="isExact">是否使用精确方法来检查几何。</param>
Analyzer::Analyzer(TShape^ shape, bool GeomControls, bool isParallel, bool isExact) {
	myAna = new BRepCheck_Analyzer(shape->GetOCC(), GeomControls, isParallel, isExact);
}

/// <summary>
/// 是否有效
/// </summary>
/// <returns></returns>
bool Analyzer::IsValid() {
	return myAna->IsValid();
}
}
}
}