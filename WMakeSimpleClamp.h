#pragma once
#include "CSharpViewer.h"
#include <Standard_Handle.hxx>
#include <AIS_Shape.hxx>
#include "WAIS_Shape.h"
#include <STEPControl_Reader.hxx>
using namespace  OCCTK::OCC::AIS;
using namespace System::Collections::Generic;
namespace OCCTK::Laser
{
	public ref struct BasePlate {
		WAIS_Shape^ shape;
		double X;
		double Y;
		double Z;
		double dX;
		double dY;
		double OffsetX;
		double OffsetY;
	};
	public enum class  VerticalPlateDirection {
		X,
		Y
	};
	public ref class WMakeSimpleClamp {
	public:
		void MakeBasePlate(TopoDS_Shape& BasePlate, TopoDS_Shape& InputFace, TopoDS_Shape& InputWorkpiece, double& theX, double& theY, double& theZ, double& BasePlateLengthX, double& BasePlateLengthY, double OffsetZ, double BasePlateOffsetX, double BasePlateOffsetY, double BasePlateThickness);
		void MakeVerticalPlate(std::pair<std::vector<TopoDS_Shape>, std::vector<TopoDS_Shape>>& VerticalPlates, TopoDS_Shape InputWorkpiece, double theX, double theY, double theZ, double BasePlateLengthX, double BasePlateLengthY, double VerticalPlateThickness, double VerticalPlateInitialOffsetX, double VerticalPlateOffsetX, double VerticalPlateInitialOffsetY, double VerticalPlateOffsetY, double VerticalPlateConnectionHeight, double VerticalPlateClearances, double VerticalPlateMinSupportingLen, double VerticalPlateCuttingDistance);
		static WAIS_Shape^ TestInputWorkpiece(String^ path);
		static BasePlate^ TestMakeBase(WAIS_Shape^ InputAISWorkpiece, WAIS_Shape^ InputAISFace, double OffsetZ, double BasePlateThickness, double BasePlateOffsetX, double BasePlateOffsetY);
		static BasePlate^ TestMakeBase_NoSelect(WAIS_Shape^ InputAISWorkpiece, double OffsetZ, double BasePlateThickness, double BasePlateOffsetX, double BasePlateOffsetY);
		static List<WAIS_Shape^>^ TestMakeVertical(WAIS_Shape^ InputAISWorkpiece, BasePlate^ theBasePlate, VerticalPlateDirection theVDir, double theValue, double VerticalPlateClearances, double VerticalPlateMinSupportingLen, double VerticalPlateCuttingDistance);
	};
}

