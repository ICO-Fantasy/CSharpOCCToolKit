#include "ICO_ViewCube.h"
#include <AIS_ViewCube.hxx>
#include <Prs3d_DatumAspect.hxx>
#include <Standard_Handle.hxx>
//local
#include "..\DataExchange\ICO_StringExchange.h"
#include "..\Extension\ICO_Color.h"
#include <Prs3d_DatumParts.hxx>
#include <V3d_TypeOfOrientation.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_Drawer.hxx>

using namespace OCCTK::Extension;

namespace OCCTK {
namespace OCC {
namespace AIS {

/// <summary>
/// 创建视图立方
/// </summary>
ViewCube::ViewCube() :InteractiveObject() {
	NativeHandle = new AIS_ViewCube();
	//修改坐标轴文字颜色
	Handle(Prs3d_Drawer) aDrawer = myViewCube()->Attributes();
	aDrawer->SetDatumAspect(new Prs3d_DatumAspect());  // 动态设置
	Handle(Prs3d_DatumAspect) aDatumAsp = aDrawer->DatumAspect();  // 获取 Prs3d_DatumAspect 对象指针
}

/// <summary>
/// 创建视图立方，并设置尺寸
/// </summary>
/// <param name="axesRadius">轴半径</param>
ViewCube::ViewCube(double size) :ViewCube() {
	//修改坐标轴文字颜色
	Handle(Prs3d_Drawer) aDrawer = myViewCube()->Attributes();
	aDrawer->SetDatumAspect(new Prs3d_DatumAspect());  // 动态设置
	Handle(Prs3d_DatumAspect) aDatumAsp = aDrawer->DatumAspect();  // 获取 Prs3d_DatumAspect 对象指针
	SetEqualScaling(size);
}

/// <summary>
/// 设置立方体尺寸
/// </summary>
/// <param name="size"></param>
void ViewCube::SetSize(double size)
{
	myViewCube()->SetSize(size);
}

/// <summary>
/// 设置轴半径
/// </summary>
/// <param name="radius"></param>
void ViewCube::SetAxesRadius(double radius)
{
	myViewCube()->SetAxesRadius(radius);
}

/// <summary>
/// 设置轴箭头圆角半径
/// </summary>
/// <param name="radius"></param>
void ViewCube::SetAxesConeRadius(double radius)
{
	myViewCube()->SetAxesConeRadius(radius);
}

/// <summary>
/// 设置轴原点圆球半径
/// </summary>
/// <param name="radius"></param>
void ViewCube::SetAxesSphereRadius(double radius)
{
	myViewCube()->SetAxesSphereRadius(radius);
}

/// <summary>
/// 设置轴标签颜色
/// </summary>
/// <param name="axisPart"></param>
/// <param name="color"></param>
void ViewCube::SetAxesTextColor(DatumParts axisPart, Extension::Color color)
{
	Handle(Prs3d_DatumAspect) aDatumAsp = myViewCube()->Attributes()->DatumAspect();  // 获取 Prs3d_DatumAspect 对象指针
	aDatumAsp->TextAspect(Prs3d_DatumParts(axisPart))->SetColor(color);
}

/// <summary>
/// 设置轴标签高度
/// </summary>
/// <param name="axisPart"></param>
/// <param name="height"></param>
void ViewCube::SetAxesTextHeight(DatumParts axisPart, double height)
{
	Handle(Prs3d_DatumAspect) aDatumAsp = myViewCube()->Attributes()->DatumAspect();  // 获取 Prs3d_DatumAspect 对象指针
	aDatumAsp->TextAspect(Prs3d_DatumParts(axisPart))->SetHeight(height);
}

/// <summary>
/// 设置轴颜色
/// </summary>
/// <param name="axisPart"></param>
/// <param name="color"></param>
void ViewCube::SetAxesColor(DatumParts axisPart, Extension::Color color)
{
	Handle(Prs3d_DatumAspect) aDatumAsp = myViewCube()->Attributes()->DatumAspect();  // 获取 Prs3d_DatumAspect 对象指针
	// 修改坐标轴颜色
	aDatumAsp->ShadingAspect(Prs3d_DatumParts(axisPart))->SetColor(color);
}

/// <summary>
/// 设置轴标签文字
/// </summary>
/// <param name="x"></param>
/// <param name="y"></param>
/// <param name="z"></param>
void ViewCube::SetAxesLabels(System::String^ x, System::String^ y, System::String^ z)
{
	myViewCube()->SetAxesLabels(DataExchange::ToAsciiString(x), DataExchange::ToAsciiString(y), DataExchange::ToAsciiString(z));
}

/// <summary>
/// 是否显示立方体描边
/// </summary>
/// <param name="isEnabled"></param>
void ViewCube::SetFaceBoundaryDraw(bool isEnabled)
{
	Handle(Prs3d_Drawer) aDrawer = myViewCube()->Attributes();
	aDrawer->SetFaceBoundaryDraw(isEnabled);
}

/// <summary>
/// 设置立方体描边颜色
/// </summary>
/// <param name="color"></param>
void ViewCube::SetFaceBoundaryColor(Extension::Color color)
{
	Handle(Prs3d_Drawer) aDrawer = myViewCube()->Attributes();
	aDrawer->FaceBoundaryAspect()->SetColor(color);
}

/// <summary>
/// 设置立方体标签
/// </summary>
/// <param name="boxSide"></param>
/// <param name="label"></param>
void ViewCube::SetBoxSideLabel(BoxSide boxSide, System::String^ label)
{
	myViewCube()->SetBoxSideLabel(V3d_TypeOfOrientation(boxSide), DataExchange::ConvertChineseToUnicode(label));
}

/// <summary>
/// 设置立方体表面字体
/// </summary>
/// <param name="fontFamilyName"></param>
void ViewCube::SetBoxSideFont(System::String^ fontFamilyName)
{
	myViewCube()->SetFont(DataExchange::ToAsciiString(fontFamilyName));
}

/// <summary>
/// 设置立方体标签字体大小
/// </summary>
/// <param name="height"></param>
void ViewCube::SetBoxSideTextHeight(double height)
{
	myViewCube()->SetFontHeight(height);
}

/// <summary>
/// 设置立方体标签颜色
/// </summary>
/// <param name="color"></param>
void ViewCube::SetBoxSideTextColor(Extension::Color color)
{
	myViewCube()->SetTextColor(color);
}

/// <summary>
/// 设置立方体表面颜色
/// </summary>
/// <param name="color"></param>
void ViewCube::SetBoxSideFaceColor(Extension::Color color)
{
	myViewCube()->SetBoxColor(color);
}

/// <summary>
/// 设置立方体透明度
/// </summary>
/// <param name="transparency"></param>
void ViewCube::SetTransparency(double transparency)
{
	myViewCube()->SetTransparency(transparency);
}

/// <summary>
/// 设置立方体面到边的距离
/// </summary>
/// <param name="extension"></param>
void ViewCube::SetBoxFacetExtension(double extension)
{
	myViewCube()->SetBoxFacetExtension(extension);
}

/// <summary>
/// 设置立方体圆角半径
/// </summary>
/// <param name="roundRadius"></param>
void ViewCube::SetRoundRadius(double roundRadius)
{
	myViewCube()->SetRoundRadius(roundRadius);
}

/// <summary>
/// 设置立方体边间空隙的可选择范围
/// </summary>
/// <param name="gap"></param>
void ViewCube::SetBoxEdgeGap(double gap)
{
	myViewCube()->SetBoxEdgeGap(gap);
}


void ViewCube::SetICODefault() {
	int R, G, B;
	//修改坐标轴文字颜色
	Handle(Prs3d_Drawer) aDrawer = myViewCube()->Attributes();
	aDrawer->SetDatumAspect(new Prs3d_DatumAspect());  // 动态设置
	Handle(Prs3d_DatumAspect) aDatumAsp = aDrawer->DatumAspect();  // 获取 Prs3d_DatumAspect 对象指针
	aDatumAsp->TextAspect(Prs3d_DP_XAxis)->SetColor(Quantity_NOC_RED2);
	aDatumAsp->TextAspect(Prs3d_DP_YAxis)->SetColor(Quantity_NOC_GREEN2);
	aDatumAsp->TextAspect(Prs3d_DP_ZAxis)->SetColor(Quantity_NOC_BLUE2);
	// 修改坐标轴颜色
	aDatumAsp->ShadingAspect(Prs3d_DP_XAxis)->SetColor(Quantity_NOC_RED2);
	aDatumAsp->ShadingAspect(Prs3d_DP_YAxis)->SetColor(Quantity_NOC_GREEN2);
	aDatumAsp->ShadingAspect(Prs3d_DP_ZAxis)->SetColor(Quantity_NOC_BLUE2);
	//设置描边和描边颜色
	aDrawer->SetFaceBoundaryDraw(true);
	aDrawer->SetFaceBoundaryAspect(new Prs3d_LineAspect(new Graphic3d_AspectLine3d()));
	R = 228;
	G = 144;
	B = 255;
	aDrawer->FaceBoundaryAspect()->SetColor(Quantity_Color(R / 255, G / 255, B / 255, Quantity_TOC_RGB));
	//设置立方体标签
	myViewCube()->SetBoxSideLabel(V3d_Yneg, DataExchange::ConvertChineseToUnicode("前"));
	myViewCube()->SetBoxSideLabel(V3d_Ypos, DataExchange::ConvertChineseToUnicode("后"));
	myViewCube()->SetBoxSideLabel(V3d_Xneg, DataExchange::ConvertChineseToUnicode("左"));
	myViewCube()->SetBoxSideLabel(V3d_Xpos, DataExchange::ConvertChineseToUnicode("右"));
	myViewCube()->SetBoxSideLabel(V3d_Zpos, DataExchange::ConvertChineseToUnicode("上"));
	myViewCube()->SetBoxSideLabel(V3d_Zneg, DataExchange::ConvertChineseToUnicode("下"));
	myViewCube()->SetFont("MSYHL");
	R = 241;
	G = 217;
	B = 37;
	myViewCube()->SetTextColor(Quantity_Color(R / 255, G / 255, B / 255, Quantity_TOC_RGB));
	// 设置立方体面颜色
	R = 0;
	G = 194;
	B = 168;
	myViewCube()->SetBoxColor(Quantity_Color(R / 255, G / 255, B / 255, Quantity_TOC_RGB));
	// 设置半透明立方体
	myViewCube()->SetTransparency(0.6);
	// 设置立方体面到边的距离
	myViewCube()->SetBoxFacetExtension(14);
	// 设置轴标签
	// myViewCube()->SetAxesLabels("x", "y", "Z")
	// 设置盒子圆角
	// myViewCube()->SetRoundRadius(0.1)
	// 设置立方体面到边间空隙的可选择范围
	// myViewCube()->SetBoxEdgeGap(0)
}

void ViewCube::SetEqualScaling(double axisSize) {
	myViewCube()->SetSize(axisSize * 30.0);
	// 设置坐标轴半径
	myViewCube()->SetAxesRadius(axisSize);
	myViewCube()->SetAxesConeRadius(axisSize * 1.5);
	myViewCube()->SetAxesSphereRadius(axisSize * 1.5);
	//修改坐标轴文字颜色
	Handle(Prs3d_Drawer) aDrawer = myViewCube()->Attributes();
	aDrawer->SetDatumAspect(new Prs3d_DatumAspect());  // 动态设置
	Handle(Prs3d_DatumAspect) aDatumAsp = aDrawer->DatumAspect();  // 获取 Prs3d_DatumAspect 对象指针
	aDatumAsp->TextAspect(Prs3d_DP_XAxis)->SetHeight(axisSize * 5);
	aDatumAsp->TextAspect(Prs3d_DP_YAxis)->SetHeight(axisSize * 5);
	aDatumAsp->TextAspect(Prs3d_DP_ZAxis)->SetHeight(axisSize * 5);
}
}
}
}