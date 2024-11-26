#pragma once
#include "WMakeSimpleClamp.h"
#include "MakeSimpleClamp.h"
#include "DataExchange\ICO_StringExchange.h"
#include "OCCProxy\ICO_Trsf.h"
#include <TopoDS_FrozenShape.hxx>
using namespace System::Collections::Generic;
using namespace OCCTK::OCC::gp;
using namespace OCCTK::OCC::Topo;
using namespace OCCTK::OCC::AIS;

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
	// 设置移动
	Trsf^ T = gcnew Trsf(Pnt(theCPPBasePlate.X, theCPPBasePlate.Y, theCPPBasePlate.Z), Pnt());
	theCPPBasePlate.X = 0.0;
	theCPPBasePlate.Y = 0.0;
	theCPPBasePlate.Z = 0.0;
	BasePlate^ base = gcnew BasePlate(theCPPBasePlate);
	base->Translation = T;
	return base;
}

/// <summary>
/// 在指定位置创建一块竖板
/// </summary>
/// <param name="InputWorkpiece"></param>
/// <param name="BasePlate"></param>
/// <param name="thePose"></param>
/// <param name="theClearances"></param>
/// <param name="theMinSupportLen"></param>
/// <param name="theCuttingDistance"></param>
/// <returns></returns>
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
		if (theoccVP.auxiliary) {
			continue;
		}
		if (theoccVP.pieces.empty()) { continue; }
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
		if (theoccVP.pieces.empty()) { continue; }
		if (theoccVP.auxiliary) {
			continue;
		}
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
		//如果没有相交竖板，添加一块辅助板
		if (oneVP.singlePlate == true) {
			oneVP.singlePlate = false;
			auto auxPlate = SimpleClamp::AddSupportPlate(oneVP, true);
			auto newVP = gcnew VerticalPlate(auxPlate);
			newVP->sutured = true;
			newVP->myShape = gcnew TShape(auxPlate.shape);
			newVP->myAIS = gcnew AShape(auxPlate.shape);
			tempToUpPlates->Add(newVP);
		}
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
		//如果没有相交竖板，添加一块辅助板
		if (oneVP.singlePlate == true) {
			oneVP.singlePlate = false;
			auto auxPlate = SimpleClamp::AddSupportPlate(oneVP, false);
			auto newVP = gcnew VerticalPlate(auxPlate);
			newVP->sutured = true;
			newVP->myShape = gcnew TShape(auxPlate.shape);
			newVP->myAIS = gcnew AShape(auxPlate.shape);
			tempToDownPlates->Add(newVP);
		}
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
	// 无法判断mySlotShape是否已被冻结，直接清空在函数中重新构建
	theoccBP.mySlotShape.Nullify();
	SimpleClamp::BasePlate newOccBP;
	try {
		newOccBP = SimpleClamp::SlotBasePlate(theoccBP, tempOccDownVP, tempOccUpVP);
	}
	catch (const TopoDS_FrozenShape& ex) {
		// 将特定的 C++ 异常转换为托管异常
		throw gcnew System::InvalidOperationException("TopoDS_FrozenShape: Attempted to modify a frozen shape.");
	}
	catch (const std::exception& e) {
		System::String^ errorMsg = gcnew System::String(e.what());
		throw gcnew System::Exception(errorMsg);
	}
	return gcnew BasePlate(newOccBP);
}

VerticalPlate^ SimpleClampMaker::BrandNumberVerticalPlate(VerticalPlate^ theVerticalPlate, double hight) {
	SimpleClamp::VerticalPlate theoccPlate = theVerticalPlate->GetOCC();
	SimpleClamp::VerticalPlate newoccplate = SimpleClamp::BrandNumberVerticalPlate(theoccPlate, hight);
	VerticalPlate^ newPlate = gcnew VerticalPlate(newoccplate);
	// 更新NumberedShape和AIS
	newPlate->sutured = true;
	newPlate->numbered = true;
	newPlate->myNumberedShape = gcnew TShape(newoccplate.numberedShape);
	newPlate->myAIS = gcnew AShape(newoccplate.numberedShape);
	return newPlate;
}
BasePlate^ SimpleClampMaker::BrandNumberBasePlate(BasePlate^ theBasePlate, double hight) {
	SimpleClamp::BasePlate theoccBasePlate = theBasePlate->GetOCC();
	//可能需要判断 mySlotShape 是否已冻结
	//theoccBasePlate.mySlotShape;
	SimpleClamp::BasePlate newoccBasePlate;
	try {
		newoccBasePlate = SimpleClamp::BrandNumberBasePlate(theoccBasePlate, hight);
	}
	catch (const std::exception& e) {
		// 将 std::exception 的消息转换为 System::String^
		System::String^ errorMsg = gcnew System::String(e.what());
		// 使用转换后的消息构造新的 System::Exception
		throw gcnew System::Exception(errorMsg);
	}
	BasePlate^ newbasePlate = gcnew BasePlate(newoccBasePlate);
	return newbasePlate;
}
//void SimpleClampMaker::BrandNumber(VerticalPlate^% theVerticalPlate, double hight, int number, Wgp_Pnt thePoint)
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
		if (!onePlate->Numbered) { continue; }
		cppDownPlates.push_back(onePlate->GetOCC());
	}

	std::vector<SimpleClamp::VerticalPlate> cppUpPlates;
	for each (auto onePlate in MiddleToUpPlatesates) {
		if (!onePlate->Sutured) { continue; }
		if (!onePlate->Numbered) { continue; }
		cppUpPlates.push_back(onePlate->GetOCC());
	}
	TopoDS_Shape result = SimpleClamp::DeployPlates(BasePlate->GetOCC(), cppDownPlates, cppUpPlates);
	return gcnew TShape(result);
}

auto SimpleClampMaker::TestError(int value) {
	switch (value) {
	case 0:
		throw gcnew System::Exception("Test Error 0");
	case 1:
		throw gcnew System::Exception("Test Error 1");
	case 2:
		throw gcnew System::Exception("Test Error 2");
	case 4:
		throw gcnew System::Exception("Test Error 4");
	default:
		throw gcnew System::Exception(System::String::Format("Test Unknown Error {0}", value));
	}
	try {
		throw gcnew System::Exception(System::String::Format("Test Unknown Error {0}", value));
	}
	catch (const std::exception& e) {
		// 将 std::exception 的消息转换为 System::String^
		System::String^ errorMsg = gcnew System::String(e.what());
		// 使用转换后的消息构造新的 System::Exception
		throw gcnew System::Exception(errorMsg);
	}
	catch (const TopoDS_FrozenShape& ex) {
		// 将特定的 C++ 异常转换为托管异常
		throw gcnew System::InvalidOperationException("TopoDS_FrozenShape: Attempted to modify a frozen shape.");
	}
	catch (const Standard_Failure& e) {
		throw gcnew System::Exception(gcnew System::String(e.GetMessageString()));
	}
}


}
}
