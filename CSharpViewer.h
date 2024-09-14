#pragma once
//OCC
#include <AIS_InteractiveContext.hxx>
#include <OpenGl_GraphicDriver.hxx>
#include <Standard_Handle.hxx>
#include <V3d_Viewer.hxx>
//包装C++类到托管类
#include <NCollection_Haft.h> 
//local
#include "ICO_InteractiveContext.h"
#include "ICO_View.h"
#include "ICO_Viewer.h"

using namespace OCCTK::OCC;
using namespace OCCTK::Extension;

namespace OCCTK {
namespace Visualization {

/// <summary>
/// 存在 AIS_InteractiveContext 的情况下，交互对象均应该通过 AIS_InteractiveContext 进行管理，而不是直接操作对象本身（给对象赋予材质和颜色）
/// </summary>
public ref class CSharpViewer {
public:
	CSharpViewer();
	bool InitViewer();
	V3d::View^ CreateView(System::IntPtr theWnd);

	AIS::InteractiveContext^ GetContext();
	V3d::View^ GetMainView();
	V3d::Viewer^ GetViewer();
private:
	//NCollection_Haft<Handle(V3d_Viewer)> myViewer;
	//NCollection_Haft<Handle(AIS_InteractiveContext)> myAISContext;
	//NCollection_Haft<Handle(OpenGl_GraphicDriver)> myGraphicDriver;
	V3d::Viewer^ myViewer;
	AIS::InteractiveContext^ myAISContext;
	List<V3d::View^>^ myViews;

};
}
}