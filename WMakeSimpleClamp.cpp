#include "WMakeSimpleClamp.h"
#include <TopoDS_Shape.hxx>
#include "MakeSimpleClamp.cpp"
#include <AIS_Shape.hxx>
#include <STEPControl_Reader.hxx>
#include <vector>
#include <map>
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
void WMakeSimpleClamp::MakeVerticalPlate(std::vector<TopoDS_Shape>& VerticalPlates, TopoDS_Shape InputWorkpiece, double theX, double theY, double theZ, double BasePlateLengthX, double BasePlateLengthY, double VerticalPlateThickness, double VerticalPlateInitialOffsetX, double VerticalPlateOffsetX, double VerticalPlateInitialOffsetY, double VerticalPlateOffsetY, double VerticalPlateConnectionHeight, double VerticalPlateClearances, double VerticalPlateMinSupportingLen, double VerticalPlateCuttingDistance) {
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
		VerticalPlates.push_back(aSection);
	}
	return;
	for each (auto aSectionPair in theYSections)
	{
		TopoDS_Shape aSection;
		aSection = MakeVerticalPlateWithSection(aSectionPair, Y, theXSections, BasePlateLengthX, theX, theY, theZ, VerticalPlateThickness, VerticalPlateConnectionHeight, VerticalPlateClearances, VerticalPlateMinSupportingLen, VerticalPlateCuttingDistance);
		VerticalPlates.push_back(aSection);
	}
	#pragma endregion

}
//	throw gcnew System::NotImplementedException();
//
//}
}
