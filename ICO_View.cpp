#include "ICO_View.h"
#include <V3d_View.hxx>

namespace OCCTK {
namespace OCC {
namespace V3d {

View::View(V3d_View theView) {
	myView() = new V3d_View(theView);
}

View::View(Handle(V3d_View) theView) {
	myView() = theView;
}

Handle(V3d_View) View::getOCC() {
	return myView();
}

}
}
}