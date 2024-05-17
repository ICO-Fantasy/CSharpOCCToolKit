#pragma once
#include "MakeSimpleClamp.h"
#include "WAIS_Shape.h"
#include <vcclr.h>

using namespace  OCCTK::OCC::AIS;
using namespace System::Collections::Generic;
using namespace System;

namespace OCCTK::Laser
{
public enum class Direction {
	X,
	Y
};
public ref struct BasePlate {
	WAIS_Shape^ shape;//todo 修改成TopoDS_Shape
	double X;
	double Y;
	double Z;
	double dX;
	double dY;
	double OffsetX;
	double OffsetY;
public:
	OCCTK::SimpleClamp::BasePlate GetOCC();
};
public ref struct VerticalPiece {
	Direction dir;
	WAIS_Shape^ shape;//todo 应该换成TopoDS_Shape
	WAIS_Shape^ edge;//todo 应该换成TopoDS_Edge
	double start; //todo 应该换成Pnt
	double end; //todo 应该换成Pnt
public:
	OCCTK::SimpleClamp::VerticalPiece GetOCC();
	//todo  添加 ToString 方法
	virtual String^ ToString() override {
		String^ piecesStr = String::Format("{0}", start);//todo 应该把对应方向的值输出
		return piecesStr;
	}
};
public ref struct VerticalPlate {
	// 方向的定义参考飞书文档
	Direction direction;
	double location;
	List<VerticalPiece^>^ pieces;
	// 竖板避让间隙
	double clearances;
	// 竖板最小支撑长度
	double minSupportLen;
	// 竖板切断距离
	double cuttingDistance;
	// 竖板连接高
	double connectionHeight;
	// 最终的竖板形状
	WAIS_Shape^ shape;
public:
	OCCTK::SimpleClamp::VerticalPlate GetOCC();
	// 添加 ToString 方法
	virtual String^ ToString() override {
		String^ piecesStr = String::Format("{0}", location);
		return piecesStr;
	}
};

public ref class SimpleClampMaker {
public:
#pragma region demo测试用方法
	static WAIS_Shape^ TestInputWorkpiece(String^ path);
#pragma endregion

	static BasePlate^ MakeBasePlate_NoInteract(WAIS_Shape^ InputAISWorkpiece, double OffsetZ, double BasePlateOffsetX, double BasePlateOffsetY);
	static VerticalPlate^ MakeVerticalPlate(WAIS_Shape^ InputAISWorkpiece, BasePlate^ BasePlate, Direction theDirection, double theLocation, double theClearances, double theMinSupportLen, double theCuttingDistance);
	static List<VerticalPlate^>^ MakeVerticalPlates(WAIS_Shape^ InputAISWorkpiece, BasePlate^ BasePlate, List<double>^ XLocation, List<double>^ YLocation, double theClearances, double theMinSupportLen, double theCuttingDistance);
	static List<VerticalPlate^>^ MakeVerticalPlates(WAIS_Shape^ InputAISWorkpiece, BasePlate^ BasePlate, int XNum, int YNum, double initialOffsetX, double initialOffsetY, double theClearances, double theMinSupportLen, double theCuttingDistance);
	static void SuturePLate(VerticalPlate^% theVerticalPlate, BasePlate^ BasePlate, double theConnectHight);
	static List<VerticalPlate^>^ SuturePLates(List<VerticalPlate^>^ verticalPlates, BasePlate^ BasePlate, double theConnectHight, double theConnectThickness);
};
}

