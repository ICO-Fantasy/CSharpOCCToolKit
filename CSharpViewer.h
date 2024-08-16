#pragma once
//OCC
#include <AIS_InteractiveContext.hxx>
#include <AIS_ViewCube.hxx>
#include <AIS_Trihedron.hxx>
#include <OpenGl_GraphicDriver.hxx>
#include <Standard_Handle.hxx>
#include <V3d_View.hxx>
#include <V3d_Viewer.hxx>
//包装C++类到托管类
#include <NCollection_Haft.h> 
//local
#include "ICO_AIS_Shape.h"
#include "ICO_InteractiveObject.h"
#include "ICO_Trihedron.h"
#include "ICO_Manipulator.h"
#include "ICO_InteractiveContext.h"
#include "ICO_View.h"
#include "ICO_SelectionMode.h"

using namespace OCCTK::OCC;
using namespace OCCTK::OCC::AIS;

namespace OCCTK {
namespace Visualization {


public ref class CSharpViewer {
public:
	void InitOCCViewer(void);
	bool InitViewer(System::IntPtr theWnd);
	bool InitSecondViewer(System::IntPtr theSecondWnd);
	V3d::View^ GetMainView(void);

#pragma region 视图设置

	void SetDefaultLight();
	void SetDefaultRendering(Handle(V3d_View) aViewHandle);
	void SetDefaultHighlightStyle();

#pragma endregion

#pragma region 视图控制

	void RedrawView();
	void UpdateView();
	void UpdateCurrentViewer(void);
	void FitArea(int theXmin, int theYmin, int theXmax, int theYmax);
	void ZoomAllView(void);
	void Place(int theX, int theY, float theZoomFactor);
	void AreaZoom(int theX1, int theY1, int theX2, int theY2);
	void Zoom(double theZoomFactor, bool update);
	void StartZoomAtPoint(double startX, double startY);
	void ZoomAtPoint(double startX, double startY, double endX, double endY);
	void Pan(int theX, int theY);
	void StartRotation(int theX, int theY);
	void Rotation(int theX, int theY);
	void SetViewOrientation(int theOrientation);
	void Reset(void);
	float GetScale(void);

#pragma endregion

#pragma region 显示模式

	void SetDegenerateMode(bool theMode);
	void SetDisplayMode(int theMode);

#pragma endregion

#pragma region 交互对象管理
	// 只有部分与Viewer相关的交互对象管理方法，剩余由交互对象管理器控制

	void EraseSelect(void);
	void MoveTo(int theX, int theY);
	void SetSelectionMode(SelectionMode theMode);
	void SetSelectionMode(int theMode);
	void SetSelectionMode(TopoAbs::ShapeEnum^ theMode);
	//void Select(void);
	//void SelectAIS(AIS::Shape^ theAIS);
	//void MultipleSelect(void);
	//void XORSelect(void);
	void AreaSelect(int theX1, int theY1, int theX2, int theY2);
	void MultipleAreaSelect(int theX1, int theY1, int theX2, int theY2);
	void XORAreaSelect(int theX1, int theY1, int theX2, int theY2);
	//WAIS_Shape^ GetSelectedAIS(void);

#pragma region Manipulator

	void ActivatedModes(InteractiveObject^ theAISObject);
	void DeactiveObject(InteractiveObject^ theAISObject);
	void Active(InteractiveObject^ theAISObject, int theSelectionMode);
	//void Attach(WAIS_InteractiveObject^);

#pragma endregion

#pragma endregion

#pragma region 持久变换对象

	void SetViewCube(float axesRadius);
	void SetOriginTrihedron(float axisSize);
	void SetViewTrihedron(float axisSize);
	void DisplayGrid(bool theFlag);
	void DisplayViewCube(bool theFlag);
	void DisplayOriginTrihedron(bool theFlag);
	void DisplayViewTrihedron(bool theFlag);
private:
	NCollection_Haft<Handle(AIS_ViewCube)> myViewCube;
	NCollection_Haft<Handle(AIS_Trihedron)> myOriginTri;
	NCollection_Haft<Handle(AIS_Trihedron)> myViewTri;

#pragma endregion	

public:
	InteractiveContext^ GetContext();
	//V3d::WV3d_Viewer^ CSharpViewer;
private:
	// 将成员变为托管类型
	NCollection_Haft<Handle(V3d_Viewer)> myViewer;
	NCollection_Haft<Handle(V3d_View)> myMainView;
	NCollection_Haft<Handle(V3d_View)> mySecondView;
	NCollection_Haft<Handle(AIS_InteractiveContext)> myAISContext;
	NCollection_Haft<Handle(OpenGl_GraphicDriver)> myGraphicDriver;
	//NCollection_Haft<Handle(AIS_ViewCube)> myViewCube;

};
}
}