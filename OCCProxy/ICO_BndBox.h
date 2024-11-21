#pragma once

namespace OCCTK {
namespace OCC {
namespace gp {
ref class Pnt;
}
namespace Topo {
ref class TShape;
}
namespace Bnd {
ref class AABB;
}
}
}

namespace OCCTK {
namespace Extension {

public ref class BoundingBox {
public:
	BoundingBox(OCC::Topo::TShape^ TopoShape);
	OCC::Bnd::AABB^ GetAABB();
private:
	OCC::Bnd::AABB^ myBox;
	OCC::Topo::TShape^ myShape;
};

}
}

