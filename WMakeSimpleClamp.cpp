#pragma once
#include "WMakeSimpleClamp.h"
#include "MakeSimpleClamp.h"
#include "ICO_StringExchange.h"
using namespace System::Collections::Generic;

namespace OCCTK {
namespace Laser {

#pragma region 封装CPP

static auto getDelegate(OCCTK::SimpleClamp::BasePlate cppBase) { return gcnew BasePlate(cppBase); }

static auto getDelegate(OCCTK::SimpleClamp::PlatePose cppDir) { return gcnew PlatePose(cppDir); }

static auto getDelegate(OCCTK::SimpleClamp::VerticalPiece cppPiece) { return gcnew VerticalPiece(cppPiece); }

static auto getDelegate(OCCTK::SimpleClamp::VerticalPlate cppPlate) { return gcnew VerticalPlate(cppPlate); }

#pragma endregion

// 根据包围盒创建底板
BasePlate^ SimpleClampMaker::MakeBasePlate_NoInteract(TShape^ InputWorkpiece, double OffsetZ, double BasePlateOffsetX, double BasePlateOffsetY) {
	SimpleClamp::BasePlate theCPPBasePlate = SimpleClamp::MakeBasePlate(InputWorkpiece->GetOCC(), OffsetZ, BasePlateOffsetX, BasePlateOffsetY);
	return gcnew BasePlate(theCPPBasePlate);
}

// 在指定位置创建一块竖板
VerticalPlate^ SimpleClampMaker::MakeVerticalPlate(TShape^ InputWorkpiece, BasePlate^ BasePlate, PlatePose^ thePose, double theClearances, double theMinSupportLen, double theCuttingDistance) {
	OCCTK::SimpleClamp::VerticalPlate cppPlate = OCCTK::SimpleClamp::MakeVerticalPlate(InputWorkpiece->GetOCC(), BasePlate->GetOCC(), thePose->GetOCC(), theClearances, theMinSupportLen, theCuttingDistance);
	return gcnew VerticalPlate(cppPlate);
}

// 缝合Piece为整块板
VerticalPlate^ SimpleClampMaker::SuturePLate(VerticalPlate^ theVerticalPlate, BasePlate^ BasePlate, double theConnectHight, double theConnectThickness) {
	SimpleClamp::VerticalPlate theoccVP = theVerticalPlate->GetOCC();
	SimpleClamp::BasePlate theoccBP = BasePlate->GetOCC();
	SimpleClamp::SuturePiece(theoccVP, theoccBP, theConnectHight, theConnectThickness);
	auto newVP = gcnew VerticalPlate(theoccVP);
	// 将Shape和AIS保存在对象中
	newVP->sutured = true;
	newVP->myShape = gcnew TShape(theoccVP.shape);
	newVP->myAIS = gcnew AShape(theoccVP.shape);
	return newVP;
}

//给每块板开连接槽
cli::array<List<VerticalPlate^>^>^ SimpleClampMaker::ConnectVerticalPLates(List<VerticalPlate^>^ toDownPlates, List<VerticalPlate^>^ toUpPlates, BasePlate^ BasePlate, double theAvoidanceHeight, double theConnectThickness, double theFilletRadius) {
#pragma region 先把每块板连接起来，得到 start 和 end ，作为开槽的依据

	SimpleClamp::BasePlate theoccBP = BasePlate->GetOCC();
	std::vector<SimpleClamp::VerticalPlate> tempOccDownVP;
	std::vector<SimpleClamp::VerticalPlate> tempOccUpVP;
	List<VerticalPlate^>^ debugDown = gcnew List<VerticalPlate^>();
	List<VerticalPlate^>^ debugUp = gcnew List<VerticalPlate^>();

	for each (auto oneVP in toDownPlates) {
		SimpleClamp::VerticalPlate theoccVP = oneVP->GetOCC();
		SimpleClamp::SuturePiece(theoccVP, theoccBP, theAvoidanceHeight, theConnectThickness);
		tempOccDownVP.push_back(theoccVP);
		////! debug
		//auto newVP = gcnew VerticalPlate(theoccVP);
		//newVP->sutured = true;
		//newVP->myShape = gcnew TShape(theoccVP.shape);
		//newVP->myAIS = gcnew AShape(theoccVP.shape);
		//debugDown->Add(newVP);
	}
	for each (auto oneVP in toUpPlates) {
		SimpleClamp::VerticalPlate theoccVP = oneVP->GetOCC();
		SimpleClamp::SuturePiece(theoccVP, theoccBP, theAvoidanceHeight, theConnectThickness);
		tempOccUpVP.push_back(theoccVP);
		////! debug
		//auto newVP = gcnew VerticalPlate(theoccVP);
		//newVP->sutured = true;
		//newVP->myShape = gcnew TShape(theoccVP.shape);
		//newVP->myAIS = gcnew AShape(theoccVP.shape);
		//debugUp->Add(newVP);
	}
	////! debug
	//cli::array<List<VerticalPlate^>^>^ result = gcnew cli::array<List<VerticalPlate^>^>(2);
	//result[0] = debugDown;
	//result[1] = debugUp;
	//return result;

#pragma endregion

#pragma region 连接每块板，并在交错的位置开槽

	List<VerticalPlate^>^ tempToDownPlates = gcnew List<VerticalPlate^>();
	List<VerticalPlate^>^ tempToUpPlates = gcnew List<VerticalPlate^>();
	// 从下到上开槽
	for (size_t i = 0; i < tempOccDownVP.size(); ++i) {
		auto oneVP = tempOccDownVP[i];
		std::vector<SimpleClamp::VerticalPlate> otherVPs;
		for (size_t j = 0; j < tempOccDownVP.size(); ++j) {
			if (i == j) { continue; }
			auto otherVP = tempOccDownVP[j];
			otherVPs.push_back(otherVP);
		}
		for (size_t i = 0; i < tempOccUpVP.size(); i++) { otherVPs.push_back(tempOccUpVP[i]); }

		// 开槽
		SimpleClamp::SlotVerticalPlate(oneVP, otherVPs, theFilletRadius, true);
		// 保存结果，将Shape和AIS保存在对象中
		auto newVP = gcnew VerticalPlate(oneVP);
		newVP->sutured = true;
		newVP->myShape = gcnew TShape(oneVP.shape);
		newVP->myAIS = gcnew AShape(oneVP.shape);
		tempToDownPlates->Add(newVP);
	}

	// 从上到下开槽
	for (size_t i = 0; i < tempOccUpVP.size(); ++i) {
		auto oneVP = tempOccUpVP[i];
		std::vector<SimpleClamp::VerticalPlate> otherVPs;
		for (size_t j = 0; j < tempOccUpVP.size(); ++j) {
			if (i == j) { continue; }
			otherVPs.push_back(tempOccUpVP[j]);
		}
		for (size_t i = 0; i < tempOccDownVP.size(); i++) {
			otherVPs.push_back(tempOccDownVP[i]);
		}

		// 开槽
		SimpleClamp::SlotVerticalPlate(oneVP, otherVPs, theFilletRadius, false);
		// 保存结果，将Shape和AIS保存在对象中
		auto newVP = gcnew VerticalPlate(oneVP);
		newVP->sutured = true;
		newVP->myShape = gcnew TShape(oneVP.shape);
		newVP->myAIS = gcnew AShape(oneVP.shape);
		tempToUpPlates->Add(newVP);
	}

#pragma endregion

	cli::array<List<VerticalPlate^>^>^ result = gcnew cli::array<List<VerticalPlate^>^>(2);
	result[0] = tempToDownPlates;
	result[1] = tempToUpPlates;
	return result;
}

BasePlate^ SimpleClampMaker::SlotBasePLates(BasePlate^ theBasePlate, List<VerticalPlate^>^ toDownPlates, List<VerticalPlate^>^ toUpPlates) {
	SimpleClamp::BasePlate theoccBP = theBasePlate->GetOCC();
	std::vector<SimpleClamp::VerticalPlate> tempOccDownVP;
	std::vector<SimpleClamp::VerticalPlate> tempOccUpVP;
	for each (auto oneVP in toDownPlates) { tempOccDownVP.push_back(oneVP->GetOCC()); }
	for each (auto oneVP in toUpPlates) { tempOccUpVP.push_back(oneVP->GetOCC()); }

	SimpleClamp::BasePlate newOccBP = SimpleClamp::SlotBasePlate(theoccBP, tempOccDownVP, tempOccUpVP);
	return gcnew BasePlate(newOccBP);
}

VerticalPlate^ SimpleClampMaker::BrandNumberVerticalPlate(VerticalPlate^ theVerticalPlate, double hight) {
	SimpleClamp::VerticalPlate theoccPlate = theVerticalPlate->GetOCC();
	SimpleClamp::VerticalPlate newoccplate = SimpleClamp::BrandNumberVerticalPlate(theoccPlate, hight);
	VerticalPlate^ newPlate = gcnew VerticalPlate(newoccplate);
	// 更新NumberedShape和AIS
	newPlate->sutured = true;
	newPlate->myNumberedShape = gcnew TShape(newoccplate.numberedShape);
	newPlate->myAIS = gcnew AShape(newoccplate.numberedShape);
	return newPlate;
}
BasePlate^ SimpleClampMaker::BrandNumberBasePlate(BasePlate^ theBasePlate, double hight) {
	SimpleClamp::BasePlate theoccBasePlate = theBasePlate->GetOCC();
	SimpleClamp::BasePlate newoccBasePlate = SimpleClamp::BrandNumberBasePlate(theoccBasePlate, hight);
	BasePlate^ newbasePlate = gcnew BasePlate(newoccBasePlate);
	return newbasePlate;
}
//void SimpleClampMaker::BrandNumber(VerticalPlate^% theVerticalPlate, double hight, int number, Wgp_Pnt^ thePoint)
//{
//	OCCTK::SimpleClamp::VerticalPlate theoccPlate = theVerticalPlate->GetOCC();
//	OCCTK::SimpleClamp::VerticalPlate newplate = OCCTK::SimpleClamp::BrandNumber(theoccPlate, hight, number, *thePoint->GetOCC());
//	VerticalPlate^ WPlate = getDelegate(newplate);
//	WPlate->tagValue = number;
//	theVerticalPlate = WPlate;
//}

// 得到展平形状
TShape^ SimpleClampMaker::DeployPlates(BasePlate^ BasePlate, List<VerticalPlate^>^ MiddleToDownPlates, List<VerticalPlate^>^ MiddleToUpPlatesates) {
	std::vector<SimpleClamp::VerticalPlate> cppDownPlates;
	for each (auto onePlate in MiddleToDownPlates) {
		if (!onePlate->Sutured) { continue; }
		cppDownPlates.push_back(onePlate->GetOCC());
	}

	std::vector<SimpleClamp::VerticalPlate> cppUpPlates;
	for each (auto onePlate in MiddleToUpPlatesates) {
		if (!onePlate->Sutured) { continue; }
		cppUpPlates.push_back(onePlate->GetOCC());
	}
	TopoDS_Shape result = SimpleClamp::DeployPlates(BasePlate->GetOCC(), cppDownPlates, cppUpPlates);
	return gcnew TShape(result);
}


}
}
