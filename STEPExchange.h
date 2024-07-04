#include <TopoDS_Shape.hxx>
#include <vcclr.h>
#include "WTopoDS_Shape.h"
using namespace System;
using namespace OCCTK::OCC::TopoDS;

#pragma once
namespace OCCTK {
namespace IO {

public ref class STEPExchange
{
public:
	STEPExchange(WTopoDS_Shape^ theShape);
	STEPExchange(String^ filePath);
	void ReadFile(String^ filePath);
	WTopoDS_Shape^ Shape();
	bool SaveFile(String^ filePath);
private:
	WTopoDS_Shape^ myShape;
};

}
}

