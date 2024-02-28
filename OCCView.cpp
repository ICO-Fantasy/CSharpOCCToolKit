//OCCView.cpp: 委托到 .NET 的 OCC 视图类
#include "OCCView.h"
// OCC所需头文件
#include <Standard_Version.hxx>
#include <Message_ProgressIndicator.hxx>
#include <Message_ProgressScope.hxx>
//OCC图形显示
#include <Aspect_DisplayConnection.hxx>
#include <WNT_Window.hxx>
#include <OpenGl_GraphicDriver.hxx>
//视图控制
#include <V3d_Viewer.hxx>
#include <V3d_View.hxx>
#include <AIS_InteractiveContext.hxx>
//包装C++类到托管类
#include <NCollection_Haft.h> 
//帮助使用托管扩展到 C++ 的辅助代码
#include <vcclr.h> 
//光源
#include <V3d_DirectionalLight.hxx>
#include <V3d_AmbientLight.hxx>
//
#include <Prs3d_DatumAspect.hxx>
//Test
#include <BRepPrimAPI_MakeBox.hxx>
#include <AIS_Shape.hxx>
#include <Standard_Handle.hxx>
#include <AIS_ViewCube.hxx>
#include "ViewCube.h"
using namespace System;
namespace OCCTK::Visualization
{
void OCCView::InitOCCView(void) {
	myGraphicDriver() = NULL;
	myViewer() = NULL;
	myMainView() = NULL;
	mySecondView() = NULL;
	myAISContext() = NULL;
}
/// <summary>
/// 初始化Viewer
/// </summary>
/// <param name="theWnd"></param>
/// <returns></returns>
bool OCCView::InitViewer(System::IntPtr theWnd)
{
	//创建OpenGL引擎
	try
	{
		Handle(Aspect_DisplayConnection) aDisplayConnection;
		myGraphicDriver() = new OpenGl_GraphicDriver(aDisplayConnection);
	}
	catch (Standard_Failure)
	{
		return false;
	}

	//创建V3d_Viewer
	myViewer() = new V3d_Viewer(myGraphicDriver());
	// 设置自定义光照
	this->SetDefaultLight();
	//创建V3d_View
	myMainView() = myViewer()->CreateView();
	//创建Windows窗口，并设置为V3d_View的窗口
	Handle(WNT_Window) aWNTWindow = new WNT_Window(reinterpret_cast<HWND> (theWnd.ToPointer()));
	myMainView()->SetWindow(aWNTWindow);
	//设置渲染信息
	this->SetDefaultRendering(myMainView());
	if (!aWNTWindow->IsMapped())
	{
		aWNTWindow->Map();
	}

	//创建AIS_InteractiveContext交互对象管理器
	myAISContext() = new AIS_InteractiveContext(myViewer());
	//设置高亮模式
	this->SetDefaultHighlightStyle();
	//设置自动高亮
	myAISContext()->SetAutomaticHilight(true);
	//不显示面中间ISO线
	myAISContext()->SetIsoNumber(0);
	//更新画布
	myAISContext()->UpdateCurrentViewer();
	myMainView()->Redraw();
	myMainView()->MustBeResized();
	return true;
}
bool OCCView::InitSecondViewer(System::IntPtr theSecondWnd) {
	//todo需要增加判断是否创建了myMainView
	//if (true)
	//{
	// 
	//}
	mySecondView() = myViewer()->CreateView();
	Handle(WNT_Window) secondWNTWindow = new WNT_Window(reinterpret_cast<HWND> (theSecondWnd.ToPointer()));
	mySecondView()->SetWindow(secondWNTWindow);
	this->SetDefaultRendering(mySecondView());
	if (!secondWNTWindow->IsMapped())
	{
		secondWNTWindow->Map();
	}
	mySecondView()->Redraw();
	mySecondView()->MustBeResized();
	return true;
}
#pragma region 视图设置
/// <summary>
/// 设置默认光照
/// </summary>
void OCCView::SetDefaultLight() {
	Handle(V3d_DirectionalLight) aDirLight = new V3d_DirectionalLight(V3d_XposYnegZpos, Quantity_NOC_WHITE);
	aDirLight->SetName("HeadLight");
	aDirLight->SetHeadlight(true);
	Handle(V3d_AmbientLight) anAmbLight = new V3d_AmbientLight(Quantity_NOC_WHITE);
	anAmbLight->SetName("AmbLight");
	//打开光源
	myViewer()->SetLightOn(aDirLight);
	myViewer()->SetLightOn(anAmbLight);
}
/// <summary>
/// 设置默认渲染参数
/// </summary>
void OCCView::SetDefaultRendering(Handle(V3d_View) aViewHandle) {
	aViewHandle->ChangeRenderingParams().Method = Graphic3d_RenderingMode::Graphic3d_RM_RASTERIZATION;
	//myView()->ChangeRenderingParams().RaytracingDepth = 3;
	aViewHandle->ChangeRenderingParams().IsShadowEnabled = true;
	aViewHandle->ChangeRenderingParams().IsReflectionEnabled = false;
	aViewHandle->ChangeRenderingParams().IsAntialiasingEnabled = true;
	aViewHandle->ChangeRenderingParams().IsTransparentShadowEnabled = true;
	//myView()->ChangeRenderingParams().RenderResolutionScale = 2;
	aViewHandle->ChangeRenderingParams().NbMsaaSamples = 8;
	aViewHandle->ChangeRenderingParams().CollectedStats = Graphic3d_RenderingParams::PerfCounters_NONE;
}
void OCCView::SetDefaultHighlightStyle() {
	//整个对象被选中
	Handle(Prs3d_Drawer) all_selected = myAISContext()->HighlightStyle(Prs3d_TypeOfHighlight_Selected);
	all_selected->SetMethod(Aspect_TOHM_COLOR);
	all_selected->SetColor(Quantity_NOC_LIGHTSEAGREEN);
	all_selected->SetDisplayMode(1);
	all_selected->SetTransparency(0.2f);
	//对象的一部分被选择
	Handle(Prs3d_Drawer) part_selected = myAISContext()->HighlightStyle(Prs3d_TypeOfHighlight_LocalSelected);
	part_selected->SetMethod(Aspect_TOHM_COLOR);
	part_selected->SetColor(Quantity_NOC_LIGHTSEAGREEN);
	part_selected->SetDisplayMode(1);
	part_selected->SetTransparency(0.2f);
}
#pragma endregion
#pragma region 视图控制
/// <summary>
/// 重绘View
/// </summary>
/// <param name=""></param>
void OCCView::RedrawView() {
	if (!myMainView().IsNull())
	{
		myMainView()->Redraw();
	}
}
/// <summary>
/// 更新视图（重设视图尺寸）
/// </summary>
void OCCView::UpdateView() {
	if (!myMainView().IsNull())
	{
		myMainView()->MustBeResized();
	}
}
void OCCView::UpdateCurrentViewer(void)
{
	if (!myAISContext().IsNull())
	{
		myAISContext()->UpdateCurrentViewer();
	}
}
/// <summary>
/// 适应尺寸缩放
/// </summary>
void OCCView::FitArea(int theXmin, int theYmin, int theXmax, int theYmax)
{
	if (!myMainView().IsNull())
	{
		myMainView()->WindowFitAll(theXmin, theYmin, theXmax, theYmax);
	}
}
/// <summary>
/// 根据可视对象缩放View
/// </summary>
void OCCView::ZoomAllView(void)
{
	if (!myMainView().IsNull())
	{
		myMainView()->FitAll();
		myMainView()->ZFitAll();
	}
}
/// <summary>
/// 将 x,y 像素位置对应的视图点置于窗口中心，并更新视图。
/// </summary>
/// <param name="theX"></param>
/// <param name="theY"></param>
/// <param name="theZoomFactor"></param>
void OCCView::Place(int theX, int theY, float theZoomFactor)
{
	Standard_Real aZoomFactor = theZoomFactor;
	if (!myMainView().IsNull())
	{
		myMainView()->Place(theX, theY, aZoomFactor);
	}
}
/// <summary>
/// 根据两个位置之间的距离计算出的缩放系数缩放视图
/// </summary>
/// <param name="theX1"></param>
/// <param name="theY1"></param>
/// <param name="theX2"></param>
/// <param name="theY2"></param>
void OCCView::AreaZoom(int theX1, int theY1, int theX2, int theY2)
{
	if (!myMainView().IsNull())
	{
		myMainView()->Zoom(theX1, theY1, theX2, theY2);
	}
}
/// <summary>
/// 缩放
/// </summary>
/// <param name="theZoomFactor"></param>
/// <param name="update"></param>
void OCCView::Zoom(double theZoomFactor, bool update)
{
	Standard_Real aZoomFactor = theZoomFactor;
	if (!myMainView().IsNull())
	{
		myMainView()->SetZoom(aZoomFactor, update);
	}
}
/// <summary>
/// 平移
/// </summary>
/// <param name="theX"></param>
/// <param name="theY"></param>
void OCCView::Pan(int theX, int theY)
{
	if (!myMainView().IsNull())
	{
		myMainView()->Pan(theX, theY);
	}
}
/// <summary>
/// 设置开始旋转
/// </summary>
/// <param name="theX"></param>
/// <param name="theY"></param>
void OCCView::StartRotation(int theX, int theY)
{
	if (!myMainView().IsNull())
	{
		myMainView()->StartRotation(theX, theY);
	}
}
/// 旋转(调用前必须调用 StartRotation)
void OCCView::Rotation(int theX, int theY)
{
	if (!myMainView().IsNull())
	{
		myMainView()->Rotation(theX, theY);
	}
}
/// 设置视图方向
void OCCView::SetViewOrientation(int theOrientation)
{
	///以下内容需要和C#中定义的枚举顺序保持一致
	/// Axo,
	/// Front,
	/// Back,
	/// Top,
	/// Bottom,
	/// Left,
	/// Right,
	if (myMainView().IsNull())
	{
		return;
	}
	switch (theOrientation)
	{
	case 0:
		myMainView()->SetProj(V3d_XposYnegZpos); // Axo
		break;
	case 1:
		myMainView()->SetProj(V3d_Yneg); // Front
		break;
	case 2:
		myMainView()->SetProj(V3d_Ypos); // Back
		break;
	case 3:
		myMainView()->SetProj(V3d_Zpos); // Top
		break;
	case 4:
		myMainView()->SetProj(V3d_Zneg); // Bottom
		break;
	case 5:
		myMainView()->SetProj(V3d_Xneg); // Left
		break;
	case 6:
		myMainView()->SetProj(V3d_Xpos); // Right
		break;
	default:
		break;
	}
}
/// 重置视图的居中和方向
void OCCView::Reset(void)
{
	if (!myMainView().IsNull())
	{
		myMainView()->Reset();
	}
}
/// 获取当前缩放比例
float OCCView::GetScale(void)
{
	if (myMainView().IsNull())
	{
		return -1;
	}
	else
	{
		return (float)myMainView()->Scale();
	}
}
#pragma endregion
#pragma region 显示模式
// 设置隐藏线显示模式
void OCCView::SetDegenerateMode(bool theMode)
{
	if (!myMainView().IsNull())
	{
		myMainView()->SetComputedMode(theMode ? Standard_True : Standard_False);
		myMainView()->Redraw();
	}
}
// 设置显示模式
void OCCView::SetDisplayMode(int theMode)
{
	if (myAISContext().IsNull())
	{
		return;
	}
	AIS_DisplayMode aCurrentMode;
	if (theMode == 0)
	{
		aCurrentMode = AIS_WireFrame;
	}
	else
	{
		aCurrentMode = AIS_Shaded;
	}

	if (myAISContext()->NbSelected() == 0)
	{
		myAISContext()->SetDisplayMode(aCurrentMode, Standard_False);
	}
	else
	{
		for (myAISContext()->InitSelected(); myAISContext()->MoreSelected(); myAISContext()->NextSelected())
		{
			myAISContext()->SetDisplayMode(myAISContext()->SelectedInteractive(), theMode, Standard_False);
		}
	}
	myAISContext()->UpdateCurrentViewer();
}
#pragma endregion
#pragma region 交互对象管理
/// <summary>
/// 删除选中的物体（首先需要选中物体）
/// </summary>
/// <param name=""></param>
void OCCView::EraseObjects(void)
{
	if (myAISContext().IsNull())
	{
		return;
	}

	myAISContext()->EraseSelected(Standard_False);
	myAISContext()->ClearSelected(Standard_True);
}
/// <summary>
/// 框选
/// </summary>
void OCCView::Select(int theX1, int theY1, int theX2, int theY2)
{
	if (!myAISContext().IsNull())
	{
		myAISContext()->SelectRectangle(Graphic3d_Vec2i(theX1, theY1),
			Graphic3d_Vec2i(theX2, theY2),
			myMainView());
		myAISContext()->UpdateCurrentViewer();
	}
}
/// 选择
void OCCView::Select(void)
{
	if (!myAISContext().IsNull())
	{
		myAISContext()->SelectDetected();
		myAISContext()->UpdateCurrentViewer();
	}
}
/// <summary>
/// 继续框选
/// </summary>
void OCCView::ShiftSelect(int theX1, int theY1, int theX2, int theY2)
{
	if ((!myAISContext().IsNull()) && (!myMainView().IsNull()))
	{
		myAISContext()->SelectRectangle(Graphic3d_Vec2i(theX1, theY1),
			Graphic3d_Vec2i(theX2, theY2),
			myMainView(),
			AIS_SelectionScheme_XOR);
		myAISContext()->UpdateCurrentViewer();
	}
}
/// 继续选择
void OCCView::ShiftSelect(void)
{
	if (!myAISContext().IsNull())
	{
		myAISContext()->SelectDetected(AIS_SelectionScheme_XOR);
		myAISContext()->UpdateCurrentViewer();
	}
}
void OCCView::Display(AIS_InteractiveObject* theAISObject, bool theToUpdateViewer) {
	myAISContext()->Display(theAISObject, theToUpdateViewer);
}
void OCCView::Test(void)
{
	TopoDS_Shape aShape = BRepPrimAPI_MakeBox(10, 10, 10).Shape();
	Handle(AIS_Shape) anAIS = new AIS_Shape(aShape);
	myAISContext()->Display(anAIS, true);
}
void OCCView::DisplayOriginTrihedron() {

}
#pragma endregion
#pragma region 持久变换对象
void OCCView::DisplayViewTrihedron(float axisSize) {
	//todo 判断是否存在ViewCube
	//if (true)
	//{
	//}
	ViewCube^ aViewCube = gcnew ViewCube(axisSize);
	Handle(AIS_ViewCube)* ViewCube = aViewCube->GetOCC();
	myAISContext()->Display(*ViewCube, true);

}
#pragma endregion

}
