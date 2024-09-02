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


#pragma region 持久变换对象

//显示XOY平面网格
void CSharpViewer::DisplayGrid(bool theFlag) {
	if (theFlag)
		myViewer()->ActivateGrid(Aspect_GT_Rectangular, Aspect_GDM_Lines);
	else
		myViewer()->DeactivateGrid();
}

#pragma endregion

AIS::InteractiveContext^ CSharpViewer::GetContext() {
	return gcnew AIS::InteractiveContext(myAISContext());
}

V3d::View^ CSharpViewer::GetMainView() {
	if (myViews->Count == 0) {
		return nullptr;
	}
	auto firstView = myViews->ToArray()[0];
	return firstView;
}

V3d::Viewer^ CSharpViewer::GetViewer() {
	return gcnew V3d::Viewer(myViewer());
}

}
}
