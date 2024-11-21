#pragma once
#include <V3d_View.hxx>
#include <WNT_Window.hxx>
//包装C++类到托管类
#include <NCollection_Haft.h> 
//local
#include "..\Extension\ICO_Color.h"
#include "ICO_BaseObject.h"
#include "ICO_ViewOrientation.h"
using namespace OCCTK::Extension;

namespace OCCTK {
namespace OCC {
namespace V3d {

public ref class View :BaseObject {
private:
	Handle(V3d_View) myView() { return Handle(V3d_View)::DownCast(NativeHandle); };
public:
	View(const Handle(V3d_View)& theView);
	View(const Handle(V3d_View)& theView, System::IntPtr theWnd);
	Handle(V3d_View) GetOCC();
	void SetWindow(System::IntPtr theWnd);
	void Remove();
	void SetDefault();
	void SetDefaultRendering();
	void SetICORendering();
	void SetDefaultBGColor();
	void SetBgGradientColors(Color C1, Color C2, bool update);
	double GetScale();

#pragma region 渲染

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

	void SetViewOrientation(ViewOrientation theOrientation, bool update);

#pragma endregion

	void DisplayDefault_GraduatedTrihedron();
	void Hide_GraduatedTrihedron();
};

}
}
}

