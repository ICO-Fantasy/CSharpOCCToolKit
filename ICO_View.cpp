#include "ICO_View.h"
#include <V3d_View.hxx>

namespace OCCTK {
namespace OCC {
namespace V3d {

//可视化窗口
View::View(Handle(V3d_View) theView, System::IntPtr theWnd) {
	myView() = theView;
	// 创建Windows窗口，并设置为V3d_View的窗口
	Handle(WNT_Window) aWNTWindow = new WNT_Window(reinterpret_cast<HWND> (theWnd.ToPointer()));
	myView()->SetWindow(aWNTWindow);
}
//无屏窗口
View::View(Handle(V3d_View) theView) {
	myView() = theView;
}

Handle(V3d_View) View::GetOCC() {
	return myView();
}

//使用默认设置
void View::SetDefault() {
	this->SetDefaultBGColor();
	this->SetDefaultRendering();
}

// 设置默认渲染参数
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

//设置默认背景颜色
void View::SetDefaultBGColor() {
	if (myView().IsNull()) { return; }
	myView()->SetBgGradientColors(Quantity_Color(37. / 255., 55. / 255., 113. / 255., Quantity_TOC_RGB), Quantity_Color(36. / 255., 151. / 255., 132. / 255., Quantity_TOC_RGB), Aspect_GradientFillMethod_Vertical, true);
}

// 设置视图背景颜色
void View::SetBgGradientColors(Color^ C1, Color^ C2, bool update) {
	if (myView().IsNull()) { return; }
	myView()->SetBgGradientColors(C1->GetOCC(), C2->GetOCC(), Aspect_GradientFillMethod_Vertical, update);
}

//获取当前缩放比例
double View::GetScale() {
	if (myView().IsNull())  return 0.0;
	return myView()->Scale();
}

//重绘
void View::Redraw() {
	if (myView().IsNull())  return;
	myView()->Redraw();
}

//调整画布大小
void View::MustBeResized() {
	if (myView().IsNull())  return;
	myView()->MustBeResized();
}

//设置隐藏线显示模式
void View::SetDegenerateMode(bool open) {
	if (myView().IsNull())  return;
	myView()->SetComputedMode(open);
}

//适应尺寸缩放
void View::WindowFitAll(int theXmin, int theYmin, int theXmax, int theYmax) {
	if (myView().IsNull())  return;
	myView()->WindowFitAll(theXmin, theYmin, theXmax, theYmax);
}

//根据可视对象缩放View
void View::FitAll(double theMargin, bool update) {
	if (myView().IsNull())  return;
	myView()->FitAll(theMargin, update);
}
//根据可视对象缩放View
void View::ZFitAll(double theScaleFactor) {
	if (myView().IsNull())  return;
	myView()->ZFitAll(theScaleFactor);
}
//重置视图的居中和方向
void View::Reset() {
	if (myView().IsNull())  return;
	myView()->Reset();
}

//将 x,y 像素位置对应的视图点置于窗口中心，并更新视图。
void View::Place(int theX, int theY, float theZoomFactor) {
	if (myView().IsNull())  return;
	myView()->Place(theX, theY, theZoomFactor);
}

//根据两个位置之间的距离计算出的缩放系数，并缩放视图
void View::Zoom(int theX1, int theY1, int theX2, int theY2) {
	if (myView().IsNull())  return;
	myView()->Zoom(theX1, theY1, theX2, theY2);
}

//设置缩放比例
void View::SetZoom(double theZoomFactor, bool update) {
	if (myView().IsNull())  return;
	myView()->SetZoom(theZoomFactor, update);
}

//开始点缩放
void View::StartZoomAtPoint(double startX, double startY) {
	if (myView().IsNull())  return;
	myView()->StartZoomAtPoint(startX, startY);
}

//点缩放(调用前必须调用 StartZoomAtPoint)
void View::ZoomAtPoint(double startX, double startY, double endX, double endY) {
	if (myView().IsNull())  return;
	myView()->ZoomAtPoint(startX, startY, endX, endY);
}

////平移
//void View::Pan(int theX, int theY) {
//	if (myView().IsNull())  return;
//	double theZoomFactor = 1.0;
//	myView()->Pan(theX, theY, theZoomFactor);
//}

//记录开始平移点
void View::StartPan() {
	if (myView().IsNull())  return;
	double theZoomFactor = 1.0;
	myView()->Pan(0, 0, theZoomFactor, true);
}

//平移操作，需要传入计算后的相对X和Y值
void View::Pan(int theDX, int theDY) {
	if (myView().IsNull())  return;
	double theZoomFactor = 1.0;
	myView()->Pan(theDX, theDY, theZoomFactor, false);
}

//开始旋转
void View::StartRotation(int theX, int theY) {
	if (myView().IsNull())  return;
	myView()->StartRotation(theX, theY);
}


//旋转(调用前必须调用 StartRotation)
void View::Rotation(int theX, int theY) {
	if (myView().IsNull())  return;
	myView()->Rotation(theX, theY);
}

void View::SetViewOrientation(ViewOrientation theOrientation, bool update) {
	if (myView().IsNull())  return;
	myView()->SetProj(V3d_TypeOfOrientation((int)theOrientation), update);
}

void View::DisplayDefault_GraduatedTrihedron() {
	if (myView().IsNull()) return;
	// 显示带刻度的坐标轴
	myView()->GraduatedTrihedronDisplay(Graphic3d_GraduatedTrihedron());
}

}
}
}