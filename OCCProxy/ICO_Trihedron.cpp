#pragma once
#include "ICO_Trihedron.h"
#include <Geom_Axis2Placement.hxx>
#include <Prs3d_DatumParts.hxx>
//local
#include "..\Extension\ICO_Color.h"

namespace OCCTK {
namespace OCC {
namespace AIS {


/// <summary>
/// 创建原点坐标轴
/// </summary>
/// <param name="axisSize"></param>
ATrihedron::ATrihedron(double axisSize) :InteractiveObject() {
	NativeHandle = new AIS_Trihedron(new Geom_Axis2Placement(gp_Ax2()));

	ArrowLength = axisSize;
	SetDefaultAxis();
}

/// <summary>
/// 根据AIS对象的位置创建坐标轴
/// </summary>
/// <param name="theAIS"></param>
/// <param name="axisSize"></param>
ATrihedron::ATrihedron(AShape^ theAIS, double axisSize) :InteractiveObject() {
	gp_Trsf t = theAIS->GetOCC()->LocalTransformation();
	gp_Ax2 a = gp_Ax2();
	a.Transform(t);
	NativeHandle = new AIS_Trihedron(new Geom_Axis2Placement(a));

	ArrowLength = axisSize;
	SetDefaultAxis();
}

/// <summary>
/// 根据变换创建坐标轴
/// </summary>
/// <param name="theTrsf"></param>
/// <param name="axisSize"></param>
ATrihedron::ATrihedron(gp::Trsf^ theTrsf, double axisSize) :InteractiveObject() {
	gp_Ax2 a = gp_Ax2();
	a.Transform(theTrsf);
	NativeHandle = new AIS_Trihedron(new Geom_Axis2Placement(a));

	ArrowLength = axisSize;
	SetDefaultAxis();
}

/// <summary>
/// 设置部分箭头的宽度
/// </summary>
/// <param name="axis"></param>
/// <param name="value"></param>
void ATrihedron::SetArrowWidth(DatumParts axis, double value) {
	myTrihedron()->Attributes()->DatumAspect()->LineAspect(Prs3d_DatumParts(axis))->SetWidth(value);
}

/// <summary>
/// 设置部分箭头和文字的颜色
/// </summary>
/// <param name="axis"></param>
/// <param name="color"></param>
void ATrihedron::SetColor(DatumParts axis, Extension::Color color) {
	// 坐标轴颜色
	myTrihedron()->SetDatumPartColor(Prs3d_DatumParts(axis), color);
	// 箭头颜色
	myTrihedron()->SetArrowColor(Prs3d_DatumParts(axis), color);
	// 文字颜色
	myTrihedron()->SetTextColor(Prs3d_DatumParts(axis), color);
}

/// <summary>
/// 设置坐标轴颜色
/// </summary>
/// <param name="axis"></param>
/// <param name="color"></param>
void ATrihedron::SetDatumColor(DatumParts axis, Extension::Color color) {
	// 坐标轴颜色
	myTrihedron()->SetDatumPartColor(Prs3d_DatumParts(axis), color);
}

/// <summary>
/// 设置箭头颜色
/// </summary>
/// <param name="axis"></param>
/// <param name="color"></param>
void ATrihedron::SetArrowColor(DatumParts axis, Extension::Color color) {
	// 箭头颜色
	myTrihedron()->SetArrowColor(Prs3d_DatumParts(axis), color);
}

/// <summary>
/// 设置文字颜色
/// </summary>
/// <param name="axis"></param>
/// <param name="color"></param>
void ATrihedron::SetTextColor(DatumParts axis, Extension::Color color) {
	// 文字颜色
	myTrihedron()->SetTextColor(Prs3d_DatumParts(axis), color);
}

/// <summary>
/// 固定在屏幕二维空间上
/// </summary>
/// <param name="theX"></param>
/// <param name="theY"></param>
void ATrihedron::SetAspect(int theX, int theY) {
	myTrihedron()->SetTransformPersistence(new Graphic3d_TransformPers(Graphic3d_TMF_2d, Aspect_TypeOfTriedronPosition::Aspect_TOTP_RIGHT_LOWER, Graphic3d_Vec2i(theY, theY)));
}

void ATrihedron::SetDefaultAxis() {
	// 显示箭头
	myTrihedron()->SetDrawArrows(DrawArrow);
	// 箭头长度
	myTrihedron()->SetSize(ArrowLength);
	// 箭头宽度
	myTrihedron()->Attributes()->DatumAspect()->LineAspect(Prs3d_DatumParts_XAxis)->SetWidth(ArrowWidth);
	myTrihedron()->Attributes()->DatumAspect()->LineAspect(Prs3d_DatumParts_YAxis)->SetWidth(ArrowWidth);
	myTrihedron()->Attributes()->DatumAspect()->LineAspect(Prs3d_DatumParts_ZAxis)->SetWidth(ArrowWidth);
	// 坐标轴颜色
	myTrihedron()->SetDatumPartColor(Prs3d_DatumParts_XAxis, Extension::Color::XAxis);
	myTrihedron()->SetDatumPartColor(Prs3d_DatumParts_YAxis, Extension::Color::YAxis);
	myTrihedron()->SetDatumPartColor(Prs3d_DatumParts_ZAxis, Extension::Color::ZAxis);
	// 箭头颜色
	myTrihedron()->SetArrowColor(Prs3d_DatumParts_XAxis, Extension::Color::XAxis);
	myTrihedron()->SetArrowColor(Prs3d_DatumParts_YAxis, Extension::Color::YAxis);
	myTrihedron()->SetArrowColor(Prs3d_DatumParts_ZAxis, Extension::Color::ZAxis);
	// 文字颜色
	myTrihedron()->SetTextColor(Prs3d_DatumParts_XAxis, Extension::Color::XAxis);
	myTrihedron()->SetTextColor(Prs3d_DatumParts_YAxis, Extension::Color::YAxis);
	myTrihedron()->SetTextColor(Prs3d_DatumParts_ZAxis, Extension::Color::ZAxis);
}

double ATrihedron::ArrowLength::get() {
	return MyArrowLength;
}

void ATrihedron::ArrowLength::set(double value) {
	MyArrowLength = value;
	// 箭头长度
	myTrihedron()->SetSize(MyArrowLength);
}

double ATrihedron::ArrowWidth::get() {
	return MyArrowWidth;
}

void ATrihedron::ArrowWidth::set(double value) {
	MyArrowWidth = value;
	// 箭头宽度
	myTrihedron()->Attributes()->DatumAspect()->LineAspect(Prs3d_DatumParts_XAxis)->SetWidth(MyArrowWidth);
	myTrihedron()->Attributes()->DatumAspect()->LineAspect(Prs3d_DatumParts_YAxis)->SetWidth(MyArrowWidth);
	myTrihedron()->Attributes()->DatumAspect()->LineAspect(Prs3d_DatumParts_ZAxis)->SetWidth(MyArrowWidth);
}

bool ATrihedron::DrawArrow::get() {
	return MyDrawArrow;
}

void ATrihedron::DrawArrow::set(bool value) {
	MyDrawArrow = value;
	// 显示箭头
	myTrihedron()->SetDrawArrows(MyDrawArrow);
}

}
}
}
