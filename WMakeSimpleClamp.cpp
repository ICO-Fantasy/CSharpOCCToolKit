#include "WMakeSimpleClamp.h"
#include <TopoDS_Shape.hxx>
#include "MakeSimpleClamp.h"
#include <AIS_Shape.hxx>
#include <STEPControl_Reader.hxx>
#include <vector>
#include <TopoDS.hxx>
#include "String.h"
using namespace System::Collections::Generic;

namespace OCCTK::Laser {

/// <summary>
/// 委托结构和cpp结构的转换
/// </summary>
/// <returns></returns>
OCCTK::SimpleClamp::BasePlate BasePlate::GetOCC()
{
	OCCTK::SimpleClamp::BasePlate occBasePlate{
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
	//TopoDS_Shape shape;
	TopoDS_Shape VerticalPlateShape = TopoDS_Shape();
	if (this->shape != nullptr)
	{
		VerticalPlateShape = (*(this->shape)->GetOCC())->Shape();
	}

	//std::vector<VerticalPiece> pieces;
	std::vector<OCCTK::SimpleClamp::VerticalPiece> occpieces = std::vector<OCCTK::SimpleClamp::VerticalPiece>();
	for each (VerticalPiece ^ oneoccPiece in pieces)
	{
		occpieces.push_back(oneoccPiece->GetOCC());
	}

	//Direction dir;
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
		VerticalPlateShape,
		occpieces,
		thedir,
		this->location,
		this->clearances,
		this->minSupportLen,
		this->cuttingDistance,
		this->connectionHeight,
	};
	return occVerticalPlate;
}

OCCTK::SimpleClamp::VerticalPiece VerticalPiece::GetOCC()
{
	//Direction dir;
	OCCTK::SimpleClamp::Direction thedir;
	switch (dir)
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
	//TopoDS_Shape Shape;
	TopoDS_Shape theshape = (*(shape->GetOCC()))->Shape();
	//TopoDS_Edge Edge;
	TopoDS_Shape theedgeshape = (*(edge->GetOCC()))->Shape();//todo 这个转换可以不用
	TopoDS_Edge theedge = TopoDS::Edge(theedgeshape);//todo 这个转换可以不用
	OCCTK::SimpleClamp::VerticalPiece occVerticalPiece{
		thedir,
		theshape,
		theedge,
	};
	return occVerticalPiece;
}

//把CPP结构体封装为委托结构体
static auto getDelegate(OCCTK::SimpleClamp::BasePlate cppBase) {
	BasePlate^ theBase = gcnew BasePlate();
	theBase->shape = gcnew WAIS_Shape(cppBase.shape);//todo 需要替换为TopoDS_Shape
	theBase->X = cppBase.X;
	theBase->Y = cppBase.Y;
	theBase->Z = cppBase.Z;
	theBase->dX = cppBase.dX;
	theBase->dY = cppBase.dY;
	theBase->OffsetX = cppBase.OffsetX;
	theBase->OffsetY = cppBase.OffsetY;

	return theBase;
}
//把CPP结构体封装为委托结构体
static auto getDelegate(OCCTK::SimpleClamp::VerticalPiece cppPiece) {
	VerticalPiece^ thePiece = gcnew VerticalPiece();
	switch (cppPiece.dir)
	{
	case OCCTK::SimpleClamp::Direction::X:
		thePiece->dir = Direction::X;
		break;
	case OCCTK::SimpleClamp::Direction::Y:
		thePiece->dir = Direction::Y;
		break;
	default:
		break;
	}
	thePiece->shape = gcnew WAIS_Shape(cppPiece.Shape);
	thePiece->edge = gcnew WAIS_Shape(cppPiece.Edge);
	thePiece->start = cppPiece.StartPoint().X(); //todo 需要换成Wgp_Pnt
	thePiece->end = cppPiece.StartPoint().Y(); //todo 需要换成Wgp_Pnt
	return thePiece;
}
//把CPP结构体封装为委托结构体
static auto getDelegate(OCCTK::SimpleClamp::VerticalPlate cppPlate) {
	VerticalPlate^ thePlate = gcnew VerticalPlate();

	switch (cppPlate.dir)
	{
	case OCCTK::SimpleClamp::Direction::X:
		thePlate->direction = Direction::X;
		break;
	case OCCTK::SimpleClamp::Direction::Y:
		thePlate->direction = Direction::Y;
		break;
	default:
		break;
	}
	thePlate->location = cppPlate.location;
	thePlate->pieces = gcnew List<VerticalPiece^>();
	for (auto oneCPPPiece : cppPlate.pieces)
	{
		thePlate->pieces->Add(getDelegate(oneCPPPiece));
	}
	thePlate->clearances = cppPlate.clearances;
	thePlate->minSupportLen = cppPlate.minSupportLen;
	thePlate->cuttingDistance = cppPlate.cuttiDistance;
	thePlate->connectionHeight = cppPlate.connectionHeight;
	thePlate->shape = gcnew WAIS_Shape(cppPlate.shape);

	return thePlate;
}

#pragma region demo测试用方法
WAIS_Shape^ SimpleClampMaker::TestInputWorkpiece(String^ filePath) {
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
BasePlate^ SimpleClampMaker::MakeBasePlate_NoInteract(WAIS_Shape^ InputAISWorkpiece, double OffsetZ, double BasePlateOffsetX, double BasePlateOffsetY) {
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
VerticalPlate^ SimpleClampMaker::MakeVerticalPlate(WAIS_Shape^ InputAISWorkpiece, BasePlate^ BasePlate, Direction theDirection, double theLocation, double theClearances, double theMinSupportLen, double theCuttingDistance)
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
	onePlate->pieces = gcnew List<VerticalPiece^>();
	for (auto oneCPPPiece : cppPlate.pieces)
	{
		VerticalPiece^ onePiece = gcnew VerticalPiece();

		onePiece->dir = theDirection; // 没必要每一个都取，同一块板内的piece方向是统一的
		onePiece->shape = gcnew WAIS_Shape(oneCPPPiece.Shape);//todo 需要替换为TopoDS_Shape
		onePiece->edge = gcnew WAIS_Shape(oneCPPPiece.Edge);//todo 需要替换为TopoDS_Edge
		//这里应该区分是X还是Y，换成Pnt就不需要了
		onePiece->start = oneCPPPiece.StartPoint().X(); //todo 应该换成Pnt
		onePiece->end = oneCPPPiece.StartPoint().Y(); //todo 应该换成Pnt

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
List<VerticalPlate^>^ SimpleClampMaker::MakeVerticalPlates(WAIS_Shape^ InputAISWorkpiece, BasePlate^ BasePlate, List<double>^ XLocation, List<double>^ YLocation, double theClearances, double theMinSupportLen, double theCuttingDistance)
{
	List<VerticalPlate^>^ Plates = gcnew List<VerticalPlate^>(); // result
	TopoDS_Shape InputWorkpiece = (*InputAISWorkpiece->GetOCC())->Shape();

	for each (double aXLoc in XLocation)
	{
		VerticalPlate^ aXPlate = MakeVerticalPlate(InputAISWorkpiece, BasePlate, Direction::X, aXLoc, theClearances, theMinSupportLen, theCuttingDistance);
		Plates->Add(aXPlate);
	}
	for each (double aYLoc in YLocation)
	{
		VerticalPlate^ aYPlate = MakeVerticalPlate(InputAISWorkpiece, BasePlate, Direction::Y, aYLoc, theClearances, theMinSupportLen, theCuttingDistance);
		Plates->Add(aYPlate);
	}
	return Plates;
}

/// <summary>
/// 根据X、Y数目创建所有竖板
/// </summary>
/// <param name="InputAISWorkpiece"></param>
/// <param name="BasePlate"></param>
/// <param name="XNum"></param>
/// <param name="YNum"></param>
/// <param name="initialOffsetX"></param>
/// <param name="initialOffsetY"></param>
/// <param name="theClearances"></param>
/// <param name="theMinSupportLen"></param>
/// <param name="theCuttingDistance"></param>
/// <returns></returns>
List<VerticalPlate^>^ SimpleClampMaker::MakeVerticalPlates(WAIS_Shape^ InputAISWorkpiece, BasePlate^ BasePlate, int XNum, int YNum, double initialOffsetX, double initialOffsetY, double theClearances, double theMinSupportLen, double theCuttingDistance)
{
	// 计算 XLocation
	double XLoc = BasePlate->X + initialOffsetX;
	List<double>^ XLocation = gcnew List<double>();
	XLocation->Add(XLoc);
	//两端始终要有两块，由initialOffset控制，中间会有Num-2块，位置均分
	for (int i = 0; i < XNum - 2; ++i) {
		XLoc += (BasePlate->dX - initialOffsetX * 2) / (XNum - 2 + 1);
		XLocation->Add(XLoc);
	}
	XLocation->Add(BasePlate->X + BasePlate->dX - initialOffsetX);

	//计算 YLocation
	double YLoc = BasePlate->Y + initialOffsetY;
	List<double>^ YLocation = gcnew List<double>();
	YLocation->Add(YLoc);
	//两端始终要有两块，由initialOffset控制，中间会有Num-2块，位置均分
	for (int i = 0; i < YNum - 2; ++i) {
		YLoc += (BasePlate->dY - initialOffsetY * 2) / (YNum - 2 + 1);
		YLocation->Add(YLoc);
	}
	YLocation->Add(BasePlate->Y + BasePlate->dY - initialOffsetY);

	return MakeVerticalPlates(InputAISWorkpiece, BasePlate, XLocation, YLocation, theClearances, theMinSupportLen, theCuttingDistance);
}

/// <summary>
/// 缝合Piece为整块板
/// </summary>
/// <param name="theVerticalPlate"></param>
/// <param name="theConnectHight"></param>
void SimpleClampMaker::SuturePLate(VerticalPlate^% theVerticalPlate, BasePlate^ BasePlate, double theConnectHight)
{
	theVerticalPlate->connectionHeight = theConnectHight;
	OCCTK::SimpleClamp::VerticalPlate theoccVP = theVerticalPlate->GetOCC();
	OCCTK::SimpleClamp::BasePlate theoccBP = BasePlate->GetOCC();
	OCCTK::SimpleClamp::SuturePiece(theoccVP, theoccBP, theConnectHight);
	//shape写回对象中
	theVerticalPlate->shape = gcnew WAIS_Shape(theoccVP.shape);//todo 此处应该替换为TopoDS_Shape
}

List<VerticalPlate^>^ SimpleClampMaker::SuturePLates(List<VerticalPlate^>^ verticalPlates, BasePlate^ BasePlate, double theConnectHight, double theConnectThickness, double theFilletRadius)
{
	List<VerticalPlate^>^ plates = gcnew List<VerticalPlate^>();
	//把X和Y方向的竖板分为两组，得到其位置
	std::vector<double> XLocs, YLocs;
	for each (auto oneVP in verticalPlates)
	{
		if (oneVP->direction == Direction::X)
		{
			double l = oneVP->location;
			XLocs.push_back(l);
		}
		if (oneVP->direction == Direction::Y)
		{
			double l = oneVP->location;
			YLocs.push_back(l);
		}
	}
#pragma region 在每块板的每个对应位置开槽
	int testNum = 0;
	for each (auto theP in verticalPlates) {
		auto oneVP = theP;
		//先完成板的连接
		SuturePLate(oneVP, BasePlate, theConnectHight);
		OCCTK::SimpleClamp::VerticalPlate theoccVP = oneVP->GetOCC();
		OCCTK::SimpleClamp::BasePlate theoccBP = BasePlate->GetOCC();
		switch (oneVP->direction)
		{
		case Direction::X:
			OCCTK::SimpleClamp::Slotting(theoccVP, theoccBP, YLocs, theConnectHight, theConnectThickness, theFilletRadius);
			break;
		case Direction::Y:
			OCCTK::SimpleClamp::Slotting(theoccVP, theoccBP, XLocs, theConnectHight, theConnectThickness, theFilletRadius);
			break;
		default:
			break;
		}
		plates->Add(getDelegate(theoccVP));

	}
#pragma endregion

	return plates;
}

WAIS_Shape^ SimpleClampMaker::DeployPlates(List<VerticalPlate^>^ verticalPlates, BasePlate^ BasePlate) {
	OCCTK::SimpleClamp::BasePlate theoccBP = BasePlate->GetOCC();
	std::vector<OCCTK::SimpleClamp::VerticalPlate> cppPlates;
	for each (auto onePlate in verticalPlates)
	{
		cppPlates.push_back(onePlate->GetOCC());
	}
	TopoDS_Shape result = OCCTK::SimpleClamp::DeployPlates(theoccBP, cppPlates);
	return gcnew WAIS_Shape(result);
}

bool SimpleClampMaker::SaveSTEP(WAIS_Shape^ theShape, String^ filePath)
{
	TopoDS_Shape theoccShape = (*(theShape->GetOCC()))->Shape();
	auto theStr = OCCTK::DataExchange::toAsciiString(filePath);
	if (OCCTK::SimpleClamp::saveSTEP(theoccShape, theStr))
	{
		return true;
	}
	return false;
}


}
