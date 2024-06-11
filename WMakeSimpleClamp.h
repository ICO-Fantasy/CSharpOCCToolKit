#pragma once
#include "MakeSimpleClamp.h"
#include "WTopoDS_Shape.h"
#include "WAIS_Shape.h"
#include "Wgp_Pnt.h"
#include <vcclr.h>

using namespace  OCCTK::OCC::gp;
using namespace  OCCTK::OCC::AIS;
using namespace  OCCTK::OCC::TopoDS;
using namespace System::Collections::Generic;
using namespace System;

namespace OCCTK::Laser
{
public enum class Direction {
	X,
	Y
};
public ref struct BasePlate {
	WTopoDS_Shape^ shape;
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
	WTopoDS_Shape^ shape;
	WAIS_Shape^ aisShape;
	WTopoDS_Shape^ edge;
	Wgp_Pnt^ start;
	Wgp_Pnt^ end;
public:
	OCCTK::SimpleClamp::VerticalPiece GetOCC();
	virtual String^ ToString() override {
		if (dir == Direction::X)
		{
			String^ piecesStr = String::Format("{0}", start->Y());
			return piecesStr;
		}
		if (dir == Direction::Y)
		{
			String^ piecesStr = String::Format("{0}", start->X());
			return piecesStr;
		}
		String^ piecesStr = "Unexpected Direction";
		return piecesStr;
	}
	virtual String^ ToString(String^ format) override {
		if (format == "F2")
		{
			if (dir == Direction::X)
			{
				String^ piecesStr = String::Format("{0:0.00}", start->Y());;
				return piecesStr;
			}
			if (dir == Direction::Y)
			{
				String^ piecesStr = String::Format("{0:0.00}", start->X());;
				return piecesStr;
			}
			String^ piecesStr = "Unexpected Direction";
			return piecesStr;
		}
		return "";
	}
};
public ref struct VerticalPlate {
	// 方向的定义参考飞书文档
	Direction direction;
	// 当前位置
	double location;
	// 竖板的另一个值，可能是X或者Y
	double XY;
	// 竖板的底边线高度
	double Z;
	// 竖板所包含的片
	List<VerticalPiece^>^ pieces;
	// 竖板避让间隙
	double clearances;
	// 竖板最小支撑长度
	double minSupportLen;
	// 竖板切断距离
	double cuttingDistance;
	// 连接槽宽度
	double connectionThickness;
	// 开槽圆角半径
	double filletRadius;
	// 竖板连接高
	double connectionHeight;
	// 标签编号
	int tagValue;
	// 最终的竖板形状
	WTopoDS_Shape^ shape;
	WAIS_Shape^ aisShape;
public:
	OCCTK::SimpleClamp::VerticalPlate GetOCC();
	// 添加 ToString 方法，默认返回1位小数
	virtual String^ ToString() override {
		String^ piecesStr = String::Format("{0:0.0}", location);
		return piecesStr;
	}
};

public ref class SimpleClampMaker {
public:
	static BasePlate^ MakeBasePlate_NoInteract(WTopoDS_Shape^ InputWorkpiece, double OffsetZ, double BasePlateOffsetX, double BasePlateOffsetY);
	static VerticalPlate^ MakeVerticalPlate(WTopoDS_Shape^ InputWorkpiece, BasePlate^ BasePlate, Direction theDirection, double theLocation, double theClearances, double theMinSupportLen, double theCuttingDistance);
	static List<VerticalPlate^>^ MakeVerticalPlates(WTopoDS_Shape^ InputWorkpiece, BasePlate^ BasePlate, List<double>^ XLocation, List<double>^ YLocation, double theClearances, double theMinSupportLen, double theCuttingDistance);
	static List<VerticalPlate^>^ MakeVerticalPlates(WTopoDS_Shape^ InputWorkpiece, BasePlate^ BasePlate, int XNum, int YNum, double initialOffsetX, double initialOffsetY, double theClearances, double theMinSupportLen, double theCuttingDistance);
	static void SuturePLate(VerticalPlate^% theVerticalPlate, BasePlate^ BasePlate, double theConnectHight);
	static List<VerticalPlate^>^ SuturePLates(List<VerticalPlate^>^ verticalPlates, BasePlate^ BasePlate, double theConnectHight, double theConnectThickness, double theFilletRadius);
	static WTopoDS_Shape^ DeployPlates(List<VerticalPlate^>^ verticalPlates, BasePlate^ BasePlate);
	static void BrandNumber(VerticalPlate^% theVerticalPlate, double hight, int number);
	static void BrandNumber(VerticalPlate^% theVerticalPlate, double hight, int number, Wgp_Pnt^ thePoint);
};
}

