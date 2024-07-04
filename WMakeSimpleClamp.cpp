﻿#include "WMakeSimpleClamp.h"
#include <TopoDS_Shape.hxx>
#include "MakeSimpleClamp.h"
#include <vector>
#include <TopoDS.hxx>
using namespace System::Collections::Generic;

namespace OCCTK::Laser {

#pragma region 互相转换

// 委托结构和cpp结构的转换
OCCTK::SimpleClamp::BasePlate BasePlate::GetOCC()
{
	OCCTK::SimpleClamp::BasePlate occBasePlate{
		*(this->shape)->GetOCC(),
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
// 委托结构和cpp结构的转换
OCCTK::SimpleClamp::VerticalPlate VerticalPlate::GetOCC()
{
	//TopoDS_Shape shape;
	TopoDS_Shape VerticalPlateShape = TopoDS_Shape();
	if (this->shape != nullptr)
	{
		VerticalPlateShape = *(this->shape)->GetOCC();
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
		this->XY,
		this->Z,
		this->clearances,
		this->minSupportLen,
		this->cuttingDistance,
		this->connectionHeight,
	};
	return occVerticalPlate;
}
// 委托结构和cpp结构的转换
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
	TopoDS_Shape theshape = *(shape->GetOCC());
	//TopoDS_Edge Edge;
	TopoDS_Edge theedge = TopoDS::Edge(*(edge->GetOCC()));
	gp_Pnt thestart = *start->GetOCC();
	gp_Pnt theend = *end->GetOCC();
	OCCTK::SimpleClamp::VerticalPiece occVerticalPiece{
		thedir,
		theshape,
		theedge,
		thestart,
		theend,
	};
	return occVerticalPiece;
}

// 把CPP结构体封装为委托结构体
static auto getDelegate(OCCTK::SimpleClamp::BasePlate cppBase) {
	BasePlate^ theBase = gcnew BasePlate();
	theBase->shape = gcnew WTopoDS_Shape(cppBase.shape);
	theBase->X = cppBase.X;
	theBase->Y = cppBase.Y;
	theBase->Z = cppBase.Z;
	theBase->dX = cppBase.dX;
	theBase->dY = cppBase.dY;
	theBase->OffsetX = cppBase.OffsetX;
	theBase->OffsetY = cppBase.OffsetY;

	return theBase;
}
// 把CPP结构体封装为委托结构体
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
	thePiece->shape = gcnew WTopoDS_Shape(cppPiece.Shape);
	thePiece->aisShape = gcnew WAIS_Shape(cppPiece.Shape);
	thePiece->edge = gcnew WTopoDS_Shape(cppPiece.edge);
	thePiece->start = gcnew Wgp_Pnt(cppPiece.startPoint);
	thePiece->end = gcnew Wgp_Pnt(cppPiece.endPoint);
	return thePiece;
}
// 把CPP结构体封装为委托结构体
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
	thePlate->XY = cppPlate.XY;
	thePlate->Z = cppPlate.Z;
	thePlate->pieces = gcnew List<VerticalPiece^>();
	for (auto oneCPPPiece : cppPlate.pieces)
	{
		thePlate->pieces->Add(getDelegate(oneCPPPiece));
	}
	thePlate->clearances = cppPlate.clearances;
	thePlate->minSupportLen = cppPlate.minSupportLen;
	thePlate->cuttingDistance = cppPlate.cuttiDistance;
	thePlate->connectionHeight = cppPlate.connectionHeight;
	thePlate->shape = gcnew WTopoDS_Shape(cppPlate.shape);
	thePlate->aisShape = gcnew WAIS_Shape(cppPlate.shape);

