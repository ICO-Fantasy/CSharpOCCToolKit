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
//using namespace OCCTK::OCC::AIS;
//using namespace OCCTK::Extension;

namespace OCCTK {
namespace Visualization {


public ref class CSharpViewer {
public:
	CSharpViewer();
	bool InitViewer();
	bool CreateView(System::IntPtr theWnd);

#pragma region 视图控制

	void Update();

#pragma endregion

#pragma region 持久变换对象

	void DisplayGrid(bool theFlag);

#pragma endregion	

public:
	AIS::InteractiveContext^ GetContext();
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