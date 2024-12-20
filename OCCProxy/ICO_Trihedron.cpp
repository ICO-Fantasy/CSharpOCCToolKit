﻿#pragma once
#include "ICO_Trihedron.h"
#include <Geom_Axis2Placement.hxx>
#include <Prs3d_DatumParts.hxx>
//local
#include "..\Extension\ICO_Color.h"

namespace OCCTK {
namespace OCC {
namespace AIS {


//创建原点坐标轴
ATrihedron::ATrihedron(double axisSize) :InteractiveObject() {
	NativeHandle = new AIS_Trihedron(new Geom_Axis2Placement(gp_Ax2()));

	ArrowLength = axisSize;
	SetDefaultAxis();
}

// 根据对象创建坐标轴
ATrihedron::ATrihedron(AShape^ theAIS, double axisSize) :InteractiveObject() {
	gp_Trsf t = theAIS->GetOCC()->LocalTransformation();
	gp_Ax2 a = gp_Ax2();
	a.Transform(t);
	NativeHandle = new AIS_Trihedron(new Geom_Axis2Placement(a));

	ArrowLength = axisSize;
	SetDefaultAxis();
}

// 根据变换创建坐标轴
ATrihedron::ATrihedron(gp::Trsf^ theTrsf, double axisSize) :InteractiveObject() {
	gp_Ax2 a = gp_Ax2();
	a.Transform(theTrsf);
	NativeHandle = new AIS_Trihedron(new Geom_Axis2Placement(a));

	ArrowLength = axisSize;
	SetDefaultAxis();
}

void ATrihedron::SetArrowLength(double value) {
	ArrowLength = value;
}

void ATrihedron::SetArrowWidth(double value) {
	ArrowWidth = value;
}

void ATrihedron::SetArrowWidth(DatumParts axis, double value) {
	myTrihedron()->Attributes()->DatumAspect()->LineAspect(Prs3d_DatumParts(axis))->SetWidth(value);
}

void ATrihedron::SetColor(DatumParts axis, Extension::Color^ color) {
	myTrihedron()->SetDatumPartColor(Prs3d_DatumParts(axis), color->GetOCC());
}

void ATrihedron::SetAspect(int theX, int theY) {
	myTrihedron()->SetTransformPersistence(new Graphic3d_TransformPers(Graphic3d_TMF_2d, Aspect_TypeOfTriedronPosition::Aspect_TOTP_RIGHT_LOWER, Graphic3d_Vec2i(theY, theY)));
}


void ATrihedron::SetDefaultAxis() {
	// 显示箭头
	myTrihedron()->SetDrawArrows(true);
	// 箭头长度
	myTrihedron()->SetSize(ArrowLength);
	// 箭头宽度
	myTrihedron()->Attributes()->DatumAspect()->LineAspect(Prs3d_DatumParts_XAxis)->SetWidth(ArrowWidth);
	myTrihedron()->Attributes()->DatumAspect()->LineAspect(Prs3d_DatumParts_YAxis)->SetWidth(ArrowWidth);
	myTrihedron()->Attributes()->DatumAspect()->LineAspect(Prs3d_DatumParts_ZAxis)->SetWidth(ArrowWidth);
	// 坐标轴颜色
	myTrihedron()->SetDatumPartColor(Prs3d_DatumParts_XAxis, Extension::Color::XAxis().GetOCC());
	myTrihedron()->SetDatumPartColor(Prs3d_DatumParts_YAxis, Extension::Color::YAxis().GetOCC());
	myTrihedron()->SetDatumPartColor(Prs3d_DatumParts_ZAxis, Extension::Color::ZAxis().GetOCC());
	// 箭头颜色
	myTrihedron()->SetArrowColor(Prs3d_DatumParts_XAxis, Extension::Color::XAxis().GetOCC());
	myTrihedron()->SetArrowColor(Prs3d_DatumParts_YAxis, Extension::Color::YAxis().GetOCC());
	myTrihedron()->SetArrowColor(Prs3d_DatumParts_ZAxis, Extension::Color::ZAxis().GetOCC());
	// 文字颜色
	myTrihedron()->SetTextColor(Prs3d_DatumParts_XAxis, Extension::Color::XAxis().GetOCC());
	myTrihedron()->SetTextColor(Prs3d_DatumParts_YAxis, Extension::Color::YAxis().GetOCC());
	myTrihedron()->SetTextColor(Prs3d_DatumParts_ZAxis, Extension::Color::ZAxis().GetOCC());
}

}
}
}
