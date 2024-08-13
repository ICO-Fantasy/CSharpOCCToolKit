#pragma once
#include "WAIS_Trihedron.h"
#include <Geom_Axis2Placement.hxx>

namespace OCCTK {
namespace OCC {
namespace AIS {


//创建原点坐标轴
WAIS_Trihedron::WAIS_Trihedron(float axisSize) {
	myTrihedron() = new AIS_Trihedron(new Geom_Axis2Placement(gp_Ax2()));
	SetAxis();
}

// 根据对象创建坐标轴
WAIS_Trihedron::WAIS_Trihedron(WAIS_Shape^ theAIS, float axisSize) {
	auto t = theAIS->GetOCC()->LocalTransformation();
	gp_Ax2 a = gp_Ax2();
	a.Transform(t);
	myTrihedron() = new AIS_Trihedron(new Geom_Axis2Placement(a));
	SetAxis();
}

// 根据变换创建坐标轴
WAIS_Trihedron::WAIS_Trihedron(gp::Wgp_Trsf^ theTrsf, float axisSize) {
	gp_Ax2 a = gp_Ax2();
	a.Transform(*theTrsf->GetOCC());
	myTrihedron() = new AIS_Trihedron(new Geom_Axis2Placement(a));
	SetAxis();
}

void WAIS_Trihedron::SetArrowLength(double value) {
	ArrowLength = value;
}

void WAIS_Trihedron::SetArrowWidth(double value) {
	ArrowWidth = value;
}


Handle(AIS_Trihedron) WAIS_Trihedron::GetOCC() {
	return myTrihedron();

}

Handle(Standard_Transient) WAIS_Trihedron::GetStd() {
	return myTrihedron();
}
void WAIS_Trihedron::SetAxis() {
	// 显示箭头
	myTrihedron()->SetDrawArrows(true);
	// 箭头长度
	myTrihedron()->SetSize(ArrowLength);
	// 箭头宽度
	myTrihedron()->Attributes()->DatumAspect()->LineAspect(Prs3d_DatumParts_XAxis)->SetWidth(ArrowWidth);
	myTrihedron()->Attributes()->DatumAspect()->LineAspect(Prs3d_DatumParts_YAxis)->SetWidth(ArrowWidth);
	myTrihedron()->Attributes()->DatumAspect()->LineAspect(Prs3d_DatumParts_ZAxis)->SetWidth(ArrowWidth);

	// 坐标轴颜色
	myTrihedron()->SetDatumPartColor(Prs3d_DatumParts_XAxis, Ex::WColor().XAxis);
	myTrihedron()->SetDatumPartColor(Prs3d_DatumParts_YAxis, Ex::WColor().YAxis);
	myTrihedron()->SetDatumPartColor(Prs3d_DatumParts_ZAxis, Ex::WColor().ZAxis);
	// 箭头颜色
	myTrihedron()->SetArrowColor(Prs3d_DatumParts_XAxis, Ex::WColor().XAxis);
	myTrihedron()->SetArrowColor(Prs3d_DatumParts_YAxis, Ex::WColor().YAxis);
	myTrihedron()->SetArrowColor(Prs3d_DatumParts_ZAxis, Ex::WColor().ZAxis);
	// 文字颜色
	myTrihedron()->SetTextColor(Prs3d_DatumParts_XAxis, Ex::WColor().XAxis);
	myTrihedron()->SetTextColor(Prs3d_DatumParts_YAxis, Ex::WColor().YAxis);
	myTrihedron()->SetTextColor(Prs3d_DatumParts_ZAxis, Ex::WColor().ZAxis);
}
}
}
}
