#include "ICO_Topo_Wire.h"
#include <TopoDS.hxx>

namespace OCCTK {
namespace OCC {
namespace Topo {

TWire::TWire() {
	myShape = new TopoDS_Wire();
}

TWire::TWire(const TopoDS_Wire theWire) {
	myShape = new TopoDS_Wire(theWire);
}

TWire::TWire(System::IntPtr theWireIntPtr) {
	// 将 IntPtr 转换为原生指针
	TopoDS_Wire* pShape = reinterpret_cast<TopoDS_Wire*>(theWireIntPtr.ToPointer());
	myShape = pShape;
}

TWire::TWire(TopoDS_Wire* theWire) {
	myShape = theWire;
}

TopoDS_Wire TWire::GetOCC() {
	return TopoDS::Wire(*myShape);
}

}
}
}