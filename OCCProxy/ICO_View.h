#pragma once
#include <V3d_View.hxx>
//local
#include "ICO_BaseObject.h"

namespace OCCTK {
namespace Extension {
ref class CameraOrientation;
value struct Color;
}
namespace OCC {
namespace Graphic3d {
ref class Camera;
}
namespace V3d {

enum class ViewOrientation;

public ref class View :BaseObject {
private:
    Handle(V3d_View) myView() { return Handle(V3d_View)::DownCast(NativeHandle); };
public:
    View(const Handle(V3d_View)& theView);
    View(const Handle(V3d_View)& theView, System::IntPtr theWnd);
    Handle(V3d_View) GetOCC();
    //! 隐式转换为 V3d_View
    static operator Handle(V3d_View) (View^ v) { return v->GetOCC(); }
public:
    void SetWindow(System::IntPtr theWnd);
    void Remove();
    void SetDefault();
    void SetDefaultRendering();
    void SetDefaultBGColor();
    void SetBgGradientColors(Extension::Color C1, Extension::Color C2, bool update);
    double GetScale();
public:
    void SetICORendering();
    void SetICOBGColor();
#pragma region 渲染

    void Update();
    void Redraw();
    void MustBeResized();
    void SetDegenerateMode(bool open);

#pragma endregion

#pragma region 调整画面

    void WindowFitAll(int theXmin, int theYmin, int theXmax, int theYmax);
    void FitAll(double theMargin, bool update);
    void ZFitAll(double theScaleFactor);
    void Reset();

    void Place(int theX, int theY, float theZoomFactor);
    void Zoom(int theX1, int theY1, int theX2, int theY2);
    void SetZoom(double theZoomFactor, bool update);
    void StartZoomAtPoint(double startX, double startY);
    void ZoomAtPoint(double startX, double startY, double endX, double endY);

    //void Pan(int theX, int theY);
    void StartPan();
    void Pan(int theDX, int theDY);

    void StartRotation(int theX, int theY);
    void Rotation(int theX, int theY);

#pragma endregion
#pragma region Camera
    System::ValueTuple<double, double, double> GetProjection();
    void SetViewOrientation(double theX, double theY, double theZ);
    void SetViewOrientation(System::ValueTuple<double, double, double> projectionVec);
    void SetViewOrientation(ViewOrientation theOrientation, bool update);
    Graphic3d::Camera^ Camera();
    void SetCamera(Graphic3d::Camera^ camera);

    Extension::CameraOrientation^ CurrentViewOrientation();
    void SetViewOrientation(Extension::CameraOrientation^ theOrientation, bool update);
#pragma endregion


    void DisplayDefault_GraduatedTrihedron();
    void Hide_GraduatedTrihedron();
};

}
}
}

