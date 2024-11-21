#include "ICO_BRepPrimAPI_MakeRevol.h"
//local
#include"ICO_Topo_Shape.h"
#include"ICO_Ax1.h"
#include "ICO_Exception.h"

using namespace OCCTK::OCC::Topo;
using namespace OCCTK::OCC::gp;

namespace OCCTK {
namespace OCC {
namespace BRepPrimAPI {
/// <summary>
/// 构建回转体（整个圆周）
/// </summary>
/// <param name="shape">基面</param>
/// <param name="axis">旋转轴</param>
MakeRevol::MakeRevol(Topo::TShape^ shape, gp::Ax1^ axis) {
	myMaker = new BRepPrimAPI_MakeRevol(shape->GetOCC(), axis->GetOCC());
}
/// <summary>
/// 构建回转体（整个圆周）
/// </summary>
/// <param name="shape">基面</param>
/// <param name="axis">旋转轴</param>
/// <param name="copy">是否复制基面</param>
MakeRevol::MakeRevol(Topo::TShape^ shape, gp::Ax1^ axis, bool copy) {
	myMaker = new BRepPrimAPI_MakeRevol(shape->GetOCC(), axis->GetOCC(), copy);
}
/// <summary>
/// 构建回转体
/// </summary>
/// <param name="shape">基面</param>
/// <param name="axis">旋转轴</param>
/// <param name="angle">旋转角度</param>
MakeRevol::MakeRevol(Topo::TShape^ shape, gp::Ax1^ axis, double angle) {
	myMaker = new BRepPrimAPI_MakeRevol(shape->GetOCC(), axis->GetOCC(), angle);
}
/// <summary>
/// 构建回转体
/// </summary>
/// <param name="shape">基面</param>
/// <param name="axis">旋转轴</param>
/// <param name="angle">旋转角度</param>
/// <param name="copy">是否复制基面</param>
MakeRevol::MakeRevol(Topo::TShape^ shape, gp::Ax1^ axis, double angle, bool copy) {
	myMaker = new BRepPrimAPI_MakeRevol(shape->GetOCC(), axis->GetOCC(), angle, copy);
}

Topo::TShape^ MakeRevol::Shape() {
	try {
		return gcnew TShape(myMaker->Shape());
	}
	CATCH_AND_THROW_OCC_EXCEPTIONS
}

}
}
}