#include "ICO_TopExp_Explorer.h"
#include <TopAbs_ShapeEnum.hxx>
#include <TopExp_Explorer.hxx>
//local
#include "ICO_Topo_Shape.h"
#include "ICO_ShapeEnum.h"

using namespace OCCTK::OCC::Topo;
using namespace OCCTK::OCC::TopoAbs;

namespace OCCTK::OCC::TopExp
{

Explorer::Explorer(Topo::TShape^ shape, ShapeEnum toFind)
{
    myExp = new TopExp_Explorer(shape, TopAbs_ShapeEnum(toFind));
}

Explorer::Explorer(Topo::TShape^ shape, ShapeEnum toFind, ShapeEnum toAvoid)
{
    myExp = new TopExp_Explorer(shape, TopAbs_ShapeEnum(toFind), TopAbs_ShapeEnum(toAvoid));
}

bool Explorer::More()
{
    return myExp->More();
}

void Explorer::Next()
{
    return myExp->Next();
}

TShape^ Explorer::Value()
{
    return gcnew TShape(myExp->Value());
}

Topo::TShape^ Explorer::Current()
{
    return gcnew TShape(myExp->Current());
}

void Explorer::ReInit()
{
    myExp->ReInit();
}

Topo::TShape^ Explorer::ExploredShape()
{
    return gcnew TShape(myExp->ExploredShape());
}

int Explorer::Depth()
{
    return myExp->Depth();
}

void Explorer::Clear()
{
    myExp->Clear();
}

}