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
//Test
#include <BRepPrimAPI_MakeBox.hxx>
#include <AIS_Shape.hxx>
#include <Standard_Handle.hxx>
using namespace System;

namespace OCCTK::Visualization
{
    void OCCView::InitOCCView(void){
        myGraphicDriver() = NULL;
        myViewer() = NULL;
        myView() = NULL;
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
        catch(Standard_Failure)
        {
            return false;
        }

        //创建V3d_Viewer
        myViewer() = new V3d_Viewer(myGraphicDriver());
        //设置并打开默认光照
        myViewer()->SetDefaultLights();
        myViewer()->SetLightOn();
        //创建V3d_View
        myView() = myViewer()->CreateView();
        //创建Windows窗口，并设置为V3d_View的窗口
        Handle(WNT_Window) aWNTWindow = new WNT_Window(reinterpret_cast<HWND> (theWnd.ToPointer()));
        myView()->SetWindow(aWNTWindow);
        if(!aWNTWindow->IsMapped())
        {
            aWNTWindow->Map();
        }
        //创建AIS_InteractiveContext交互对象管理器
        myAISContext() = new AIS_InteractiveContext(myViewer());
        //更新画布
        myAISContext()->UpdateCurrentViewer();
        myView()->Redraw();
        myView()->MustBeResized();
        return true;
    }
#pragma region 视图控制
    /// <summary>
    /// 重绘View
    /// </summary>
    /// <param name=""></param>
    void OCCView::RedrawView(){
        if(!myView().IsNull())
        {
            myView()->Redraw();
        }
    }
    /// <summary>
    /// 更新视图（重设视图尺寸）
    /// </summary>
    void OCCView::UpdateView(){
        if(!myView().IsNull())
        {
            myView()->MustBeResized();
        }
    }
    void OCCView::UpdateCurrentViewer(void)
    {
        if(!myAISContext().IsNull())
        {
            myAISContext()->UpdateCurrentViewer();
        }
    }
    /// <summary>
    /// 适应尺寸缩放
    /// </summary>
    void OCCView::FitArea(int theXmin, int theYmin, int theXmax, int theYmax)
    {
        if(!myView().IsNull())
        {
            myView()->WindowFitAll(theXmin, theYmin, theXmax, theYmax);
        }
    }
    /// <summary>
    /// 根据可视对象缩放View
    /// </summary>
    void OCCView::ZoomAllView(void)
    {
        if(!myView().IsNull())
        {
            myView()->FitAll();
            myView()->ZFitAll();
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
        if(!myView().IsNull())
        {
            myView()->Place(theX, theY, aZoomFactor);
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
        if(!myView().IsNull())
        {
            myView()->Zoom(theX1, theY1, theX2, theY2);
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
        if(!myView().IsNull())
        {
            myView()->SetZoom(aZoomFactor, update);
        }
    }
    /// <summary>
    /// 平移
    /// </summary>
    /// <param name="theX"></param>
    /// <param name="theY"></param>
    void OCCView::Pan(int theX, int theY)
    {
        if(!myView().IsNull())
        {
            myView()->Pan(theX, theY);
        }
    }
    /// <summary>
    /// 设置开始旋转
    /// </summary>
    /// <param name="theX"></param>
    /// <param name="theY"></param>
    void OCCView::StartRotation(int theX, int theY)
    {
        if(!myView().IsNull())
        {
            myView()->StartRotation(theX, theY);
        }
    }
    /// 旋转(调用前必须调用 StartRotation)
    void OCCView::Rotation(int theX, int theY)
    {
        if(!myView().IsNull())
        {
            myView()->Rotation(theX, theY);
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
        if(myView().IsNull())
        {
            return;
        }
        switch(theOrientation)
        {
        case 0:
            myView()->SetProj(V3d_XposYnegZpos); // Axo
            break;
        case 1:
            myView()->SetProj(V3d_Yneg); // Front
            break;
        case 2:
            myView()->SetProj(V3d_Ypos); // Back
            break;
        case 3:
            myView()->SetProj(V3d_Zpos); // Top
            break;
        case 4:
            myView()->SetProj(V3d_Zneg); // Bottom
            break;
        case 5:
            myView()->SetProj(V3d_Xneg); // Left
            break;
        case 6:
            myView()->SetProj(V3d_Xpos); // Right
            break;
        default:
            break;
        }
    }
    /// 重置视图的居中和方向
    void OCCView::Reset(void)
    {
        if(!myView().IsNull())
        {
            myView()->Reset();
        }
    }
    /// 获取当前缩放比例
    float OCCView::GetScale(void)
    {
        if(myView().IsNull())
        {
            return -1;
        }
        else
        {
            return (float)myView()->Scale();
        }
    }
#pragma endregion
#pragma region 显示模式
    // 设置隐藏线显示模式
    void OCCView::SetDegenerateMode(bool theMode)
    {
        if(!myView().IsNull())
        {
            myView()->SetComputedMode(theMode ? Standard_True : Standard_False);
            myView()->Redraw();
        }
    }
    // 设置显示模式
    void OCCView::SetDisplayMode(int theMode)
    {
        if(myAISContext().IsNull())
        {
            return;
        }
        AIS_DisplayMode aCurrentMode;
        if(theMode == 0)
        {
            aCurrentMode = AIS_WireFrame;
        }
        else
        {
            aCurrentMode = AIS_Shaded;
        }

        if(myAISContext()->NbSelected() == 0)
        {
            myAISContext()->SetDisplayMode(aCurrentMode, Standard_False);
        }
        else
        {
            for(myAISContext()->InitSelected(); myAISContext()->MoreSelected(); myAISContext()->NextSelected())
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
        if(myAISContext().IsNull())
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
        if(!myAISContext().IsNull())
        {
            myAISContext()->SelectRectangle(Graphic3d_Vec2i(theX1, theY1),
                                            Graphic3d_Vec2i(theX2, theY2),
                                            myView());
            myAISContext()->UpdateCurrentViewer();
        }
    }
    /// 选择
    void OCCView::Select(void)
    {
        if(!myAISContext().IsNull())
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
        if((!myAISContext().IsNull()) && (!myView().IsNull()))
        {
            myAISContext()->SelectRectangle(Graphic3d_Vec2i(theX1, theY1),
                                            Graphic3d_Vec2i(theX2, theY2),
                                            myView(),
                                            AIS_SelectionScheme_XOR);
            myAISContext()->UpdateCurrentViewer();
        }
    }
    /// 继续选择
    void OCCView::ShiftSelect(void)
    {
        if(!myAISContext().IsNull())
        {
            myAISContext()->SelectDetected(AIS_SelectionScheme_XOR);
            myAISContext()->UpdateCurrentViewer();
        }
    }
    void OCCView::Display(AIS_InteractiveObject *theAISObject, bool theToUpdateViewer){
        myAISContext()->Display(theAISObject, theToUpdateViewer);
    }
    void OCCView::Test(void)
    {
        TopoDS_Shape aShape = BRepPrimAPI_MakeBox(10, 10, 10).Shape();
        Handle(AIS_Shape) anAIS = new AIS_Shape(aShape);
        myAISContext()->Display(anAIS, true);
    }
#pragma endregion
}
