#pragma once

namespace OCCTK {
namespace OCC {
namespace gp {
ref class Pnt;
}
namespace Topo {
ref class TShape;
ref class TVertex;
ref class TEdge;
}
namespace Geom {
ref class Curve;
}
}
}

namespace OCCTK {
namespace OCC {
namespace BRep {

public ref class Tool {
private:
	Tool() {};
public:
	static gp::Pnt^ Pnt(Topo::TVertex^ vertex);
	static Geom::Curve^ Curve(Topo::TEdge^ edge, double% first, double% last);
};

}
}
}