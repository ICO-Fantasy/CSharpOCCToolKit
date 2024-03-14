//CSharpViewer.cpp: 委托到 .NET 的 OCC 视图类
#include "CSharpViewer.h"
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
//Test
#include <BRepPrimAPI_MakeBox.hxx>
#include <AIS_Shape.hxx>
#include <Standard_Handle.hxx>
#include <AIS_ViewCube.hxx>
#include "WMakeSimpleClamp.h"
//local
#include "WAIS_ViewCube.h"
using namespace System;
using namespace OCCTK::OCC;
namespace OCCTK::Visualization
{
void CSharpViewer::InitOCCViewer(void) {
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
bool CSharpViewer::InitViewer(System::IntPtr theWnd) {
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
bool CSharpViewer::InitSecondViewer(System::IntPtr theSecondWnd) {
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
void CSharpViewer::SetDefaultLight() {
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
void CSharpViewer::SetDefaultRendering(Handle(V3d_View) aViewHandle) {
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
void CSharpViewer::SetDefaultHighlightStyle() {
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
void CSharpViewer::RedrawView() {
	if (!myMainView().IsNull())
	{
		myMainView()->Redraw();
	}
}
/// <summary>
/// 更新视图（重设视图尺寸）
/// </summary>
void CSharpViewer::UpdateView() {
	if (!myMainView().IsNull())
	{
		myMainView()->MustBeResized();
	}
}
void CSharpViewer::UpdateCurrentViewer(void) {
	if (!myAISContext().IsNull())
	{
		myAISContext()->UpdateCurrentViewer();
	}
}
/// <summary>
/// 适应尺寸缩放
/// </summary>
void CSharpViewer::FitArea(int theXmin, int theYmin, int theXmax, int theYmax) {
	if (!myMainView().IsNull())
	{
		myMainView()->WindowFitAll(theXmin, theYmin, theXmax, theYmax);
	}
}
/// <summary>
/// 根据可视对象缩放View
/// </summary>
void CSharpViewer::ZoomAllView(void) {
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
void CSharpViewer::Place(int theX, int theY, float theZoomFactor) {
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
void CSharpViewer::AreaZoom(int theX1, int theY1, int theX2, int theY2) {
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
void CSharpViewer::Zoom(double theZoomFactor, bool update) {
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
void CSharpViewer::Pan(int theX, int theY) {
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
void CSharpViewer::StartRotation(int theX, int theY) {
	if (!myMainView().IsNull())
	{
		myMainView()->StartRotation(theX, theY);
	}
}
/// 旋转(调用前必须调用 StartRotation)
void CSharpViewer::Rotation(int theX, int theY) {
	if (!myMainView().IsNull())
	{
		myMainView()->Rotation(theX, theY);
	}
}
/// 设置视图方向
void CSharpViewer::SetViewOrientation(int theOrientation) {
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
void CSharpViewer::Reset(void) {
	if (!myMainView().IsNull())
	{
		myMainView()->Reset();
	}
}
/// 获取当前缩放比例
float CSharpViewer::GetScale(void) {
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
void CSharpViewer::SetDegenerateMode(bool theMode) {
	if (!myMainView().IsNull())
	{
		myMainView()->SetComputedMode(theMode ? Standard_True : Standard_False);
		myMainView()->Redraw();
	}
}
// 设置显示模式
void CSharpViewer::SetDisplayMode(int theMode) {
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
void CSharpViewer::EraseObjects(void) {
	if (myAISContext().IsNull())
	{
		return;
	}

	myAISContext()->EraseSelected(Standard_False);
	myAISContext()->ClearSelected(Standard_True);
}
/// <summary>
/// 单选
/// </summary>
/// <param name=""></param>
void CSharpViewer::Select(void) {
	if (!myAISContext().IsNull())
	{
		myAISContext()->SelectDetected(AIS_SelectionScheme_Replace);//清除当前选择并选择检测到的对象
		myAISContext()->UpdateCurrentViewer();
	}
}
/// <summary>
/// 多选
/// </summary>
/// <param name=""></param>
void CSharpViewer::MultipleSelect(void) {
	if (!myAISContext().IsNull())
	{
		myAISContext()->SelectDetected(AIS_SelectionScheme_Add);//将检测到的对象添加到当前选择
		myAISContext()->UpdateCurrentViewer();
	}
}
/// <summary>
/// 异或多选
/// </summary>
/// <param name=""></param>
void CSharpViewer::XORSelect(void) {
	if (!myAISContext().IsNull())
	{
		myAISContext()->SelectDetected(AIS_SelectionScheme_XOR);//对检测到的对象执行异或，再次点击可以取消选择
		myAISContext()->UpdateCurrentViewer();
	}
}
/// <summary>
/// 框选
/// </summary>
void CSharpViewer::AreaSelect(int theX1, int theY1, int theX2, int theY2) {
	if (!myAISContext().IsNull())
	{
		myAISContext()->SelectRectangle(Graphic3d_Vec2i(theX1, theY1),
			Graphic3d_Vec2i(theX2, theY2),
			myMainView());
		myAISContext()->UpdateCurrentViewer();
	}
}
/// <summary>
/// 框选多选
/// </summary>
void CSharpViewer::MultipleAreaSelect(int theX1, int theY1, int theX2, int theY2) {
	if ((!myAISContext().IsNull()) && (!myMainView().IsNull()))
	{
		myAISContext()->SelectRectangle(Graphic3d_Vec2i(theX1, theY1),
			Graphic3d_Vec2i(theX2, theY2),
			myMainView(),
			AIS_SelectionScheme_Add);
		myAISContext()->UpdateCurrentViewer();
	}
}
/// <summary>
/// 异或框选多选
/// </summary>
void CSharpViewer::XORAreaSelect(int theX1, int theY1, int theX2, int theY2) {
	if ((!myAISContext().IsNull()) && (!myMainView().IsNull()))
	{
		myAISContext()->SelectRectangle(Graphic3d_Vec2i(theX1, theY1),
			Graphic3d_Vec2i(theX2, theY2),
			myMainView(),
			AIS_SelectionScheme_XOR);
		myAISContext()->UpdateCurrentViewer();
	}
}
void CSharpViewer::Display(Handle(AIS_InteractiveObject)& theAISObject, bool theToUpdateViewer) {
	myAISContext()->Display(theAISObject, theToUpdateViewer);
}
//void CSharpViewer::Display(AIS_Shape* theAISObject, bool theToUpdateViewer) {
//	myAISContext()->Display(theAISObject, theToUpdateViewer);
//}
void CSharpViewer::DisplayOriginTrihedron() {

}
#pragma endregion
#pragma region 持久变换对象
void CSharpViewer::DisplayViewCube(float axisSize) {
	//todo 判断是否存在ViewCube
	//if (true)
	//{
	//}
	AIS::ViewCube^ aViewCube = gcnew AIS::ViewCube(axisSize);
	Handle(AIS_ViewCube)* ViewCube = aViewCube->GetOCC();
	myAISContext()->Display(*ViewCube, true);
}
void CSharpViewer::DisplayGrid(bool theFlag) {
	if (theFlag)
		myViewer()->ActivateGrid(Aspect_GT_Rectangular, Aspect_GDM_Lines);
	else
		myViewer()->DeactivateGrid();
}
void CSharpViewer::DisplayViewTrihedron(float axisSize) {

}
#pragma endregion
void CSharpViewer::Test(void) {
	TopoDS_Shape aShape = BRepPrimAPI_MakeBox(10, 10, 10).Shape();
	Handle(AIS_Shape) anAIS = new AIS_Shape(aShape);
	myAISContext()->Display(anAIS, true);
}
void CSharpViewer::MakeClampTest(double theX, double theY, double theZ, double OffsetZ, double BasePlateOffsetX, double BasePlateOffsetY, double BasePlateThickness, double BasePlateLengthX, double BasePlateLengthY, double VerticalPlateThickness, double VerticalPlateInitialOffsetX, double VerticalPlateOffsetX, double VerticalPlateInitialOffsetY, double VerticalPlateOffsetY, double VerticalPlateConnectionHeight, double VerticalPlateClearances, double VerticalPlateMinSupportingLen, double VerticalPlateCuttingDistance) {
	OCCTK::Laser::WMakeSimpleClamp^ aMaker = gcnew OCCTK::Laser::WMakeSimpleClamp();
	TopoDS_Shape BasePlate, InputFace, InputWorkpiece;
	aMaker->MakeBasePlate(BasePlate, InputFace, InputWorkpiece, theX, theY, theZ, BasePlateLengthX, BasePlateLengthY, OffsetZ, BasePlateOffsetX, BasePlateOffsetY, BasePlateThickness);
	Handle(AIS_Shape) anAISBasePlate = new AIS_Shape(BasePlate);
	Handle(AIS_Shape) anAISInputFace = new AIS_Shape(InputFace);
	Handle(AIS_Shape) anAISInputWorkpiece = new AIS_Shape(InputWorkpiece);
	anAISInputWorkpiece->SetColor(Quantity_NOC_GRAY);
	anAISInputWorkpiece->SetTransparency(0.8);
	anAISInputFace->SetColor(Quantity_NOC_BLUE);
	anAISInputFace->SetTransparency(0.8);
	anAISBasePlate->SetTransparency(0.8);
	myAISContext()->Display(anAISInputWorkpiece, true);
	myAISContext()->Display(anAISInputFace, true);
	myAISContext()->Display(anAISBasePlate, true);
	std::vector<TopoDS_Shape> VerticalPlates;
	aMaker->MakeVerticalPlate(VerticalPlates, InputWorkpiece, theX, theY, theZ, BasePlateLengthX, BasePlateLengthY, VerticalPlateThickness, VerticalPlateInitialOffsetX, VerticalPlateOffsetX, VerticalPlateInitialOffsetY, VerticalPlateOffsetY, VerticalPlateConnectionHeight, VerticalPlateClearances, VerticalPlateMinSupportingLen, VerticalPlateCuttingDistance);
	for each (auto aSection in VerticalPlates)
	{
		Handle(AIS_Shape) anAISSection = new AIS_Shape(aSection);
		anAISSection->SetTransparency();
		anAISSection->SetColor(Quantity_NOC_GREEN);
		myAISContext()->Display(anAISSection, true);
	}
}

}
