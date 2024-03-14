#pragma once
#include "CSharpViewer.h"
#include <Standard_Handle.hxx>
#include <AIS_Shape.hxx>
namespace OCCTK::Laser
{
public ref class WMakeSimpleClamp {

public:
	void MakeBasePlate(TopoDS_Shape& BasePlate, TopoDS_Shape& InputFace, TopoDS_Shape& InputWorkpiece, double& theX, double& theY, double& theZ, double& BasePlateLengthX, double& BasePlateLengthY, double OffsetZ, double BasePlateOffsetX, double BasePlateOffsetY, double BasePlateThickness);
	void MakeVerticalPlate(std::vector<TopoDS_Shape>& VerticalPlates, TopoDS_Shape InputWorkpiece, double theX, double theY, double theZ, double BasePlateLengthX, double BasePlateLengthY, double VerticalPlateThickness, double VerticalPlateInitialOffsetX, double VerticalPlateOffsetX, double VerticalPlateInitialOffsetY, double VerticalPlateOffsetY, double VerticalPlateConnectionHeight, double VerticalPlateClearances, double VerticalPlateMinSupportingLen, double VerticalPlateCuttingDistance);
};
}

