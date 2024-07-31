#pragma once
#include <AIS_InteractiveContext.hxx>
#include <AIS_ViewCube.hxx>
#include <OpenGl_GraphicDriver.hxx>
#include <Standard_Handle.hxx>
#include <V3d_View.hxx>
#include <V3d_Viewer.hxx>
#include <AIS_Manipulator.hxx>
//包装C++类到托管类
#include <NCollection_Haft.h> 

#include "WAIS_Shape.h"
#include "WAIS_InteractiveObject.h"
#include "WAIS_Manipulator.h"
#include "WV3d_View.h"
#include "WAIS_InteractiveContext.h"

using namespace OCCTK::OCC;

namespace OCCTK {
namespace Visualization
{
public enum class SelectionMode {
	Vertex,
	Edge,
	Face,
	Shell,
	Shape,
};

public ref class CSharpViewer {
public:
	void InitOCCViewer(void);
	bool InitViewer(System::IntPtr theWnd);
	bool InitSecondViewer(System::IntPtr theSecondWnd);
	V3d::WV3d_View^ GetMainView(void);
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
	void Select(void);
	void SelectAIS(AIS::WAIS_Shape^ theAIS);
	void MultipleSelect(void);
	void XORSelect(void);
	void AreaSelect(int theX1, int theY1, int theX2, int theY2);
	void MultipleAreaSelect(int theX1, int theY1, int theX2, int theY2);
	void XORAreaSelect(int theX1, int theY1, int theX2, int theY2);
	AIS::WAIS_Shape^ GetSelectedAIS(void);
	void DisplayOriginTrihedron();
#pragma region Manipulator
	void ActivatedModes(AIS::WAIS_InteractiveObject^ theAISObject);
	void DeactiveObject(AIS::WAIS_InteractiveObject^ theAISObject);
	void Active(AIS::WAIS_InteractiveObject^ theAISObject, int theSelectionMode);
	//void Attach(AIS::WAIS_InteractiveObject^);
#pragma endregion
#pragma endregion

#pragma region 持久变换对象
	void DisplayViewCube(float axisSize);
	void DisplayGrid(bool theFlag);
	void DisplayViewTrihedron(float axisSize);
#pragma endregion	

	//! 测试代码
	AIS::WAIS_Shape^ TestMakeBox(void);
public:
	AIS::WAIS_InteractiveContext^ CSharpAISContext;
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