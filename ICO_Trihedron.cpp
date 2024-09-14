#pragma once
#include "ICO_Trihedron.h"
#include <Geom_Axis2Placement.hxx>

namespace OCCTK {
namespace OCC {
namespace AIS {


//创建原点坐标轴
Trihedron::Trihedron(double axisSize) :InteractiveObject() {
	NativeHandle = new AIS_Trihedron(new Geom_Axis2Placement(gp_Ax2()));

	ArrowLength = axisSize;
	SetAxis();
}

// 根据对象创建坐标轴
Trihedron::Trihedron(AShape^ theAIS, double axisSize) :InteractiveObject() {
	gp_Trsf t = theAIS->GetOCC()->LocalTransformation();
	gp_Ax2 a = gp_Ax2();
	a.Transform(t);
	NativeHandle = new AIS_Trihedron(new Geom_Axis2Placement(a));

	ArrowLength = axisSize;
	SetAxis();
}

// 根据变换创建坐标轴
Trihedron::Trihedron(gp::Trsf^ theTrsf, double axisSize) :InteractiveObject() {
	gp_Ax2 a = gp_Ax2();
	a.Transform(theTrsf->GetOCC());
	NativeHandle = new AIS_Trihedron(new Geom_Axis2Placement(a));

	ArrowLength = axisSize;
	SetAxis();
}

void Trihedron::SetArrowLength(double value) {
	ArrowLength = value;
}

void Trihedron::SetArrowWidth(double value) {
	ArrowWidth = value;
}

void Trihedron::SetAspect(int theX, int theY) {
	myTrihedron()->SetTransformPersistence(new Graphic3d_TransformPers(Graphic3d_TMF_2d, Aspect_TypeOfTriedronPosition::Aspect_TOTP_RIGHT_LOWER, Graphic3d_Vec2i(theY, theY)));
}


void Trihedron::SetAxis() {
	// 显示箭头
	myTrihedron()->SetDrawArrows(true);
	// 箭头长度
	myTrihedron()->SetSize(ArrowLength);
	// 箭头宽度
	myTrihedron()->Attributes()->DatumAspect()->LineAspect(Prs3d_DatumParts_XAxis)->SetWidth(ArrowWidth);
	myTrihedron()->Attributes()->DatumAspect()->LineAspect(Prs3d_DatumParts_YAxis)->SetWidth(ArrowWidth);
	myTrihedron()->Attributes()->DatumAspect()->LineAspect(Prs3d_DatumParts_ZAxis)->SetWidth(ArrowWidth);

	// 坐标轴颜色
	myTrihedron()->SetDatumPartColor(Prs3d_DatumParts_XAxis, Extension::Color().XAxis);
	myTrihedron()->SetDatumPartColor(Prs3d_DatumParts_YAxis, Extension::Color().YAxis);
	myTrihedron()->SetDatumPartColor(Prs3d_DatumParts_ZAxis, Extension::Color().ZAxis);
	// 箭头颜色
	myTrihedron()->SetArrowColor(Prs3d_DatumParts_XAxis, Extension::Color().XAxis);
	myTrihedron()->SetArrowColor(Prs3d_DatumParts_YAxis, Extension::Color().YAxis);
	myTrihedron()->SetArrowColor(Prs3d_DatumParts_ZAxis, Extension::Color().ZAxis);
	// 文字颜色
	myTrihedron()->SetTextColor(Prs3d_DatumParts_XAxis, Extension::Color().XAxis);
	myTrihedron()->SetTextColor(Prs3d_DatumParts_YAxis, Extension::Color().YAxis);
	myTrihedron()->SetTextColor(Prs3d_DatumParts_ZAxis, Extension::Color().ZAxis);
}

}
}
}
