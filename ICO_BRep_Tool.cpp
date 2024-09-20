#include "ICO_BRep_Tool.h"
#include <BRep_Tool.hxx>
//local
#include "ICO_Pnt.h"
#include "ICO_Topo_Vertex.h"

using namespace OCCTK::OCC::gp;
using namespace OCCTK::OCC::Topo;

namespace OCCTK {
namespace OCC {
namespace BRep {

/// <summary>
/// 返回TopoDS_Vertex对应的gp_Pnt点
/// </summary>
/// <param name="vertex"></param>
/// <returns></returns>
gp::Pnt^ Tool::Pnt(TVertex^ vertex) {
	return gcnew gp::Pnt(BRep_Tool::Pnt(vertex->GetOCC()));
}

}
}
}

