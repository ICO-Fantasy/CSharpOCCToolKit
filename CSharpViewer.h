#pragma once
//OCC
#include <AIS_InteractiveContext.hxx>
#include <AIS_ViewCube.hxx>
#include <AIS_Trihedron.hxx>
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
using namespace OCCTK::OCC::AIS;
using namespace OCCTK::Extension;

namespace OCCTK {
namespace Visualization {

//! 存在 AIS_InteractiveContext 的情况下，交互对象均应该通过 AIS_InteractiveContext 进行管理，而不是直接操作对象本身
public ref class CSharpViewer {
public:
	CSharpViewer();
	bool InitViewer();
	V3d::View^ CreateView(System::IntPtr theWnd);

	InteractiveContext^ GetContext();
	V3d::View^ GetMainView();
	V3d::Viewer^ GetViewer();
private:
	NCollection_Haft<Handle(V3d_Viewer)> myViewer;
	List<V3d::View^>^ myViews;
	//List<*Handle(V3d_View)>>^ myViews = gcnew List<NCollection_Haft<Handle(V3d_View)>^>();
	NCollection_Haft<Handle(AIS_InteractiveContext)> myAISContext;
	NCollection_Haft<Handle(OpenGl_GraphicDriver)> myGraphicDriver;

};
}
}