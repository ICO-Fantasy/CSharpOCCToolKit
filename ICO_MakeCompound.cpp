#include "ICO_MakeCompound.h"
#include "ICO_Topo_Shape.h"

using namespace OCCTK::OCC::Topo;

namespace OCCTK {
namespace Tool {

CompoundMaker::CompoundMaker() {
}

CompoundMaker::CompoundMaker(List<TShape^>^ theShapeList) {
	for each (auto oneShape in theShapeList) {
		myShapeList->push_back(oneShape->GetOCC());
	}
}

TShape^ CompoundMaker::Shape() {
	TopoDS_Compound result;
	myBuilder->MakeCompound(result);
	for (size_t i = 0; i < myShapeList->size(); i++) {
		myBuilder->Add(result, myShapeList->at(i));
	}
	return gcnew TShape(result);
}

void CompoundMaker::Add(TShape^ theShape) {
	myShapeList->push_back(theShape->GetOCC());
}

}
}