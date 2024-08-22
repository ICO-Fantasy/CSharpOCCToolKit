#pragma once
#include <TopoDS_Compound.hxx>
#include <BRep_Builder.hxx>
#include<vector>
//包装C++类到托管类
#include <NCollection_Haft.h> 
//local
#include"ICO_TopoDS_Shape.h"
using namespace OCCTK::OCC::TopoDS;

namespace OCCTK {
namespace Tool {

public ref class CompoundMaker {
public:
	CompoundMaker();
	CompoundMaker(List<TShape^>theShapeList);
	TShape^ Shape();
	void Add(TShape^ theShape);
private:
	std::vector<TopoDS_Shape>* myShapeList = new std::vector<TopoDS_Shape>;
	BRep_Builder* myBuilder;
};

}
}