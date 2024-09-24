#include "ICO_BndBox.h"
#include <Bnd_Box.hxx>
#include <TopoDS_Shape.hxx>
#include <BRepBndLib.hxx>
//local
#include "ICO_Pnt.h"
#include "ICO_Topo_Shape.h"
#include "ICO_AABB.h"

using namespace OCCTK::OCC::Topo;
using namespace OCCTK::OCC::gp;
using namespace OCCTK::OCC::Bnd;

namespace OCCTK {
namespace Extension {

BoundingBox::BoundingBox(TShape^ TopoShape) {
	Bnd_Box box = Bnd_Box();
	myShape = TopoShape;
	BRepBndLib::Add(myShape->GetOCC(), box);
	Bnd_Box* temp = new Bnd_Box(box);
	myBox = gcnew AABB(temp);
}

/// <summary>
/// 获取AABB包围盒
/// </summary>
/// <returns></returns>
AABB^ BoundingBox::GetAABB() {
	if (myShape == nullptr) {
		throw gcnew System::Exception(gcnew System::String("Non Shape Added"));
	}
	return myBox;
}

}
}