	return thePlate;
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
BasePlate^ SimpleClampMaker::MakeBasePlate_NoInteract(WTopoDS_Shape^ InputWorkpiece, double OffsetZ, double BasePlateOffsetX, double BasePlateOffsetY) {
	TopoDS_Shape occInputWorkpiece = *InputWorkpiece->GetOCC();

	OCCTK::SimpleClamp::BasePlate theCPPBasePlate = OCCTK::SimpleClamp::MakeBasePlate(occInputWorkpiece, OffsetZ, BasePlateOffsetX, BasePlateOffsetY);

	BasePlate^ theBasePlate = gcnew BasePlate();
	theBasePlate->shape = gcnew WTopoDS_Shape(theCPPBasePlate.shape);
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
VerticalPlate^ SimpleClampMaker::MakeVerticalPlate(WTopoDS_Shape^ InputWorkpiece, BasePlate^ BasePlate, Direction theDirection, double theLocation, double theClearances, double theMinSupportLen, double theCuttingDistance)
{
	VerticalPlate^ onePlate = gcnew VerticalPlate();
	OCCTK::SimpleClamp::VerticalPlate cppPlate;

	TopoDS_Shape occInputWorkpiece = *InputWorkpiece->GetOCC();
	switch (theDirection)
	{
	case OCCTK::Laser::Direction::X:
		cppPlate = OCCTK::SimpleClamp::MakeVerticalPlate(occInputWorkpiece, BasePlate->GetOCC(), OCCTK::SimpleClamp::Direction::X, theLocation, theClearances, theMinSupportLen, theCuttingDistance);
		break;
	case OCCTK::Laser::Direction::Y:
		cppPlate = OCCTK::SimpleClamp::MakeVerticalPlate(occInputWorkpiece, BasePlate->GetOCC(), OCCTK::SimpleClamp::Direction::Y, theLocation, theClearances, theMinSupportLen, theCuttingDistance);
		break;
	default:
		break;
	}
	//把CPP结构体封装为委托结构体
	onePlate->direction = theDirection;
	onePlate->XY = cppPlate.XY;
	onePlate->Z = cppPlate.Z;
	onePlate->location = cppPlate.location;
	onePlate->pieces = gcnew List<VerticalPiece^>();
	for (auto oneCPPPiece : cppPlate.pieces)
	{
		VerticalPiece^ onePiece = gcnew VerticalPiece();

		onePiece->dir = theDirection; // 没必要每一个都取，同一块板内的piece方向是统一的
		onePiece->shape = gcnew WTopoDS_Shape(oneCPPPiece.Shape);
		onePiece->aisShape = gcnew WAIS_Shape(oneCPPPiece.Shape);
		onePiece->edge = gcnew WTopoDS_Shape(oneCPPPiece.edge);
		//这里应该区分是X还是Y，换成Pnt就不需要了
		onePiece->start = gcnew Wgp_Pnt(oneCPPPiece.startPoint);
		onePiece->end = gcnew Wgp_Pnt(oneCPPPiece.endPoint);

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
List<VerticalPlate^>^ SimpleClampMaker::MakeVerticalPlates(WTopoDS_Shape^ InputWorkpiece, BasePlate^ BasePlate, List<double>^ XLocation, List<double>^ YLocation, double theClearances, double theMinSupportLen, double theCuttingDistance)
{
	List<VerticalPlate^>^ Plates = gcnew List<VerticalPlate^>(); // result
	TopoDS_Shape occInputWorkpiece = *InputWorkpiece->GetOCC();

	for each (double aXLoc in XLocation)
	{
		VerticalPlate^ aXPlate = MakeVerticalPlate(InputWorkpiece, BasePlate, Direction::X, aXLoc, theClearances, theMinSupportLen, theCuttingDistance);
		Plates->Add(aXPlate);
	}
	for each (double aYLoc in YLocation)
	{
		VerticalPlate^ aYPlate = MakeVerticalPlate(InputWorkpiece, BasePlate, Direction::Y, aYLoc, theClearances, theMinSupportLen, theCuttingDistance);
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
List<VerticalPlate^>^ SimpleClampMaker::MakeVerticalPlates(WTopoDS_Shape^ InputWorkpiece, BasePlate^ BasePlate, int XNum, int YNum, double initialOffsetX, double initialOffsetY, double theClearances, double theMinSupportLen, double theCuttingDistance)
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

	return MakeVerticalPlates(InputWorkpiece, BasePlate, XLocation, YLocation, theClearances, theMinSupportLen, theCuttingDistance);
}

/// <summary>
/// 缝合Piece为整块板
/// </summary>
/// <param name="theVerticalPlate"></param>
/// <param name="theConnectHight"></param>
void SimpleClampMaker::SuturePLate(VerticalPlate^% theVerticalPlate, BasePlate^ BasePlate, double theConnectHight)
{
	//theVerticalPlate->connectionHeight = theConnectHight;
	//OCCTK::SimpleClamp::VerticalPlate theoccVP = theVerticalPlate->GetOCC();
	//OCCTK::SimpleClamp::BasePlate theoccBP = BasePlate->GetOCC();
	//OCCTK::SimpleClamp::SuturePiece(theoccVP, theoccBP, theConnectHight);
	////shape写回对象中
	//theVerticalPlate->shape = gcnew WTopoDS_Shape(theoccVP.shape);
	//theVerticalPlate->aisShape = gcnew WAIS_Shape(theoccVP.shape);
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
		oneVP->connectionHeight = theConnectHight;
		// 板的连接
		OCCTK::SimpleClamp::VerticalPlate theoccVP = oneVP->GetOCC();
		OCCTK::SimpleClamp::BasePlate theoccBP = BasePlate->GetOCC();
		OCCTK::SimpleClamp::SuturePiece(theoccVP, theoccBP, theConnectHight);
		oneVP->shape = gcnew WTopoDS_Shape(theoccVP.shape);
		oneVP->aisShape = gcnew WAIS_Shape(theoccVP.shape);
		// 切连接槽
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
		VerticalPlate^ WPlate = getDelegate(theoccVP);
		WPlate->connectionHeight = theConnectHight;
		WPlate->connectionThickness = theConnectThickness;
		WPlate->filletRadius = theFilletRadius;
		plates->Add(WPlate);
	}
#pragma endregion

	return plates;
}

WTopoDS_Shape^ SimpleClampMaker::DeployPlates(List<VerticalPlate^>^ verticalPlates, BasePlate^ BasePlate) {
	OCCTK::SimpleClamp::BasePlate theoccBP = BasePlate->GetOCC();
	std::vector<OCCTK::SimpleClamp::VerticalPlate> cppPlates;
	for each (auto onePlate in verticalPlates)
	{
		cppPlates.push_back(onePlate->GetOCC());
	}
	TopoDS_Shape result = OCCTK::SimpleClamp::DeployPlates(theoccBP, cppPlates);
	return gcnew WTopoDS_Shape(result);
}

void SimpleClampMaker::BrandNumber(VerticalPlate^% theVerticalPlate, double hight, int number)
{
	OCCTK::SimpleClamp::VerticalPlate theoccPlate = theVerticalPlate->GetOCC();
	OCCTK::SimpleClamp::VerticalPlate newplate = OCCTK::SimpleClamp::BrandNumber(theoccPlate, hight, number);
	VerticalPlate^ WPlate = getDelegate(newplate);
	WPlate->tagValue = number;
	theVerticalPlate = WPlate;
}
void SimpleClampMaker::BrandNumber(VerticalPlate^% theVerticalPlate, double hight, int number, Wgp_Pnt^ thePoint)
{
	OCCTK::SimpleClamp::VerticalPlate theoccPlate = theVerticalPlate->GetOCC();
	OCCTK::SimpleClamp::VerticalPlate newplate = OCCTK::SimpleClamp::BrandNumber(theoccPlate, hight, number, *thePoint->GetOCC());
	VerticalPlate^ WPlate = getDelegate(newplate);
	WPlate->tagValue = number;
	theVerticalPlate = WPlate;
}


}
