#include "ICO_View.h"
#include <V3d_View.hxx>

namespace OCCTK {
namespace OCC {
namespace V3d {

/// <summary>
/// 创建可视化窗口
/// </summary>
/// <param name=""></param>
/// <param name="theWnd">窗体句柄</param>
View::View(Handle(V3d_View) theView, System::IntPtr theWnd) {
	myView() = theView;
	// 创建Windows窗口，并设置为V3d_View的窗口
	Handle(WNT_Window) aWNTWindow = new WNT_Window(reinterpret_cast<HWND> (theWnd.ToPointer()));
	myView()->SetWindow(aWNTWindow);
}

/// <summary>
/// 创建无屏窗口
/// </summary>
/// <param name=""></param>
View::View(Handle(V3d_View) theView) {
	myView() = theView;
}

Handle(V3d_View) View::GetOCC() {
	return myView();
}

/// <summary>
/// 使用默认设置
/// </summary>
void View::SetDefault() {
	this->SetDefaultBGColor();
	this->SetDefaultRendering();
}

/// <summary>
/// 设置默认渲染参数
/// </summary>
void View::SetDefaultRendering() {
	if (myView().IsNull()) { return; }
	myView()->ChangeRenderingParams().Method = Graphic3d_RenderingMode::Graphic3d_RM_RASTERIZATION;
	//myView()->ChangeRenderingParams().RaytracingDepth = 3;
	myView()->ChangeRenderingParams().IsShadowEnabled = true;
	myView()->ChangeRenderingParams().IsReflectionEnabled = false;
	myView()->ChangeRenderingParams().IsAntialiasingEnabled = true;
	myView()->ChangeRenderingParams().IsTransparentShadowEnabled = true;
	//myView()->ChangeRenderingParams().RenderResolutionScale = 2;
	myView()->ChangeRenderingParams().NbMsaaSamples = 8;
	myView()->ChangeRenderingParams().CollectedStats = Graphic3d_RenderingParams::PerfCounters_NONE;
}

/// <summary>
/// 设置默认背景颜色
/// </summary>
void View::SetDefaultBGColor() {
	if (myView().IsNull()) { return; }
	myView()->SetBgGradientColors(Quantity_Color(37. / 255., 55. / 255., 113. / 255., Quantity_TOC_RGB), Quantity_Color(36. / 255., 151. / 255., 132. / 255., Quantity_TOC_RGB), Aspect_GradientFillMethod_Vertical, true);
}

/// <summary>
/// 设置视图背景颜色
/// </summary>
/// <param name="C1"></param>
/// <param name="C2"></param>
/// <param name="update"></param>
void View::SetBgGradientColors(Color^ C1, Color^ C2, bool update) {
	if (myView().IsNull()) { return; }
	myView()->SetBgGradientColors(C1->GetOCC(), C2->GetOCC(), Aspect_GradientFillMethod_Vertical, update);
}

/// <summary>
/// 获取当前缩放比例
/// </summary>
/// <returns></returns>
double View::GetScale() {
	if (myView().IsNull())  return -1.0;
	return myView()->Scale();
}

/// <summary>
/// 重绘
/// </summary>
void View::Redraw() {
	if (myView().IsNull())  return;
	myView()->Redraw();
}

/// <summary>
/// 调整画布大小
/// </summary>
void View::MustBeResized() {
	if (myView().IsNull())  return;
	myView()->MustBeResized();
}

/// <summary>
/// 设置隐藏线显示模式
/// </summary>
/// <param name="open"></param>
void View::SetDegenerateMode(bool open) {
	if (myView().IsNull())  return;
	myView()->SetComputedMode(open);
}

/// <summary>
/// 适应尺寸缩放
/// </summary>
/// <param name="theXmin"></param>
/// <param name="theYmin"></param>
/// <param name="theXmax"></param>
/// <param name="theYmax"></param>
void View::WindowFitAll(int theXmin, int theYmin, int theXmax, int theYmax) {
	if (myView().IsNull())  return;
	myView()->WindowFitAll(theXmin, theYmin, theXmax, theYmax);
}

/// <summary>
/// 根据可视对象缩放View
/// </summary>
/// <param name="theMargin"></param>
/// <param name="update"></param>
void View::FitAll(double theMargin, bool update) {
	if (myView().IsNull())  return;
	myView()->FitAll(theMargin, update);
}

/// <summary>
/// 根据可视对象缩放View
/// </summary>
/// <param name="theScaleFactor"></param>
void View::ZFitAll(double theScaleFactor) {
	if (myView().IsNull())  return;
	myView()->ZFitAll(theScaleFactor);
}

/// <summary>
/// 重置视图的居中和方向
/// </summary>
void View::Reset() {
	if (myView().IsNull())  return;
	myView()->Reset();
}

/// <summary>
/// 将 x,y 像素位置对应的视图点置于窗口中心，并更新视图。
/// </summary>
/// <param name="theX"></param>
/// <param name="theY"></param>
/// <param name="theZoomFactor"></param>
void View::Place(int theX, int theY, float theZoomFactor) {
	if (myView().IsNull())  return;
	myView()->Place(theX, theY, theZoomFactor);
}

/// <summary>
/// 根据两个位置之间的距离计算出的缩放系数，并缩放视图
/// </summary>
/// <param name="theX1"></param>
/// <param name="theY1"></param>
/// <param name="theX2"></param>
/// <param name="theY2"></param>
void View::Zoom(int theX1, int theY1, int theX2, int theY2) {
	if (myView().IsNull())  return;
	myView()->Zoom(theX1, theY1, theX2, theY2);
}

/// <summary>
/// 设置缩放比例
/// </summary>
/// <param name="theZoomFactor">缩放比例</param>
/// <param name="update">更新视图</param>
void View::SetZoom(double theZoomFactor, bool update) {
	if (myView().IsNull())  return;
	myView()->SetZoom(theZoomFactor, update);
}

/// <summary>
/// 记录开始缩放点
/// </summary>
/// <param name="startX"></param>
/// <param name="startY"></param>
void View::StartZoomAtPoint(double startX, double startY) {
	if (myView().IsNull())  return;
	myView()->StartZoomAtPoint(startX, startY);
}

/// <summary>
/// 点缩放(调用前必须调用 StartZoomAtPoint)
/// </summary>
/// <param name="startX"></param>
/// <param name="startY"></param>
/// <param name="endX"></param>
/// <param name="endY"></param>
void View::ZoomAtPoint(double startX, double startY, double endX, double endY) {
	if (myView().IsNull())  return;
	myView()->ZoomAtPoint(startX, startY, endX, endY);
}

// 平移
//void View::Pan(int theX, int theY) {
//	if (myView().IsNull())  return;
//	double theZoomFactor = 1.0;
//	myView()->Pan(theX, theY, theZoomFactor);
//}

/// <summary>
/// 记录开始平移点
/// </summary>
void View::StartPan() {
	if (myView().IsNull())  return;
	double theZoomFactor = 1.0;
	myView()->Pan(0, 0, theZoomFactor, true);
}

/// <summary>
/// 平移操作，需要传入计算后的相对X和Y值
/// </summary>
/// <param name="theDX"></param>
/// <param name="theDY"></param>
void View::Pan(int theDX, int theDY) {
	if (myView().IsNull())  return;
	double theZoomFactor = 1.0;
	myView()->Pan(theDX, theDY, theZoomFactor, false);
}

/// <summary>
/// 记录开始旋转点
/// </summary>
/// <param name="theX"></param>
/// <param name="theY"></param>
void View::StartRotation(int theX, int theY) {
	if (myView().IsNull())  return;
	myView()->StartRotation(theX, theY);
}


/// <summary>
/// 旋转(调用前必须调用 StartRotation)
/// </summary>
/// <param name="theX"></param>
/// <param name="theY"></param>
void View::Rotation(int theX, int theY) {
	if (myView().IsNull())  return;
	myView()->Rotation(theX, theY);
}
/// <summary>
/// 设置视图方向
/// </summary>
/// <param name="theOrientation"></param>
/// <param name="update"></param>
void View::SetViewOrientation(ViewOrientation theOrientation, bool update) {
	if (myView().IsNull())  return;
	myView()->SetProj(V3d_TypeOfOrientation((int)theOrientation), update);
}
/// <summary>
/// 显示带刻度的坐标轴
/// </summary>
void View::DisplayDefault_GraduatedTrihedron() {
	if (myView().IsNull()) return;
	myView()->GraduatedTrihedronDisplay(Graphic3d_GraduatedTrihedron());
}

}
}
}