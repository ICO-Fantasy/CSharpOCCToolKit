#include "ICO_ViewCube.h"
#include <AIS_ViewCube.hxx>
#include <Prs3d_DatumAspect.hxx>
#include <Standard_Handle.hxx>
//local
#include "..\DataExchange\ICO_StringExchange.h"
#include "ICO_DatumParts.h"
#include "ICO_ViewOrientation.h"
#include "ICO_Exception.h"

using namespace OCCTK::Extension;
using namespace OCCTK::OCC::V3d;

namespace OCCTK {
namespace OCC {
namespace AIS {

ViewCube::ViewCube() :InteractiveObject() {
    NativeHandle = new AIS_ViewCube(); SetDefault();
}

/// <summary>
/// 创建视图立方
/// </summary>
/// <param name="axesRadius">轴半径</param>
ViewCube::ViewCube(double axesRadius) :ViewCube() {
    myAxisSize = axesRadius;
    SetDefault();
}

void ViewCube::SetDefault() {
    int R, G, B;
    myViewCube()->SetSize(myAxisSize * 30.0);
    // 设置坐标轴半径
    myViewCube()->SetAxesRadius(myAxisSize);
    myViewCube()->SetAxesConeRadius(myAxisSize * 1.5);
    myViewCube()->SetAxesSphereRadius(myAxisSize * 1.5);

    Handle(Prs3d_Drawer) aDrawer = myViewCube()->Attributes();
    if (aDrawer->DatumAspect().IsNull()) {
        aDrawer->SetDatumAspect(new Prs3d_DatumAspect());  // 动态设置
    }
    Handle(Prs3d_DatumAspect) aDatumAsp = aDrawer->DatumAspect();  // 获取 Prs3d_DatumAspect 对象指针
    //修改坐标轴文字颜色
    aDatumAsp->TextAspect(Prs3d_DP_XAxis)->SetColor(Quantity_NOC_RED2);
    aDatumAsp->TextAspect(Prs3d_DP_YAxis)->SetColor(Quantity_NOC_GREEN2);
    aDatumAsp->TextAspect(Prs3d_DP_ZAxis)->SetColor(Quantity_NOC_BLUE2);
    //修改坐标轴文字高度
    aDatumAsp->TextAspect(Prs3d_DP_XAxis)->SetHeight(myAxisSize * 5);
    aDatumAsp->TextAspect(Prs3d_DP_YAxis)->SetHeight(myAxisSize * 5);
    aDatumAsp->TextAspect(Prs3d_DP_ZAxis)->SetHeight(myAxisSize * 5);
    // 修改坐标轴颜色
    aDatumAsp->ShadingAspect(Prs3d_DP_XAxis)->SetColor(Quantity_NOC_RED2);
    aDatumAsp->ShadingAspect(Prs3d_DP_YAxis)->SetColor(Quantity_NOC_GREEN2);
    aDatumAsp->ShadingAspect(Prs3d_DP_ZAxis)->SetColor(Quantity_NOC_BLUE2);
    //设置描边和描边颜色
    aDrawer->SetFaceBoundaryDraw(true);
    aDrawer->SetFaceBoundaryAspect(new Prs3d_LineAspect(new Graphic3d_AspectLine3d()));//必须创建一个实例
    R = 228;
    G = 144;
    B = 255;
    aDrawer->FaceBoundaryAspect()->SetColor(Quantity_Color(R / 255, G / 255, B / 255, Quantity_TOC_RGB));
    //设置立方体标签
    myViewCube()->SetBoxSideLabel(V3d_Xneg, DataExchange::ConvertChineseToUnicode("左"));
    myViewCube()->SetBoxSideLabel(V3d_Xpos, DataExchange::ConvertChineseToUnicode("右"));
    myViewCube()->SetBoxSideLabel(V3d_Yneg, DataExchange::ConvertChineseToUnicode("前"));
    myViewCube()->SetBoxSideLabel(V3d_Ypos, DataExchange::ConvertChineseToUnicode("后"));
    myViewCube()->SetBoxSideLabel(V3d_Zpos, DataExchange::ConvertChineseToUnicode("上"));
    myViewCube()->SetBoxSideLabel(V3d_Zneg, DataExchange::ConvertChineseToUnicode("下"));
    myViewCube()->SetFont("MSYHL");
    myViewCube()->SetFontHeight(myAxisSize * 15.0);
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

double ViewCube::AxesSize::get()
{
    return myViewCube()->Size();
}

void ViewCube::AxesSize::set(double value)
{
    myViewCube()->SetSize(value);
}

double ViewCube::AxesRadius::get()
{
    return myViewCube()->AxesRadius();
}

void ViewCube::AxesRadius::set(double value)
{
    myViewCube()->SetAxesRadius(value);
}

double ViewCube::AxesConeRadius::get()
{
    return myViewCube()->AxesConeRadius();
}

void ViewCube::AxesConeRadius::set(double value)
{
    myViewCube()->SetAxesConeRadius(value);
}

double ViewCube::AxesSphereRadius::get()
{
    return myViewCube()->AxesSphereRadius();
}

void ViewCube::AxesSphereRadius::set(double value)
{
    myViewCube()->SetAxesSphereRadius(value);
}

/// <summary>
/// 获取轴颜色
/// </summary>
/// <param name="axis"></param>
/// <returns></returns>
Color ViewCube::GettAxesColor(DatumParts axis)
{
    Handle(Prs3d_Drawer) aDrawer = myViewCube()->Attributes();
    if (aDrawer->DatumAspect().IsNull()) {
        aDrawer->SetDatumAspect(new Prs3d_DatumAspect());  // 动态设置
    }
    Handle(Prs3d_DatumAspect) aDatumAsp = aDrawer->DatumAspect();  // 获取 Prs3d_DatumAspect 对象指针
    return Color(aDatumAsp->ShadingAspect(Prs3d_DatumParts(axis))->Color());
}

/// <summary>
/// 设置轴颜色
/// </summary>
/// <param name="axis"></param>
/// <param name="color"></param>
void ViewCube::SetAxesColor(DatumParts axis, Color color)
{
    Handle(Prs3d_Drawer) aDrawer = myViewCube()->Attributes();
    if (aDrawer->DatumAspect().IsNull()) {
        aDrawer->SetDatumAspect(new Prs3d_DatumAspect());  // 动态设置
    }
    Handle(Prs3d_DatumAspect) aDatumAsp = aDrawer->DatumAspect();  // 获取 Prs3d_DatumAspect 对象指针
    // 修改坐标轴颜色
    aDatumAsp->ShadingAspect(Prs3d_DatumParts(axis))->SetColor(color);
}

/// <summary>
/// 设置轴文字颜色
/// </summary>
/// <param name="axis"></param>
/// <param name="color"></param>
void ViewCube::SetAxesTextColor(DatumParts axis, Color color)
{
    Handle(Prs3d_Drawer) aDrawer = myViewCube()->Attributes();
    if (aDrawer->DatumAspect().IsNull()) {
        aDrawer->SetDatumAspect(new Prs3d_DatumAspect());  // 动态设置
    }
    Handle(Prs3d_DatumAspect) aDatumAsp = aDrawer->DatumAspect();  // 获取 Prs3d_DatumAspect 对象指针
    //修改坐标轴文字颜色
    aDatumAsp->TextAspect(Prs3d_DatumParts(axis))->SetColor(color);
}

/// <summary>
/// 设置轴文字高度
/// </summary>
/// <param name="axis"></param>
/// <param name="height"></param>
void ViewCube::SetAxesTextHeight(DatumParts axis, double height) {
    Handle(Prs3d_Drawer) aDrawer = myViewCube()->Attributes();
    if (aDrawer->DatumAspect().IsNull()) {
        aDrawer->SetDatumAspect(new Prs3d_DatumAspect());  // 动态设置
    }
    Handle(Prs3d_DatumAspect) aDatumAsp = aDrawer->DatumAspect();  // 获取 Prs3d_DatumAspect 对象指针
    //修改坐标轴文字高度
    aDatumAsp->TextAspect(Prs3d_DatumParts(axis))->SetHeight(height);
}

/// <summary>
/// 获取轴文字高度
/// </summary>
/// <param name="axis"></param>
/// <returns></returns>
double ViewCube::GetAxesTextHeight(DatumParts axis)
{
    Handle(Prs3d_Drawer) aDrawer = myViewCube()->Attributes();
    if (aDrawer->DatumAspect().IsNull()) {
        aDrawer->SetDatumAspect(new Prs3d_DatumAspect());  // 动态设置
    }
    Handle(Prs3d_DatumAspect) aDatumAsp = aDrawer->DatumAspect();  // 获取 Prs3d_DatumAspect 对象指针
    //坐标轴文字高度
    return aDatumAsp->TextAspect(Prs3d_DatumParts(axis))->Height();
}

bool ViewCube::EnableFaceBoundary::get()
{
    Handle(Prs3d_Drawer) aDrawer = myViewCube()->Attributes();
    if (aDrawer->DatumAspect().IsNull()) {
        aDrawer->SetDatumAspect(new Prs3d_DatumAspect());  // 动态设置
    }
    //设置描边
    return aDrawer->FaceBoundaryDraw();
}

/// <summary>
/// 启用描边
/// </summary>
/// <param name="value"></param>
void ViewCube::EnableFaceBoundary::set(bool value)
{
    Handle(Prs3d_Drawer) aDrawer = myViewCube()->Attributes();
    //设置描边
    aDrawer->SetFaceBoundaryDraw(value);
}

/// <summary>
/// 设置描边颜色
/// </summary>
/// <param name="color"></param>
void ViewCube::SetFaceBoundaryColor(Extension::Color color)
{
    Handle(Prs3d_Drawer) aDrawer = myViewCube()->Attributes();
    if (!aDrawer->FaceBoundaryDraw())
    {
        return;
    }
    //设置描边
    aDrawer->FaceBoundaryAspect()->SetColor(color);
}

/// <summary>
/// 设置立方体标签
/// </summary>
/// <param name="side"></param>
/// <param name="label"></param>
void ViewCube::SetBoxSideLabel(V3d::ViewOrientation side, System::String^ label)
{
    if (side != V3d::ViewOrientation::V3d_Xneg ||
        side != V3d::ViewOrientation::V3d_Xpos ||
        side != V3d::ViewOrientation::V3d_Yneg ||
        side != V3d::ViewOrientation::V3d_Ypos ||
        side != V3d::ViewOrientation::V3d_Zpos ||
        side != V3d::ViewOrientation::V3d_Zneg)
    {
        return;
    }
    //设置立方体标签
    myViewCube()->SetBoxSideLabel(V3d_TypeOfOrientation(side), DataExchange::ConvertChineseToUnicode(label));
}

/// <summary>
/// 获取轴标签
/// </summary>
/// <param name="side"></param>
/// <returns></returns>
System::String^ ViewCube::GetBoxSideLabel(V3d::ViewOrientation side)
{
    if (side != V3d::ViewOrientation::V3d_Xneg ||
        side != V3d::ViewOrientation::V3d_Xpos ||
        side != V3d::ViewOrientation::V3d_Yneg ||
        side != V3d::ViewOrientation::V3d_Ypos ||
        side != V3d::ViewOrientation::V3d_Zpos ||
        side != V3d::ViewOrientation::V3d_Zneg)
    {
        return "";
    }
    return gcnew System::String(myViewCube()->BoxSideLabel(V3d_TypeOfOrientation(side)).ToCString());
}

/// <summary>
/// 设置轴标签
/// </summary>
/// <param name="texts"></param>
void ViewCube::SetAxesLabels(System::ValueTuple<System::String^, System::String^, System::String^> texts)
{
    myViewCube()->SetAxesLabels(DataExchange::ToAsciiString(texts.Item1), DataExchange::ToAsciiString(texts.Item2), DataExchange::ToAsciiString(texts.Item3));
}

/// <summary>
/// 获取轴标签
/// </summary>
/// <param name="axis"></param>
/// <returns></returns>
System::String^ ViewCube::GetAxisLabels(DatumParts axis)
{
    return gcnew System::String(myViewCube()->AxisLabel(Prs3d_DatumParts(axis)).ToCString());
}

System::String^ ViewCube::Font::get()
{
    return gcnew System::String(myViewCube()->Font().ToCString());
}

void ViewCube::Font::set(System::String^ value)
{
    try
    {
        myViewCube()->SetFont(DataExchange::ConvertChineseToUnicode(value));
    }
    CATCH_AND_THROW_OCC_EXCEPTIONS
}

double ViewCube::FontHeight::get()
{
    return myViewCube()->FontHeight();
}

void ViewCube::FontHeight::set(double value)
{
    myViewCube()->SetFontHeight(value);
}

Extension::Color ViewCube::BoxTextColor::get()
{
    return Extension::Color(myViewCube()->TextColor());
}

void ViewCube::BoxTextColor::set(Extension::Color value)
{
    myViewCube()->SetTextColor(value);
}

Extension::Color ViewCube::BoxColor::get()
{
    return Extension::Color(myViewCube()->BoxColor());
}

void ViewCube::BoxColor::set(Extension::Color value)
{
    myViewCube()->SetBoxColor(value);
}

double ViewCube::Transparency::get()
{
    return myViewCube()->Transparency();
}

void ViewCube::Transparency::set(double value)
{
    if (value < 0 || value>1)
    {
        return;
    }
    myViewCube()->SetTransparency(value);
}

double ViewCube::BoxFacetExtension::get()
{
    return myViewCube()->BoxFacetExtension();
}

void ViewCube::BoxFacetExtension::set(double value)
{
    myViewCube()->SetBoxFacetExtension(value);
}

double ViewCube::BoxRoundRadius::get()
{
    return myViewCube()->RoundRadius();;
}

void ViewCube::BoxRoundRadius::set(double value)
{
    if (value < 0 || value>0.5)
    {
        return;
    }
    myViewCube()->SetRoundRadius(value);
}

double ViewCube::BoxEdgeGap::get()
{
    return myViewCube()->BoxEdgeGap();
}

void ViewCube::BoxEdgeGap::set(double value)
{

    myViewCube()->SetBoxEdgeGap(value);
}

}
}
}
