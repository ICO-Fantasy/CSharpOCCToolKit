#pragma once
#include <V3d_Viewer.hxx>
#include <V3d_View.hxx>
#include <AIS_InteractiveContext.hxx>
#include <AIS_ViewCube.hxx>
#include <OpenGl_GraphicDriver.hxx>
#include <Standard_Handle.hxx>
//包装C++类到托管类
#include <NCollection_Haft.h> 
#include "WAIS_Shape.h"
using namespace OCCTK::OCC;
//using namespace System;
namespace OCCTK::Visualization
{
	public enum class SelectionMode {
		Vertex,
		Edge,
		Face,
		Shell,
		Shape,
	};
	//public enum class testS :TopAbs_ShapeEnum {
	//
	//};
	public ref class CSharpViewer {
	public:
		void InitOCCViewer(void);
		bool InitViewer(System::IntPtr theWnd);
		bool InitSecondViewer(System::IntPtr theSecondWnd);
#pragma region 视图设置
		void SetDefaultLight();
		void SetDefaultRendering(Handle(V3d_View) aView);
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
		void EraseObjects(void);
		void MoveTo(int theX, int theY);
		void SetSelectionMode(SelectionMode theMode);
		void Select(void);
		void SelectAIS(AIS::WAIS_Shape^ theAIS);
		void MultipleSelect(void);
		void XORSelect(void);
		void AreaSelect(int theX1, int theY1, int theX2, int theY2);
		void MultipleAreaSelect(int theX1, int theY1, int theX2, int theY2);
		void XORAreaSelect(int theX1, int theY1, int theX2, int theY2);
		void Display(Handle(AIS_InteractiveObject) theAISObject, bool theToUpdateViewer);
		void Display(AIS::WAIS_Shape^ theAIS, bool theToUpdateViewer);
		void EraseAll();
		void Erase(Handle(AIS_InteractiveObject) theAISObject, bool theToUpdateViewer);
		void Erase(AIS::WAIS_Shape^ theAIS, bool theToUpdateViewer);
		AIS::WAIS_Shape^ GetSelectedAIS(void);
		void DisplayOriginTrihedron();
#pragma endregion
#pragma region 持久变换对象
		void DisplayViewCube(float axisSize);
		void DisplayGrid(bool theFlag);
		void DisplayViewTrihedron(float axisSize);
#pragma endregion	
		//!测试代码
		void Test(void);
		AIS::WAIS_Shape^ TestMakeBox(void);
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