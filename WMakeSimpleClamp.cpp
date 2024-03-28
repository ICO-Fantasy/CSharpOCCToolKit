#include "WMakeSimpleClamp.h"
#include <TopoDS_Shape.hxx>
#include "MakeSimpleClamp.cpp"
#include <AIS_Shape.hxx>
#include <STEPControl_Reader.hxx>
#include <vector>
#include <map>
#include "String.h"
using namespace OCCTK::Visualization;

namespace OCCTK::Laser
{
	void WMakeSimpleClamp::MakeBasePlate(TopoDS_Shape& BasePlate, TopoDS_Shape& InputFace, TopoDS_Shape& InputWorkpiece, double& theX, double& theY, double& theZ, double& BasePlateLengthX, double& BasePlateLengthY, double OffsetZ, double BasePlateOffsetX, double BasePlateOffsetY, double BasePlateThickness) {
#pragma region readStep
		//生成一个step模型类
		STEPControl_Reader reader;
		//加载一个文件并且返回一个状态枚举值
		//IFSelect_ReturnStatus status = reader.ReadFile("test-B柱_5421231-ED01.stp");
		IFSelect_ReturnStatus status = reader.ReadFile("testWorkPiece.STEP");
		if (status == IFSelect_RetDone)
		{
			reader.TransferRoot(1);
			InputWorkpiece = reader.Shape(1);
		}
#pragma endregion
		InputFace = GetTestFace(InputWorkpiece); //!模拟交互动作，得到下底面
		theZ = GetZFromFace(InputFace) - OffsetZ;
		std::pair<gp_Pnt, gp_Pnt> point_pair = GetCorners(InputWorkpiece);
		gp_Pnt lowerLeftPoint = point_pair.first;
		gp_Pnt topRightPoint = point_pair.second;
		BasePlateLengthX = topRightPoint.X() - lowerLeftPoint.X();
		BasePlateLengthY = topRightPoint.Y() - lowerLeftPoint.Y();
		BasePlate = MakeBasePlateShape(lowerLeftPoint, topRightPoint, theZ, BasePlateThickness, BasePlateOffsetX, BasePlateOffsetY);
	}
	void WMakeSimpleClamp::MakeVerticalPlate(std::pair<std::vector<TopoDS_Shape>, std::vector<TopoDS_Shape>>& VerticalPlates, TopoDS_Shape InputWorkpiece, double theX, double theY, double theZ, double BasePlateLengthX, double BasePlateLengthY, double VerticalPlateThickness, double VerticalPlateInitialOffsetX, double VerticalPlateOffsetX, double VerticalPlateInitialOffsetY, double VerticalPlateOffsetY, double VerticalPlateConnectionHeight, double VerticalPlateClearances, double VerticalPlateMinSupportingLen, double VerticalPlateCuttingDistance) {
		//todo 先做生成竖板、然后加入连接槽和连接处
#pragma region 生成竖板
		std::map<double, TopoDS_Shape> theXSections;
		std::map<double, TopoDS_Shape> theYSections;
#pragma region 沿着X方向生成截平面
		double TempX = theX + VerticalPlateInitialOffsetX;
		double DefaultXmin = 0.0;//todo 默认不生成长度
		while (TempX < BasePlateLengthX - DefaultXmin)
		{
			TopoDS_Face aXPlane = MakeVerticalPlane(TempX, X);
			TopoDS_Shape aXSection = MakeSection(aXPlane, InputWorkpiece);
			theXSections.insert(std::make_pair(TempX, aXSection));
			//VerticalPlates.push_back(aXSection); //test
			TempX = TempX + VerticalPlateOffsetX;
		}
#pragma endregion
#pragma region 沿着Y方向生成截平面
		double TempY = theY + VerticalPlateInitialOffsetY;
		double DefaultYmin = 0.0;//todo 默认不生成长度
		while (TempY < BasePlateLengthY - DefaultYmin)
		{
			TopoDS_Face aYPlane = MakeVerticalPlane(TempY, Y);
			TopoDS_Shape aYSection = MakeSection(aYPlane, InputWorkpiece);
			theYSections.insert(std::make_pair(TempY, aYSection));
			//VerticalPlates.push_back(aYSection); //test
			TempY = TempY + VerticalPlateOffsetY;
		}
#pragma endregion
		for each (auto aSectionPair in theXSections)
		{
			TopoDS_Shape aSection;
			aSection = MakeVerticalPlateWithSection(aSectionPair, X, theYSections, BasePlateLengthY, theX, theY, theZ, VerticalPlateThickness, VerticalPlateConnectionHeight, VerticalPlateClearances, VerticalPlateMinSupportingLen, VerticalPlateCuttingDistance);
			VerticalPlates.first.push_back(aSection);
		}
		for each (auto aSectionPair in theYSections)
		{
			TopoDS_Shape aSection;
			aSection = MakeVerticalPlateWithSection(aSectionPair, Y, theXSections, BasePlateLengthX, theX, theY, theZ, VerticalPlateThickness, VerticalPlateConnectionHeight, VerticalPlateClearances, VerticalPlateMinSupportingLen, VerticalPlateCuttingDistance);
			VerticalPlates.second.push_back(aSection);
		}
#pragma endregion

	}
	WAIS_Shape^ WMakeSimpleClamp::TestInputWorkpiece(String^ filePath) {
		auto cPath = OCCTK::DataExchange::toAsciiString(filePath);
		TopoDS_Shape InputWorkpiece;
#pragma region readStep
		//生成一个step模型类
		STEPControl_Reader reader;
		//加载一个文件并且返回一个状态枚举值
		IFSelect_ReturnStatus status = reader.ReadFile(cPath.ToCString());
		//IFSelect_ReturnStatus status = reader.ReadFile("testWorkPiece.STEP");
		if (status == IFSelect_RetDone)
		{
			reader.TransferRoot(1);
			InputWorkpiece = reader.Shape(1);
		}
#pragma 
		Handle(AIS_Shape) theAIS = new AIS_Shape(InputWorkpiece);
		theAIS->SetColor(Quantity_NOC_GRAY);
		theAIS->SetTransparency();
		WAIS_Shape^ InputAIS = gcnew WAIS_Shape(theAIS);
		return InputAIS;
	}
	BasePlate^ WMakeSimpleClamp::TestMakeBase(WAIS_Shape^ InputAISWorkpiece, WAIS_Shape^ InputAISFace, double OffsetZ, double BasePlateThickness, double BasePlateOffsetX, double BasePlateOffsetY) {
		double theZ, WorkpieceLengthX, WorkpieceLengthY;
		BasePlate^ theBasePlate = gcnew BasePlate;
		TopoDS_Shape InputFace = (*InputAISFace->GetOCC())->Shape();
		TopoDS_Shape InputWorkpiece = (*InputAISWorkpiece->GetOCC())->Shape();
		theZ = GetZFromFace(InputFace) - OffsetZ;
		std::pair<gp_Pnt, gp_Pnt> point_pair = GetCorners(InputWorkpiece);
		gp_Pnt lowerLeftPoint = point_pair.first;
		gp_Pnt topRightPoint = point_pair.second;
		WorkpieceLengthX = topRightPoint.X() - lowerLeftPoint.X();
		WorkpieceLengthY = topRightPoint.Y() - lowerLeftPoint.Y();
		TopoDS_Shape aBasePlateShape = MakeBasePlateShape(lowerLeftPoint, topRightPoint, theZ, BasePlateThickness, BasePlateOffsetX, BasePlateOffsetY);
		theBasePlate->X = lowerLeftPoint.X();
		theBasePlate->Y = lowerLeftPoint.Y();
		theBasePlate->Z = theZ;
		theBasePlate->dX = WorkpieceLengthX;
		theBasePlate->dY = WorkpieceLengthY;
		theBasePlate->OffsetX = BasePlateOffsetX;
		theBasePlate->OffsetY = BasePlateOffsetY;
		Handle(AIS_Shape) theAIS = new AIS_Shape(aBasePlateShape);
		theAIS->SetColor(Quantity_NOC_BLUE);
		theAIS->SetTransparency();
		theBasePlate->shape = gcnew WAIS_Shape(theAIS);
		return theBasePlate;
	}
	BasePlate^ WMakeSimpleClamp::TestMakeBase_NoSelect(WAIS_Shape^ InputAISWorkpiece, double OffsetZ, double BasePlateThickness, double BasePlateOffsetX, double BasePlateOffsetY) {
		double  WorkpieceLengthX, WorkpieceLengthY;
		BasePlate^ theBasePlate = gcnew BasePlate;
		TopoDS_Shape InputWorkpiece = (*InputAISWorkpiece->GetOCC())->Shape();
		std::pair<gp_Pnt, gp_Pnt> point_pair = GetCorners(InputWorkpiece);
		gp_Pnt lowerLeftPoint = point_pair.first;
		gp_Pnt topRightPoint = point_pair.second;
		WorkpieceLengthX = topRightPoint.X() - lowerLeftPoint.X();
		WorkpieceLengthY = topRightPoint.Y() - lowerLeftPoint.Y();
		TopoDS_Shape aBasePlateShape = MakeBasePlateShape(lowerLeftPoint, topRightPoint, lowerLeftPoint.Z(), BasePlateThickness, BasePlateOffsetX, BasePlateOffsetY);
		theBasePlate->X = lowerLeftPoint.X();
		theBasePlate->Y = lowerLeftPoint.Y();
		theBasePlate->Z = lowerLeftPoint.Z();
		theBasePlate->dX = WorkpieceLengthX;
		theBasePlate->dY = WorkpieceLengthY;
		theBasePlate->OffsetX = BasePlateOffsetX;
		theBasePlate->OffsetY = BasePlateOffsetY;
		Handle(AIS_Shape) theAIS = new AIS_Shape(aBasePlateShape);
		theAIS->SetColor(Quantity_NOC_BLUE);
		theAIS->SetTransparency();
		theBasePlate->shape = gcnew WAIS_Shape(theAIS);
		return theBasePlate;
	}
	List<WAIS_Shape^>^ WMakeSimpleClamp::TestMakeVertical(WAIS_Shape^ InputAISWorkpiece, BasePlate^ theBasePlate, VerticalPlateDirection theVDir, double theValue, double VerticalPlateClearances, double VerticalPlateMinSupportingLen, double VerticalPlateCuttingDistance) {
		TopoDS_Shape InputWorkpiece = (*InputAISWorkpiece->GetOCC())->Shape();
		List<WAIS_Shape^>^ result = gcnew List<WAIS_Shape^>();
		switch (theVDir)
		{
		case OCCTK::Laser::VerticalPlateDirection::X:
		{
			TopoDS_Face aXPlane = MakeVerticalPlane(theValue, X);
			TopoDS_Shape aXSection = MakeSection(aXPlane, InputWorkpiece);
			//result->Add(gcnew WAIS_Shape(aXSection));//test
			std::vector<TopoDS_Shape> thePieces = MakeVerticalPieceWithSection(aXSection, X, theBasePlate->Z, VerticalPlateClearances, VerticalPlateMinSupportingLen, VerticalPlateCuttingDistance);
			for (AIS_Shape aV : thePieces)
			{
				WAIS_Shape^ theAISV = gcnew WAIS_Shape(aV);
				result->Add(theAISV);
			}
		}
		break;
		case OCCTK::Laser::VerticalPlateDirection::Y:
		{
			TopoDS_Face aYPlane = MakeVerticalPlane(theValue, Y);
			TopoDS_Shape aYSection = MakeSection(aYPlane, InputWorkpiece);
			std::vector<TopoDS_Shape> thePieces = MakeVerticalPieceWithSection(aYSection, Y, theBasePlate->Z, VerticalPlateClearances, VerticalPlateMinSupportingLen, VerticalPlateCuttingDistance);
			for (AIS_Shape aV : thePieces)
			{
				WAIS_Shape^ theAISV = gcnew WAIS_Shape(aV);
				result->Add(theAISV);
			}
		}
		break;
		default:
			break;
		}
		return result;
	}
}
