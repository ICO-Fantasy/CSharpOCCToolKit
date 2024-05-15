#include "WMakeSimpleClamp.h"
#include <TopoDS_Shape.hxx>
#include "MakeSimpleClamp.h"
#include <AIS_Shape.hxx>
#include <STEPControl_Reader.hxx>
#include <vector>
#include <map>
#include "String.h"
using namespace System::Collections::Generic;

namespace OCCTK::Laser {
//	void WMakeSimpleClamp::MakeVerticalPlate(std::pair<std::vector<TopoDS_Shape>, std::vector<TopoDS_Shape>>& VerticalPlates, TopoDS_Shape InputWorkpiece, double theX, double theY, double theZ, double BasePlateLengthX, double BasePlateLengthY, double VerticalPlateThickness, double VerticalPlateInitialOffsetX, double VerticalPlateOffsetX, double VerticalPlateInitialOffsetY, double VerticalPlateOffsetY, double VerticalPlateConnectionHeight, double VerticalPlateClearances, double VerticalPlateMinSupportingLen, double VerticalPlateCuttingDistance) {
//		//todo 先做生成竖板、然后加入连接槽和连接处
//#pragma region 生成竖板
//		std::map<double, TopoDS_Shape> theXSections;
//		std::map<double, TopoDS_Shape> theYSections;
//#pragma region 沿着X方向生成截平面
//		double TempX = theX + VerticalPlateInitialOffsetX;
//		double DefaultXmin = 0.0;//todo 默认不生成长度
//		while (TempX < BasePlateLengthX - DefaultXmin) {
//			TopoDS_Face aXPlane = MakeVerticalPlane(TempX, X);
//			TopoDS_Shape aXSection = MakeSection(aXPlane, InputWorkpiece);
//			theXSections.insert(std::make_pair(TempX, aXSection));
//			//VerticalPlates.push_back(aXSection); //test
//			TempX = TempX + VerticalPlateOffsetX;
//		}
//#pragma endregion
//#pragma region 沿着Y方向生成截平面
//		double TempY = theY + VerticalPlateInitialOffsetY;
//		double DefaultYmin = 0.0;//todo 默认不生成长度
//		while (TempY < BasePlateLengthY - DefaultYmin) {
//			TopoDS_Face aYPlane = MakeVerticalPlane(TempY, Y);
//			TopoDS_Shape aYSection = MakeSection(aYPlane, InputWorkpiece);
//			theYSections.insert(std::make_pair(TempY, aYSection));
//			//VerticalPlates.push_back(aYSection); //test
//			TempY = TempY + VerticalPlateOffsetY;
//		}
//#pragma endregion
//		for each (auto aSectionPair in theXSections) {
//			TopoDS_Shape aSection;
//			aSection = MakeVerticalPlateWithSection(aSectionPair, X, theYSections, BasePlateLengthY, theX, theY, theZ, VerticalPlateThickness, VerticalPlateConnectionHeight, VerticalPlateClearances, VerticalPlateMinSupportingLen, VerticalPlateCuttingDistance);
//			VerticalPlates.first.push_back(aSection);
//		}
//		for each (auto aSectionPair in theYSections) {
//			TopoDS_Shape aSection;
//			aSection = MakeVerticalPlateWithSection(aSectionPair, Y, theXSections, BasePlateLengthX, theX, theY, theZ, VerticalPlateThickness, VerticalPlateConnectionHeight, VerticalPlateClearances, VerticalPlateMinSupportingLen, VerticalPlateCuttingDistance);
//			VerticalPlates.second.push_back(aSection);
//		}
//#pragma endregion
//
//	}

/// <summary>
/// 委托结构和cpp结构的转换
/// </summary>
/// <returns></returns>
OCCTK::SimpleClamp::BasePlate BasePlate::GetOCC()
{
	//获取TopoDS_Shape *指针需要如下步骤
	//auto a = this->workpiece;
	//auto b = a->GetOCC();
	//auto c = *b;
	//auto e = c->Shape();
	//auto f = &e;
	TopoDS_Shape* workpiecePnt = const_cast<TopoDS_Shape*>(&(*(this->workpiece)->GetOCC())->Shape());

	OCCTK::SimpleClamp::BasePlate occBasePlate{
		workpiecePnt,
		(*(this->shape)->GetOCC())->Shape() ,
		this->X,
		this->Y,
		this->Z,
		this->dX,
		this->dY,
		this->OffsetX,
		this->OffsetY,
	};

	return occBasePlate;
}

/// <summary>
/// 委托结构和cpp结构的转换
/// </summary>
/// <returns></returns>
OCCTK::SimpleClamp::VerticalPlate VerticalPlate::GetOCC()
{
	TopoDS_Shape* workpiecePtr = const_cast<TopoDS_Shape*>(&(*(this->workpiece)->GetOCC())->Shape());
	OCCTK::SimpleClamp::BasePlate baseplate = this->baseplate->GetOCC();
	OCCTK::SimpleClamp::BasePlate* baseplatePtr = &(baseplate);
	TopoDS_Shape VerticalPlateShape;
	if (this->shape != nullptr)
	{
		VerticalPlateShape = (*(this->workpiece)->GetOCC())->Shape();
	}
	OCCTK::SimpleClamp::Direction thedir;
	switch (this->direction)
	{
	case OCCTK::Laser::Direction::X:
		thedir = OCCTK::SimpleClamp::Direction::X;
		break;
	case OCCTK::Laser::Direction::Y:
		thedir = OCCTK::SimpleClamp::Direction::Y;
		break;
	default:
		throw std::runtime_error("Unexpected Direction in switch");
	}

	OCCTK::SimpleClamp::VerticalPlate occVerticalPlate{
		workpiecePtr,
		baseplatePtr,
		TopoDS_Shape(),//(*(this->shape)->GetOCC())->Shape() ,
		std::vector<OCCTK::SimpleClamp::VerticalPiece>(),
		thedir,
		this->location,
		this->clearances,
		this->minSupportLen,
		this->cuttingDistance,
	};
	return occVerticalPlate;
}

#pragma region demo测试用方法
WAIS_Shape^ WMakeSimpleClamp::TestInputWorkpiece(String^ filePath) {
	auto cPath = OCCTK::DataExchange::toAsciiString(filePath);
	TopoDS_Shape InputWorkpiece;
#pragma region readStep

	//生成一个step模型类
	STEPControl_Reader reader;
	//加载一个文件并且返回一个状态枚举值
	IFSelect_ReturnStatus status = reader.ReadFile(cPath.ToCString());
	//IFSelect_ReturnStatus status = reader.ReadFile("testWorkPiece.STEP");
	if (status == IFSelect_RetDone) {
		reader.TransferRoot(1);
		InputWorkpiece = reader.Shape(1);
	}

#pragma endregion

	Handle(AIS_Shape) theAIS = new AIS_Shape(InputWorkpiece);
	theAIS->SetColor(Quantity_NOC_GRAY);
	theAIS->SetTransparency();
	WAIS_Shape^ InputAIS = gcnew WAIS_Shape(theAIS);
	return InputAIS;
}
#pragma endregion


/// <summary>
/// 非交互的创建底板
/// </summary>
/// <param name="InputAISWorkpiece"></param>
/// <param name="OffsetZ"></param>
/// <param name="BasePlateOffsetX"></param>
/// <param name="BasePlateOffsetY"></param>
/// <returns></returns>
BasePlate^ WMakeSimpleClamp::MakeBasePlate_NoInteract(WAIS_Shape^ InputAISWorkpiece, double OffsetZ, double BasePlateOffsetX, double BasePlateOffsetY) {
	TopoDS_Shape InputWorkpiece = (*InputAISWorkpiece->GetOCC())->Shape();

	OCCTK::SimpleClamp::BasePlate theCPPBasePlate = OCCTK::SimpleClamp::MakeBasePlate(InputWorkpiece, OffsetZ, BasePlateOffsetX, BasePlateOffsetY);

	BasePlate^ theBasePlate = gcnew BasePlate();
	theBasePlate->shape = gcnew WAIS_Shape(theCPPBasePlate.shape);
	theBasePlate->X = theCPPBasePlate.X;
	theBasePlate->Y = theCPPBasePlate.Y;
	theBasePlate->Z = theCPPBasePlate.Z;
	theBasePlate->dX = theCPPBasePlate.dX;
	theBasePlate->dY = theCPPBasePlate.dY;
	theBasePlate->OffsetX = BasePlateOffsetX;
	theBasePlate->OffsetY = BasePlateOffsetY;

	return theBasePlate;
}

/// <summary>
/// 在指定位置创建一块竖板
/// </summary>
/// <param name="InputAISWorkpiece"></param>
/// <param name="BasePlate"></param>
/// <param name="theLocation"></param>
/// <param name="theMinSupportLen"></param>
/// <param name="theClearances"></param>
/// <param name="theCuttingDistance"></param>
/// <returns></returns>
VerticalPlate^ WMakeSimpleClamp::MakeVerticalPlate(WAIS_Shape^ InputAISWorkpiece, BasePlate^ BasePlate, Direction theDirection, double theLocation, double theClearances, double theMinSupportLen, double theCuttingDistance)
{
	VerticalPlate^ onePlate = gcnew VerticalPlate();
	OCCTK::SimpleClamp::VerticalPlate cppPlate;
	TopoDS_Shape InputWorkpiece = (*InputAISWorkpiece->GetOCC())->Shape();
	switch (theDirection)
	{
	case OCCTK::Laser::Direction::X:
		cppPlate = OCCTK::SimpleClamp::MakeVerticalPlate(InputWorkpiece, BasePlate->GetOCC(), OCCTK::SimpleClamp::Direction::X, theLocation, theClearances, theMinSupportLen, theCuttingDistance);
		break;
	case OCCTK::Laser::Direction::Y:
		cppPlate = OCCTK::SimpleClamp::MakeVerticalPlate(InputWorkpiece, BasePlate->GetOCC(), OCCTK::SimpleClamp::Direction::Y, theLocation, theClearances, theMinSupportLen, theCuttingDistance);
		break;
	default:
		break;
	}
	//把CPP结构体封装为委托结构体
	onePlate->direction = theDirection;
	onePlate->location = theLocation;
	for (auto oneCPPPiece : cppPlate.pieces)
	{
		VerticalPiece^ onePiece = gcnew VerticalPiece();

		onePiece->shape = gcnew WAIS_Shape(oneCPPPiece.Shape);
		onePiece->start; //todo 应该换成Pnt
		onePiece->end; //todo 应该换成Pnt
		onePiece->dir = theDirection; // 没必要每一个都取，同一块板内的piece方向是统一的

		onePlate->pieces->Add(onePiece);
	}
	onePlate->clearances = theClearances;
	onePlate->minSupportLen = theMinSupportLen;
	onePlate->cuttingDistance = theCuttingDistance;
	// onePlate->connectionHeight 在此还没用到;
	return onePlate;
}

/// <summary>
/// 创建所有竖板
/// </summary>
/// <param name="InputAISWorkpiece"></param>
/// <param name="BasePlate"></param>
/// <param name="XLocation"></param>
/// <param name="YLocation"></param>
/// <param name="theClearances"></param>
/// <param name="theMinSupportLen"></param>
/// <param name="theCuttingDistance"></param>
/// <returns></returns>
List<VerticalPlate^>^ WMakeSimpleClamp::MakeVerticalPlates(WAIS_Shape^ InputAISWorkpiece, BasePlate^ BasePlate, List<double>^ XLocation, List<double>^ YLocation, double theClearances, double theMinSupportLen, double theCuttingDistance)
{
	List<VerticalPlate^>^ Plates = gcnew List<VerticalPlate^>(); // result
	TopoDS_Shape InputWorkpiece = (*InputAISWorkpiece->GetOCC())->Shape();

	for each (double aXLoc in XLocation)
	{
		VerticalPlate^ aXPlate = WMakeSimpleClamp::MakeVerticalPlate(InputAISWorkpiece, BasePlate, Direction::X, aXLoc, theClearances, theMinSupportLen, theCuttingDistance);
		Plates->Add(aXPlate);
	}
	for each (double aYLoc in YLocation)
	{
		VerticalPlate^ aYPlate = WMakeSimpleClamp::MakeVerticalPlate(InputAISWorkpiece, BasePlate, Direction::Y, aYLoc, theClearances, theMinSupportLen, theCuttingDistance);
		Plates->Add(aYPlate);
	}
	return Plates;
}

List<VerticalPlate^>^ WMakeSimpleClamp::MakeVerticalPlates(WAIS_Shape^ InputAISWorkpiece, BasePlate^ BasePlate, int XNum, int YNum, double initialOffsetX, double initialOffsetY, double theClearances, double theMinSupportLen, double theCuttingDistance)
{
	// 计算 XLocation
	double XLoc = BasePlate->X + initialOffsetX;
	List<double>^ XLocation = gcnew List<double>();
	XLocation->Add(XLoc);
	//两端始终要有两块，由initialOffset控制，中间会有Num-2块，位置均分
	for (int i = 0; i < XNum - 2; ++i) {
		XLoc += BasePlate->dX / (XNum - 2);
		XLocation->Add(XLoc);
	}
	XLocation->Add(BasePlate->X + BasePlate->dX - initialOffsetX);

	//计算 YLocation
	double YLoc = BasePlate->Y + initialOffsetY;
	List<double>^ YLocation = gcnew List<double>();
	YLocation->Add(YLoc);
	//两端始终要有两块，由initialOffset控制，中间会有Num-2块，位置均分
	for (int i = 0; i < YNum - 2; ++i) {
		YLoc += BasePlate->dY / (YNum - 2);
		YLocation->Add(YLoc);
	}
	YLocation->Add(BasePlate->Y + BasePlate->dY - initialOffsetY);

	return MakeVerticalPlates(InputAISWorkpiece, BasePlate, XLocation, YLocation, theClearances, theMinSupportLen, theCuttingDistance);
}

void WMakeSimpleClamp::SuturePLate(VerticalPlate^& theVerticalPlate, double theConnectHight)
{
	theVerticalPlate->connectionHeight = theConnectHight;
	auto theoccVP = theVerticalPlate->GetOCC();
	OCCTK::SimpleClamp::SuturePiece(theoccVP, theConnectHight);
	//shape写回对象中
	theVerticalPlate->shape = gcnew WAIS_Shape(theoccVP.shape);//todo 此处应该替换为TopoDS_Shape
}





}
