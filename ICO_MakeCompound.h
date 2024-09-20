#pragma once
#include <TopoDS_Compound.hxx>
#include <BRep_Builder.hxx>
#include <vector>
//包装C++类到托管类
#include <NCollection_Haft.h> 
//local
#include"ICO_Topo_Shape.h"

namespace OCCTK {
namespace Tool {

public ref class CompoundMaker {
public:
	CompoundMaker();
	CompoundMaker(List<OCC::Topo::TShape^>theShapeList);
	OCC::Topo::TShape^ Shape();
	void Add(OCC::Topo::TShape^ theShape);
private:
	std::vector<TopoDS_Shape>* myShapeList = new std::vector<TopoDS_Shape>;
	BRep_Builder* myBuilder;
};

}
}