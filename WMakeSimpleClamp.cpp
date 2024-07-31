#include "WMakeSimpleClamp.h"
#include "MakeSimpleClamp.h"
#include <TopoDS_Shape.hxx>
#include <TopoDS.hxx>
using namespace System::Collections::Generic;

namespace OCCTK {
namespace Laser {

#pragma region 封装CPP

static auto getDelegate(OCCTK::SimpleClamp::BasePlate cppBase) { return gcnew BasePlate(cppBase); }

static auto getDelegate(OCCTK::SimpleClamp::PlatePose cppDir) { return gcnew PlatePose(cppDir); }

static auto getDelegate(OCCTK::SimpleClamp::VerticalPiece cppPiece) { return gcnew VerticalPiece(cppPiece); }

static auto getDelegate(OCCTK::SimpleClamp::VerticalPlate cppPlate) { return gcnew VerticalPlate(cppPlate); }

#pragma endregion

// 缝合板为一块
void VerticalPlate::SuturePLate() {
	throw gcnew System::NotImplementedException();
}

// 根据包围盒创建底板
BasePlate^ SimpleClampMaker::MakeBasePlate_NoInteract(WTopoDS_Shape^ InputWorkpiece, double OffsetZ, double BasePlateOffsetX, double BasePlateOffsetY) {
	OCCTK::SimpleClamp::BasePlate theCPPBasePlate = OCCTK::SimpleClamp::MakeBasePlate(*InputWorkpiece->GetOCC(), OffsetZ, BasePlateOffsetX, BasePlateOffsetY);
	return gcnew BasePlate(theCPPBasePlate);
}

// 在指定位置创建一块竖板
VerticalPlate^ SimpleClampMaker::MakeVerticalPlate(WTopoDS_Shape^ InputWorkpiece, BasePlate^ BasePlate, PlatePose^ theDirection, double theClearances, double theMinSupportLen, double theCuttingDistance) {
	OCCTK::SimpleClamp::VerticalPlate cppPlate = OCCTK::SimpleClamp::MakeVerticalPlate(*InputWorkpiece->GetOCC(), BasePlate->GetOCC(), theDirection->GetOCC(), theClearances, theMinSupportLen, theCuttingDistance);
	return gcnew VerticalPlate(cppPlate);
}

///// <summary>
///// 创建所有竖板
///// </summary>
///// <param name="InputAISWorkpiece"></param>
///// <param name="BasePlate"></param>
///// <param name="XLocation"></param>
///// <param name="YLocation"></param>
///// <param name="theClearances"></param>
///// <param name="theMinSupportLen"></param>
///// <param name="theCuttingDistance"></param>
///// <returns></returns>
//List<VerticalPlate^>^ SimpleClampMaker::MakeVerticalPlates(WTopoDS_Shape^ InputWorkpiece, BasePlate^ BasePlate, List<double>^ XLocation, List<double>^ YLocation, double theClearances, double theMinSupportLen, double theCuttingDistance)
//{
//	List<VerticalPlate^>^ Plates = gcnew List<VerticalPlate^>(); // result
//	TopoDS_Shape occInputWorkpiece = *InputWorkpiece->GetOCC();
//
//	for each (double aXLoc in XLocation)
//	{
//		VerticalPlate^ aXPlate = MakeVerticalPlate(InputWorkpiece, BasePlate, Direction::X, aXLoc, theClearances, theMinSupportLen, theCuttingDistance);
//		Plates->Add(aXPlate);
//	}
//	for each (double aYLoc in YLocation)
//	{
//		VerticalPlate^ aYPlate = MakeVerticalPlate(InputWorkpiece, BasePlate, Direction::Y, aYLoc, theClearances, theMinSupportLen, theCuttingDistance);
//		Plates->Add(aYPlate);
//	}
//	return Plates;
//}
//
///// <summary>
///// 根据X、Y数目创建所有竖板
///// </summary>
///// <param name="InputAISWorkpiece"></param>
///// <param name="BasePlate"></param>
///// <param name="XNum"></param>
///// <param name="YNum"></param>
///// <param name="initialOffsetX"></param>
///// <param name="initialOffsetY"></param>
///// <param name="theClearances"></param>
///// <param name="theMinSupportLen"></param>
///// <param name="theCuttingDistance"></param>
///// <returns></returns>
//List<VerticalPlate^>^ SimpleClampMaker::MakeVerticalPlates(WTopoDS_Shape^ InputWorkpiece, BasePlate^ BasePlate, int XNum, int YNum, double initialOffsetX, double initialOffsetY, double theClearances, double theMinSupportLen, double theCuttingDistance)
//{
//	// 计算 XLocation
//	double XLoc = BasePlate->X + initialOffsetX;
//	List<double>^ XLocation = gcnew List<double>();
//	XLocation->Add(XLoc);
//	//两端始终要有两块，由initialOffset控制，中间会有Num-2块，位置均分
//	for (int i = 0; i < XNum - 2; ++i) {
//		XLoc += (BasePlate->dX - initialOffsetX * 2) / (XNum - 2 + 1);
//		XLocation->Add(XLoc);
//	}
//	XLocation->Add(BasePlate->X + BasePlate->dX - initialOffsetX);
//
//	//计算 YLocation
//	double YLoc = BasePlate->Y + initialOffsetY;
//	List<double>^ YLocation = gcnew List<double>();
//	YLocation->Add(YLoc);
//	//两端始终要有两块，由initialOffset控制，中间会有Num-2块，位置均分
//	for (int i = 0; i < YNum - 2; ++i) {
//		YLoc += (BasePlate->dY - initialOffsetY * 2) / (YNum - 2 + 1);
//		YLocation->Add(YLoc);
//	}
//	YLocation->Add(BasePlate->Y + BasePlate->dY - initialOffsetY);
//
//	return MakeVerticalPlates(InputWorkpiece, BasePlate, XLocation, YLocation, theClearances, theMinSupportLen, theCuttingDistance);
//}

// 缝合Piece为整块板
//void SimpleClampMaker::SuturePLate(VerticalPlate^% theVerticalPlate, BasePlate^ BasePlate, double theConnectHight)
//{
//	//theVerticalPlate->connectionHeight = theConnectHight;
//	//OCCTK::SimpleClamp::VerticalPlate theoccVP = theVerticalPlate->GetOCC();
//	//OCCTK::SimpleClamp::BasePlate theoccBP = BasePlate->GetOCC();
//	//OCCTK::SimpleClamp::SuturePiece(theoccVP, theoccBP, theConnectHight);
//	////shape写回对象中
//	//theVerticalPlate->shape = gcnew WTopoDS_Shape(theoccVP.shape);
//	//theVerticalPlate->aisShape = gcnew WAIS_Shape(theoccVP.shape);
//}

//List<VerticalPlate^>^ SimpleClampMaker::SuturePLates(List<VerticalPlate^>^ verticalPlates, BasePlate^ BasePlate, double theConnectHight, double theConnectThickness, double theFilletRadius)
//{
//	List<VerticalPlate^>^ plates = gcnew List<VerticalPlate^>();
//	//把X和Y方向的竖板分为两组，得到其位置
//	std::vector<double> XLocs, YLocs;
//	for each (auto oneVP in verticalPlates)
//	{
//		if (oneVP->direction == Direction::X)
//		{
//			double l = oneVP->location;
//			XLocs.push_back(l);
//		}
//		if (oneVP->direction == Direction::Y)
//		{
//			double l = oneVP->location;
//			YLocs.push_back(l);
//		}
//	}
//#pragma region 在每块板的每个对应位置开槽
//	int testNum = 0;
//	for each (auto theP in verticalPlates) {
//		auto oneVP = theP;
//		oneVP->connectionHeight = theConnectHight;
//		// 板的连接
//		OCCTK::SimpleClamp::VerticalPlate theoccVP = oneVP->GetOCC();
//		OCCTK::SimpleClamp::BasePlate theoccBP = BasePlate->GetOCC();
//		OCCTK::SimpleClamp::SuturePiece(theoccVP, theoccBP, theConnectHight);
//		oneVP->shape = gcnew WTopoDS_Shape(theoccVP.shape);
//		oneVP->aisShape = gcnew WAIS_Shape(theoccVP.shape);
//		// 切连接槽
//		switch (oneVP->direction)
//		{
//		case Direction::X:
//			OCCTK::SimpleClamp::Slotting(theoccVP, theoccBP, YLocs, theConnectHight, theConnectThickness, theFilletRadius);
//			break;
//		case Direction::Y:
//			OCCTK::SimpleClamp::Slotting(theoccVP, theoccBP, XLocs, theConnectHight, theConnectThickness, theFilletRadius);
//			break;
//		default:
//			break;
//		}
//		VerticalPlate^ WPlate = getDelegate(theoccVP);
//		WPlate->connectionHeight = theConnectHight;
//		WPlate->connectionThickness = theConnectThickness;
//		WPlate->filletRadius = theFilletRadius;
//		plates->Add(WPlate);
//	}
//#pragma endregion
//
//	return plates;
//}

//WTopoDS_Shape^ SimpleClampMaker::DeployPlates(List<VerticalPlate^>^ verticalPlates, BasePlate^ BasePlate) {
//	OCCTK::SimpleClamp::BasePlate theoccBP = BasePlate->GetOCC();
//	std::vector<OCCTK::SimpleClamp::VerticalPlate> cppPlates;
//	for each (auto onePlate in verticalPlates)
//	{
//		cppPlates.push_back(onePlate->GetOCC());
//	}
//	TopoDS_Shape result = OCCTK::SimpleClamp::DeployPlates(theoccBP, cppPlates);
//	return gcnew WTopoDS_Shape(result);
//}

//void SimpleClampMaker::BrandNumber(VerticalPlate^% theVerticalPlate, double hight, int number)
//{
//	OCCTK::SimpleClamp::VerticalPlate theoccPlate = theVerticalPlate->GetOCC();
//	OCCTK::SimpleClamp::VerticalPlate newplate = OCCTK::SimpleClamp::BrandNumber(theoccPlate, hight, number);
//	VerticalPlate^ WPlate = getDelegate(newplate);
//	WPlate->tagValue = number;
//	theVerticalPlate = WPlate;
//}

//void SimpleClampMaker::BrandNumber(VerticalPlate^% theVerticalPlate, double hight, int number, Wgp_Pnt^ thePoint)
//{
//	OCCTK::SimpleClamp::VerticalPlate theoccPlate = theVerticalPlate->GetOCC();
//	OCCTK::SimpleClamp::VerticalPlate newplate = OCCTK::SimpleClamp::BrandNumber(theoccPlate, hight, number, *thePoint->GetOCC());
//	VerticalPlate^ WPlate = getDelegate(newplate);
//	WPlate->tagValue = number;
//	theVerticalPlate = WPlate;
//}


}
}
