#include "WV3d_View.h"
#include <V3d_View.hxx>

namespace OCCTK {
namespace OCC {
namespace V3d {

WV3d_View::WV3d_View(V3d_View theView)
{
	myView() = new V3d_View(theView);
}

WV3d_View::WV3d_View(Handle(V3d_View) theView)
{
	myView() = theView;
}

Handle(V3d_View) WV3d_View::getOCC()
{
	return myView();
}

}
}
}