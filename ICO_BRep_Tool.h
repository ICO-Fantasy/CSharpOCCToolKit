#pragma once

namespace OCCTK {
namespace OCC {
namespace gp {
ref class Pnt;
}
namespace Topo {
ref class TShape;
ref class TVertex;
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
};

}
}
}