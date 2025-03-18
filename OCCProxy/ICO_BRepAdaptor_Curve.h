#pragma once
#include <BRepAdaptor_Curve.hxx>
//Local
#include "ICO_BaseObject.h"


namespace OCCTK {
namespace OCC {
namespace Topo {
ref class TEdge;
ref class TFace;
}
namespace gp {
value struct Pnt;
value struct Circle;
}
namespace GeomAbs {
enum class CurveType;
}
}
}


namespace OCCTK {
namespace OCC {
namespace BRepAdaptor {
public ref class Curve : BaseObject {
private:
    Handle(BRepAdaptor_Curve) myCure() { return Handle(BRepAdaptor_Curve)::DownCast(NativeHandle); }
public:
    Curve();
    Curve(Topo::TEdge^ edge);
    Curve(Topo::TEdge^ edge, Topo::TFace^ face);
    Handle(BRepAdaptor_Curve) GetOCC() { return myCure(); }
    //! 隐式转换为 Handle(BRepAdaptor_Curve)
    static operator Handle(BRepAdaptor_Curve) (Curve^ c) { return c->GetOCC(); }
    //! 隐式转换为 Handle(BRepAdaptor_Curve)
    static operator BRepAdaptor_Curve (Curve^ c) { return *c->GetOCC(); }
public:
    gp::Pnt Value(double UVValue);
    double FirstParameter();
    double LastParameter();
    GeomAbs::CurveType GetType();
    gp::Circle Circle();
    property GeomAbs::CurveType Type {GeomAbs::CurveType get() { return GetType(); }}
};

}
}
}

