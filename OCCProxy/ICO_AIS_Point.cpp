﻿#include "ICO_AIS_Point.h"
#include <AIS_Point.hxx>
#include <Aspect_TypeOfMarker.hxx>
#include <Geom_CartesianPoint.hxx>
//local
#include "ICO_Pnt.h"

namespace OCCTK {
namespace OCC {
namespace AIS {

APoint::APoint(gp::Pnt aPnt) {
    Handle(Geom_Point) geomPoint = new Geom_CartesianPoint(aPnt);
    Handle(AIS_Point) pnt = new AIS_Point(geomPoint);
    pnt->SetMarker(Aspect_TypeOfMarker::Aspect_TOM_O_PLUS);
    NativeHandle = pnt;
}

void APoint::SetWidth(double width) {
    myPnt()->SetWidth(width);
}

void APoint::SetMakerType(Aspect::TypeOfMarker type)
{
    myPnt()->SetMarker(Aspect_TypeOfMarker(type));
}

}
}
}