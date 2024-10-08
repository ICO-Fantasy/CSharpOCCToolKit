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

using namespace OCCTK::OCC;

namespace OCCTK {
namespace Visualization {

CSharpViewer::CSharpViewer() {
	myViews = gcnew List<V3d::View^>();
}

/// <summary>
/// 初始化Viewer
/// </summary>
/// <returns></returns>
bool CSharpViewer::InitViewer() {
	//创建OpenGL引擎
	//创建V3d_Viewer
	Handle(Aspect_DisplayConnection) aDisplayConnection;
	myViewer = gcnew V3d::Viewer(new V3d_Viewer(new OpenGl_GraphicDriver(aDisplayConnection)));

	// 创建AIS_InteractiveContext交互对象管理器
	myAISContext = gcnew AIS::InteractiveContext(myViewer);

	return true;
}

/// <summary>
/// 绑定系统IntPtr，创建三维窗口
/// </summary>
/// <param name="theWnd"></param>
/// <returns></returns>
V3d::View^ CSharpViewer::CreateView(System::IntPtr theWnd) {
	// 创建V3d_View
	V3d::View^ newView = myViewer->CreateView();
	newView->SetWindow(theWnd);
	myViews->Add(newView);
	return newView;
}

/// <summary>
/// 获取交互对象管理器
/// </summary>
/// <returns></returns>
AIS::InteractiveContext^ CSharpViewer::GetContext() {
	return myAISContext;
}

/// <summary>
/// 获取主窗口（创建的第一个窗口）
/// </summary>
/// <returns></returns>
V3d::View^ CSharpViewer::GetMainView() {
	if (myViews->Count == 0) {
		throw gcnew System::Exception("未创建View");
	}
	return myViews->ToArray()[0];
}

/// <summary>
/// 获取三维场景管理器
/// </summary>
/// <returns></returns>
V3d::Viewer^ CSharpViewer::GetViewer() {
	return myViewer;
}


}
}
