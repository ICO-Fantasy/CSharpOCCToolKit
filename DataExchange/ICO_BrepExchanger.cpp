#pragma once
#include <BRep_Builder.hxx>
#include <BRepTools.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopoDS_Shape.hxx>
//local
#include "ICO_BrepExchanger.h"
#include "ICO_StringExchange.h"
#include "..\OCCProxy\ICO_Topo_Shape.h"

using namespace OCCTK::OCC::Topo;
using namespace OCCTK::OCC::gp;
using namespace System;

namespace OCCTK {
namespace IO {

BrepExchanger::BrepExchanger(TShape^ theShape) {
    myShape = theShape;
}

BrepExchanger::BrepExchanger(String^ filePath) {
    this->ReadFile(filePath);
}

void BrepExchanger::ReadFile(String^ filePath) {
    BRepTools brepTool;
    BRep_Builder builder;
    TopoDS_Shape shape;
    brepTool.Read(shape, DataExchange::ToAsciiString(filePath).ToCString(), builder);
    myShape = gcnew TShape(shape);
}

TShape^ BrepExchanger::Shape() {
    return myShape;
}

bool BrepExchanger::SaveFile(String^ filePath) {
    TopoDS_Shape theoccShape = myShape->GetOCC();
    auto cPath = DataExchange::ConvertChineseToUnicode(filePath);
    TCollection_AsciiString asciiString(cPath, 'x');

    BRepTools brepTool;
    brepTool.Write(theoccShape, asciiString.ToCString());
    return true;
}

}
}
