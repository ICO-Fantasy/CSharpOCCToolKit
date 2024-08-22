#pragma once
#include <TopoDS_Shape.hxx>
#include "ICO_TopoDS_Shape.h"
using namespace System;
using namespace OCCTK::OCC::TopoDS;

#pragma once
namespace OCCTK {
namespace IO {

public ref class STEPExchange {
public:
	STEPExchange(TShape^ theShape);
	STEPExchange(String^ filePath);
	void ReadFile(String^ filePath);
	TShape^ Shape();
	bool SaveFile(String^ filePath);
private:
	TShape^ myShape;
};

}
}

