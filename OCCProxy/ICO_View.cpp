#include "ICO_View.h"
#include <V3d_View.hxx>
#include <WNT_Window.hxx>
//local
#include "ICO_Graphic3d_Camera.h"
#include "ICO_Pnt.h"
#include "ICO_Trsf.h"
#include "ICO_InteractiveContext.h"
#include "ICO_InteractiveObject.h"
#include "ICO_Vec.h"
#include "ICO_Dir.h"
#include "ICO_ImageDumpOptions.h"
#include "ICO_PixMap.h"
#include "..\Extension\ICO_CameraOrientation.h"
#include "..\DataExchange\ICO_StringExchange.h"
#include <Image_VideoRecorder.hxx>

using namespace System::Collections::Generic;
using namespace OCCTK::OCC::Graphic3d;
using namespace OCCTK::OCC::gp;
using namespace OCCTK::OCC::AIS;
using namespace OCCTK::Extension;
using namespace OCCTK::DataExchange;

namespace OCCTK {
namespace OCC {
namespace V3d {

/// <summary>
/// 创建无屏窗口
/// </summary>
/// <param name=""></param>
View::View(const Handle(V3d_View)& theView) :BaseObject(theView) {
}

/// <summary>
/// 创建可视化窗口
/// </summary>
/// <param name=""></param>
/// <param name="theWnd">窗体句柄</param>
View::View(const Handle(V3d_View)& theView, System::IntPtr theWnd) :BaseObject() {
    NativeHandle = theView;
    this->SetWindow(theWnd);
}


Handle(V3d_View) View::GetOCC() {
    return myView();
}

void View::SetWindow(System::IntPtr theWnd) {
    // 创建Windows窗口，并设置为V3d_View的窗口
    Handle(WNT_Window) aWNTWindow = new WNT_Window(reinterpret_cast<HWND> (theWnd.ToPointer()));
    myView()->SetWindow(aWNTWindow);
    if (!aWNTWindow->IsMapped()) {
        aWNTWindow->Map();
    }
}

void View::Remove() {
    myView()->Remove();
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
/// 设置自定义的渲染参数
/// </summary>
void View::SetICORendering() {
    if (myView().IsNull()) { return; }
    //光栅化渲染
    myView()->ChangeRenderingParams().Method = Graphic3d_RenderingMode::Graphic3d_RM_RASTERIZATION;
    //阴影
    myView()->ChangeRenderingParams().IsShadowEnabled = false;
    //反射
    myView()->ChangeRenderingParams().IsReflectionEnabled = false;
    //抗锯齿
    myView()->ChangeRenderingParams().IsAntialiasingEnabled = true;
    //透明阴影
    myView()->ChangeRenderingParams().IsTransparentShadowEnabled = false;
    //多重采样抗锯齿
    myView()->ChangeRenderingParams().NbMsaaSamples = 8;
    //禁用所有的性能计数器
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
void View::SetBgGradientColors(Color C1, Color C2, bool update) {
    if (myView().IsNull()) { return; }
    myView()->SetBgGradientColors(C1.GetOCC(), C2.GetOCC(), Aspect_GradientFillMethod_Vertical, update);
}

/// <summary>
/// 获取当前缩放比例
/// </summary>
/// <returns></returns>
double View::GetScale() {
    if (myView().IsNull())  return -1.0;
    return myView()->Scale();
}

void View::Update() {
    if (myView().IsNull()) return;
    myView()->Update();
}

/// <summary>
/// 重绘
/// </summary>
void View::Redraw() {
    if (myView().IsNull()) return;
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
/// 返回投影向量。
/// </summary>
/// <returns></returns>
System::ValueTuple<double, double, double> View::GetProjection() {
    if (myView().IsNull())  return System::ValueTuple<double, double, double>(0.0, 0.0, 0.0);
    double x, y, z;
    myView()->Proj(x, y, z);
    return System::ValueTuple<double, double, double>(x, y, z);
}

/// <summary>
/// 设置投影向量
/// </summary>
/// <param name="theX"></param>
/// <param name="theY"></param>
/// <param name="theZ"></param>
void View::SetViewOrientation(double theX, double theY, double theZ) {
    if (myView().IsNull())  return;
    myView()->SetProj(theX, theY, theZ);
}

void View::SetViewOrientation(System::ValueTuple<double, double, double> projectionVec) {
    if (myView().IsNull())  return;
    myView()->SetProj(projectionVec.Item1, projectionVec.Item2, projectionVec.Item3);
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
/// 获取当前的相机对象
/// </summary>
/// <returns></returns>
Camera^ View::Camera() {
    if (myView().IsNull())  throw gcnew System::Exception("View对象为空，无法获取相机");
    return gcnew Graphic3d::Camera(myView()->Camera());//返回的是当前相机对象的引用
}

/// <summary>
/// 设置相机对象
/// </summary>
/// <param name="camera"></param>
void View::SetCamera(Graphic3d::Camera^ camera) {
    if (myView().IsNull())  return;
    myView()->SetCamera(camera->GetOCC());
}

CameraOrientation^ View::CurrentViewOrientation() {
    CameraOrientation^ params = gcnew CameraOrientation();

    params->Scale = myView()->Scale();
    params->Aspect = myView()->Camera()->Aspect();
    params->ViewDirection = gcnew Dir(myView()->Camera()->Direction());

    double eyeX, eyeY, eyeZ;
    myView()->Eye(eyeX, eyeY, eyeZ);
    params->Eye = gcnew Pnt(eyeX, eyeY, eyeZ);

    double atX, atY, atZ;
    myView()->At(atX, atY, atZ);
    params->ViewPoint = gcnew Pnt(atX, atY, atZ);

    double upX, upY, upZ;
    myView()->Up(upX, upY, upZ);
    params->HightPoint = gcnew Pnt(upX, upY, upZ);

    double projX, projY, projZ;
    myView()->Proj(projX, projY, projZ);
    params->Projection = gcnew Vec(projX, projY, projZ);

    double width, hight;
    myView()->Size(width, hight);
    params->Size = System::ValueTuple<double, double>(width, hight);

    return params;
}

void View::SetViewOrientation(CameraOrientation^ theOrientation, bool update) {
    myView()->SetScale(theOrientation->Scale);
    myView()->Camera()->SetAspect(theOrientation->Aspect);
    myView()->Camera()->SetDirection(theOrientation->ViewDirection->GetOCC());
    myView()->SetEye(theOrientation->Eye->X, theOrientation->Eye->Y, theOrientation->Eye->Z);
    myView()->SetAt(theOrientation->ViewPoint->X, theOrientation->ViewPoint->Y, theOrientation->ViewPoint->Z);
    myView()->SetUp(theOrientation->HightPoint->X, theOrientation->HightPoint->Y, theOrientation->HightPoint->Z);
    myView()->SetProj(theOrientation->Projection->X, theOrientation->Projection->Y, theOrientation->Projection->Z);
    myView()->SetSize(theOrientation->Size.Item1 / theOrientation->Size.Item2);
    if (update) { myView()->Update(); }
}

bool View::ToPixMap(Image::PixMap^ pixmap, ImageDumpOptions options)
{
    return myView()->ToPixMap(pixmap->GetRef(), options);
}

void View::RecordTest(InteractiveContext^ context, InteractiveObject^ ais, System::String^ filePath, List<Trsf^>^ trsfList, int x, int y)
{
    myView()->FitAll();
    // 设置视频录制参数，例如视频格式、分辨率和帧率。
    Image_VideoParams aRecParams;
    aRecParams.Width = x;   // 视频的宽度，取自窗口大小。
    aRecParams.Height = y;  // 视频的高度，取自窗口大小。
    aRecParams.FpsNum = 24;            // 帧率的分子，表示每秒24帧。
    //aRecParams.Format = "matroska";    // 视频文件格式，这里指定为 Matroska (.mkv) 格式。
    //aRecParams.PixelFormat = "yuv420p"; // 像素格式，常用于视频编码以压缩存储。
    //aRecParams.VideoCodec = "mjpeg";   // 视频编码器，这里使用 MJPEG（Motion JPEG）。

    // 创建一个视频录制器对象，用于管理视频的录制过程。
    Handle(Image_VideoRecorder) aRecorder = new Image_VideoRecorder();

    // 打开视频录制器，指定输出文件名和录制参数。
    System::Diagnostics::Debug::WriteLine(gcnew System::String("D:\\TEST\\test.mkv"));
    if (!aRecorder->Open("D:\\TEST\\test.mkv", aRecParams))
    {
        throw gcnew System::Exception("文件打开失败");          // 如果打开失败，输出错误信息。
    }

    // 开始动画循环，录制动画为视频。
    for (size_t aNbFrames = 0; aNbFrames < trsfList->Count; aNbFrames++)
    {
        // 计算当前帧的展示时间戳（PTS，Presentation Timestamp）。
        double aPts = (double(aRecParams.FpsDen) / double(aRecParams.FpsNum))
            * double(aNbFrames); // PTS = 时间间隔 × 帧数

        // 更新动画到当前时间点。
        ais->SetLocalTransformation(trsfList[aNbFrames]);
        context->Redisplay(ais, true);
        context->UpdateCurrentViewer();
        // 转储当前帧内容为像素数据。
        V3d_ImageDumpOptions aDumpParams;
        aDumpParams.Width = aRecParams.Width;  // 帧的宽度，等于视频宽度。
        aDumpParams.Height = aRecParams.Height; // 帧的高度，等于视频高度。
        //aDumpParams.BufferType = Graphic3d_BT_RGBA; // 图像缓冲区类型为RGBA。
        aDumpParams.ToAdjustAspect = true;      // 调整宽高比以匹配目标分辨率。

        // 从视图中捕获当前帧的像素数据，并将其保存到视频录制器的缓冲区。
        if (!myView()->ToPixMap(aRecorder->ChangeFrame(), aDumpParams))
        {
            throw gcnew System::Exception("图像捕获失败");          // 如果dump失败，输出错误信息。
        }

        // 将帧的像素数据沿Y轴翻转。
        Image_PixMap::FlipY(aRecorder->ChangeFrame());

        // 将当前帧推送到视频录制器以存入视频文件。
        if (!aRecorder->PushFrame()) { throw gcnew System::Exception("推送失败"); } // 如果推送失败，退出循环。
    }
}

/// <summary>
/// 显示带刻度的坐标轴
/// </summary>
void View::DisplayDefault_GraduatedTrihedron() {
    if (myView().IsNull()) return;
    Graphic3d_GraduatedTrihedron GT = Graphic3d_GraduatedTrihedron();
    //GT.ChangeXAxisAspect().SetTickmarksLength(50);
    //GT.ChangeYAxisAspect().SetTickmarksLength(50);
    //GT.ChangeZAxisAspect().SetTickmarksLength(50);
    GT.ChangeXAxisAspect().SetTickmarksNumber(10);
    GT.ChangeYAxisAspect().SetTickmarksNumber(10);
    GT.ChangeZAxisAspect().SetTickmarksNumber(3);
    GT.ChangeXAxisAspect().SetValuesOffset(30);
    GT.ChangeYAxisAspect().SetValuesOffset(30);
    GT.ChangeZAxisAspect().SetValuesOffset(30);
    myView()->GraduatedTrihedronDisplay(GT);
}

/// <summary>
/// 隐藏带刻度的坐标轴
/// </summary>
void View::Hide_GraduatedTrihedron() {
    myView()->GraduatedTrihedronErase();
}

}
}
}