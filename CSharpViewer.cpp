//CSharpViewer.cpp: 委托到 .NET 的 OCC 视图类
#pragma once
#include "CSharpViewer.h"
//OCC图形显示
#include <Aspect_DisplayConnection.hxx>
#include <WNT_Window.hxx>
#include <OpenGl_GraphicDriver.hxx>
//视图控制
#include <V3d_Viewer.hxx>
#include <V3d_View.hxx>
#include <AIS_InteractiveContext.hxx>
//其它OCC类
//包装C++类到托管类
#include <NCollection_Haft.h> 

#include <Standard_Handle.hxx>
#include <Graphic3d_TransformPers.hxx>
//local
#include "ICO_ViewCube.h"
#include "ICO_Trihedron.h"

using namespace OCCTK::OCC;

namespace OCCTK {
namespace Visualization {

CSharpViewer::CSharpViewer() {
	myGraphicDriver() = NULL;
	myViewer() = NULL;
	myAISContext() = NULL;
	myViews = gcnew List<V3d::View^>();
	myViewCube() = NULL;
	myOriginTri() = NULL;
	myViewTri() = NULL;
}

// 初始化Viewer
bool CSharpViewer::InitViewer() {
	//创建OpenGL引擎
	try {
		Handle(Aspect_DisplayConnection) aDisplayConnection;
		myGraphicDriver() = new OpenGl_GraphicDriver(aDisplayConnection);
	}
	catch (Standard_Failure) {
		return false;
	}

	//创建V3d_Viewer
	myViewer() = new V3d_Viewer(myGraphicDriver());

	// 创建AIS_InteractiveContext交互对象管理器
	myAISContext() = new AIS_InteractiveContext(myViewer());

	return true;
}

bool CSharpViewer::CreateView(System::IntPtr theWnd) {
	// 创建V3d_View
	auto newView = myViewer()->CreateView();
	// 创建Windows窗口，并设置为V3d_View的窗口
	Handle(WNT_Window) aWNTWindow = new WNT_Window(reinterpret_cast<HWND> (theWnd.ToPointer()));
	newView->SetWindow(aWNTWindow);
	if (!aWNTWindow->IsMapped()) {
		aWNTWindow->Map();
	}
	auto test = gcnew V3d::View(newView);
	myViews->Add(test);
	return true;
}

#pragma region 视图控制

void CSharpViewer::Update() {
	if (!myAISContext().IsNull()) {
		for each (auto view in myViews) {
			if (view->GetOCC().IsNull()) {
				continue;
			}
			view->Redraw();
		}
		myAISContext()->UpdateCurrentViewer();
	}
}

#pragma endregion

#pragma region 交互对象管理
//! 存在 AIS_InteractiveContext 的情况下，交互对象均应该通过 AIS_InteractiveContext 进行管理，而不是直接操作对象本身

#pragma endregion

#pragma region 持久变换对象

void CSharpViewer::SetViewCube(float axesRadius) {
	ViewCube^ aViewCube = gcnew ViewCube(axesRadius);//根据默认设置构建
	myViewCube() = aViewCube->GetOCC();
}

void CSharpViewer::SetOriginTrihedron(float axisSize) {
	Trihedron^ aTri = gcnew Trihedron(axisSize);//根据默认设置构建
	myOriginTri() = aTri->GetOCC();
}

void CSharpViewer::SetViewTrihedron(float axisSize) {
	Trihedron^ aTri = gcnew Trihedron(axisSize);
	myViewTri() = aTri->GetOCC();
	// 右下角
	myViewTri()->SetTransformPersistence(new	Graphic3d_TransformPers((Graphic3d_TMF_TriedronPers), Aspect_TOTP_RIGHT_LOWER, Graphic3d_Vec2i(80, 50)));
}

//显示XOY平面网格
void CSharpViewer::DisplayGrid(bool theFlag) {
	if (theFlag)
		myViewer()->ActivateGrid(Aspect_GT_Rectangular, Aspect_GDM_Lines);
	else
		myViewer()->DeactivateGrid();
}

//显示视角立方体
void CSharpViewer::DisplayViewCube(bool theFlag) {
	if (myViewCube().IsNull()) {
		SetViewCube(5);
	}
	if (theFlag) {
		myAISContext()->Display(myViewCube(), false);
	}
	else {
		if (myViewCube().IsNull()) { return; }
		myAISContext()->Erase(myViewCube(), false);
	}
}

// 显示原点坐标
void CSharpViewer::DisplayOriginTrihedron(bool theFlag) {
	if (myOriginTri().IsNull()) { SetOriginTrihedron(200); }
	if (theFlag) {
		myAISContext()->Display(myOriginTri(), false);
	}
	else {
		if (myOriginTri().IsNull()) { return; }
		myAISContext()->Erase(myOriginTri(), false);
	}
}

//显示视角坐标系
void CSharpViewer::DisplayViewTrihedron(bool theFlag) {
	if (theFlag) {
		if (myViewTri().IsNull()) { SetViewCube(50); }
		myAISContext()->Display(myViewTri(), false);
	}
	else {
		if (myViewTri().IsNull()) { return; }
		myAISContext()->Erase(myViewTri(), false);
	}
}

InteractiveContext^ CSharpViewer::GetContext() {
	return gcnew InteractiveContext(myAISContext());
}

V3d::View^ CSharpViewer::GetMainView() {
	if (myViews->Count == 0) {
		return nullptr;
	}
	auto f = myViews->ToArray()[0];
	return f;
}

V3d::Viewer^ CSharpViewer::GetViewer() {
	return gcnew V3d::Viewer(myViewer());
}

#pragma endregion

}
}
