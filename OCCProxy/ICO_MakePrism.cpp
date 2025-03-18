#include "ICO_MakePrism.h"
#include <BRepPrimAPI_MakePrism.hxx>
//local
#include "ICO_Topo_Shape.h"
#include "ICO_Vec.h"

namespace OCCTK {
namespace OCC {
namespace BRepPrimAPI {

MakePrism::MakePrism(Topo::TShape^ shape, gp::Vec vec)
{
    myMaker=new BRepPrimAPI_MakePrism(shape, vec);
}
MakePrism::MakePrism(Topo::TShape^ shape, gp::Vec vec, bool copy)
{
    myMaker = new BRepPrimAPI_MakePrism(shape, vec,copy);
}
MakePrism::MakePrism(Topo::TShape^ shape, gp::Vec vec, bool copy, bool canonize)
{
    myMaker = new BRepPrimAPI_MakePrism(shape, vec,canonize);
}
Topo::TShape^ MakePrism::Shape()
{
   return gcnew Topo::TShape(myMaker->Shape());
}
bool MakePrism::IsDone()
{
    return myMaker->IsDone();
}
bool MakePrism::Error()
{
    return myMaker;
}
}
}
}