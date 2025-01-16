#pragma once
#include <AIS_ViewCube.hxx>
//Local
#include "ICO_InteractiveObject.h"
#include "..\Extension\ICO_Color.h"

namespace OCCTK {
namespace OCC {
//前向声明
namespace V3d {
enum class ViewOrientation;
}
namespace AIS {

//前向声明
enum class DatumParts;

public ref class ViewCube :public InteractiveObject {
private:
    Handle(AIS_ViewCube) myViewCube() { return Handle(AIS_ViewCube)::DownCast(NativeHandle); }
public:
    ViewCube();
    ViewCube(double axesRadius);
    Handle(AIS_ViewCube) GetOCC() { return myViewCube(); };
public:
    /// <summary>
    /// 整体尺寸
    /// </summary>
    property double AxesSize {double get(); void  set(double value); }
    /// <summary>
    /// 坐标轴半径
    /// </summary>
    property double AxesRadius {double get(); void  set(double value); }
    /// <summary>
    /// 坐标轴锥体半径
    /// </summary>
    property double AxesConeRadius {double get(); void  set(double value); }
    /// <summary>
    /// 坐标轴圆球半径
    /// </summary>
    property double AxesSphereRadius {double get(); void  set(double value); }
    /// <summary>
    /// 绘制面的边框
    /// </summary>
    property bool EnableFaceBoundary {bool get(); void  set(bool value); }
    /// <summary>
    /// 字体
    /// </summary>
    property System::String^ Font {System::String^ get(); void  set(System::String^ value); }
    /// <summary>
    /// 文字高度
    /// </summary>
    property double FontHeight {double get(); void  set(double value); }
    /// <summary>
    /// 文字颜色
    /// </summary>
    property Extension::Color BoxTextColor {Extension::Color get(); void  set(Extension::Color value); }
    /// <summary>
    /// 立方体颜色
    /// </summary>
    property Extension::Color BoxColor {Extension::Color get(); void  set(Extension::Color value); }
    /// <summary>
    /// 立方体透明度
    /// </summary>
    property double Transparency {double get(); void  set(double value); }
    /// <summary>
    /// 立方体面到边的距离
    /// </summary>
    property double BoxFacetExtension {double get(); void  set(double value); }
    /// <summary>
    /// 盒子圆角半径
    /// </summary>
    property double BoxRoundRadius {double get(); void  set(double value); }
    /// <summary>
    /// 立方体面到边间空隙
    /// </summary>
    /// <remarks>
    /// 可选择范围
    /// </remarks>
    property double BoxEdgeGap {double get(); void  set(double value); }

    Extension::Color GettAxesColor(DatumParts axis);
    void SetAxesColor(DatumParts axis, Extension::Color color);
    void SetAxesTextColor(DatumParts axis, Extension::Color color);
    void SetAxesTextHeight(DatumParts axis, double height);
    double GetAxesTextHeight(DatumParts axis);
    void SetFaceBoundaryColor(Extension::Color color);
    void SetBoxSideLabel(V3d::ViewOrientation side, System::String^ label);
    System::String^ GetBoxSideLabel(V3d::ViewOrientation side);
    void SetAxesLabels(System::ValueTuple<System::String^, System::String^, System::String^> texts);
    System::String^ GetAxisLabels(DatumParts axis);
private:
    void SetDefault();
    double myAxisSize = 5.0;
};

}
}
}
