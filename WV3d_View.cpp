#include "WV3d_View.h"
#include <V3d_View.hxx>

namespace OCCTK {
namespace OCC {
namespace V3d {

WV3d_View::WV3d_View(V3d_View theView)
{
	_ptransient = new Handle_Standard_Transient(new V3d_View(theView));// 创建基类对象智能指针
	myView = (Handle(V3d_View)*)_ptransient;// 强制转换基类对象，并创建指向Handle(V3d_View)的指针
}

WV3d_View::WV3d_View(Handle(V3d_View) theView)
{
	_ptransient = new Handle_Standard_Transient(new V3d_View(*theView));// 创建基类对象智能指针
	myView = (Handle(V3d_View)*)_ptransient;// 强制转换基类对象，并创建指向Handle(V3d_View)的指针
}

Handle(V3d_View)* WV3d_View::getOCC()
{
	return myView;
}

}
}
}