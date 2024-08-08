#pragma once
#include "MakeSimpleClamp.h"
#include <algorithm>
#include <array>
#include <Bnd_Box.hxx>
#include <BRep_Builder.hxx>//test
#include <BRepAdaptor_Curve.hxx>
#include <BRepAlgoAPI_Cut.hxx>
#include <BRepAlgoAPI_Section.hxx>
#include <BRepBndLib.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepFilletAPI_MakeFillet2d.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepLib.hxx>
#include <cmath>
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <gp_EulerSequence.hxx>
#include <gp_Quaternion.hxx>
#include <map>
#include <TCollection_AsciiString.hxx>
#include <TopExp_Explorer.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <utility>
#include <vector>
#include <BRepPrimAPI_MakePrism.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepBuilderAPI_Sewing.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <ShapeFix_Shape.hxx>

static const double LINEAR_TOLERANCE = 1e-2;

namespace OCCTK {
namespace SimpleClamp {

#pragma region 静态方法函数

// 计算2维两直线交点
//static bool Get2DLineIntersection(double x1, double y1, double x2, double y2,
//	double x3, double y3, double x4, double y4, double& outX, double& outY) {
//	// 计算两条线段的方向向量
//	double d1x = x2 - x1;
//	double d1y = y2 - y1;
//	double d2x = x4 - x3;
//	double d2y = y4 - y3;
//
//	Handle(Geom2d_Line) hLine1 = new Geom2d_Line(gp_Pnt2d(x1, y1), gp_Dir2d(x2, y2));
//	Handle(Geom2d_Line) hLine2 = new Geom2d_Line(gp_Pnt2d(x3, y3), gp_Dir2d(x4, y4));
//
//	Geom2dAPI_InterCurveCurve ICC(hLine1, hLine2, 1e-2);
//	if (ICC.NbPoints() > 0) {
//		gp_Pnt2d P = ICC.Point(1);
//		outX = P.X();
//		outY = P.Y();
//		return true;
//	}
//	return false;
//}

static bool Get2DLineIntersection(double x1, double y1, double x2, double y2,
	double x3, double y3, double x4, double y4, double& outX, double& outY) {
	// 计算两条线段的方向向量
	double d1x = x2 - x1;
	double d1y = y2 - y1;
	double d2x = x4 - x3;
	double d2y = y4 - y3;

	// 计算叉积
	double cross = d1x * d2y - d1y * d2x;

	if (std::abs(cross) > 1e-6) {
		// 计算向量 (x3 - x1, y3 - y1) 和 d2 的点积
		double v1x = x3 - x1;
		double v1y = y3 - y1;
		double t = (v1x * d2y - v1y * d2x) / cross;

		// 计算交点
		outX = x1 + t * d1x;
		outY = y1 + t * d1y;

		return true;
	}
	// 如果两直线为 X 和 Y 方向，此时叉积也为 0
	double tx1;
	double ty1;
	double tx2;
	double ty2;
	if (std::abs(d1x) < 1e-6) {
		outX = (x1 * y2 - x2 * y1) / (y2 - y1);
		if (std::abs(d2y) < 1e-6) {
			outY = (x3 * y4 - x4 * y3) / (x3 - x4);
			return true;
		}
	}
	if (std::abs(d1y) < 1e-6) {
		outY = (x1 * y2 - x2 * y1) / (x1 - x2);
		if (std::abs(d2x) < 1e-6) {
			outX = (x3 * y4 - x4 * y3) / (y4 - y3);
			return true;
		}
	}
	return false;
}

// 判断是否可以继续向线段中添加线
static bool AddEdge(Ring& theWire, std::vector<myEdge>& Edges, gp_Pnt& start, gp_Pnt& end) {
	for (size_t i = 0; i < Edges.size(); ++i) {
		auto const& oneEdge = Edges[i];
		if (end.Distance(oneEdge.start) < LINEAR_TOLERANCE) {
			end = oneEdge.end;
			theWire.push_back(oneEdge);
			Edges.erase(Edges.begin() + i);
			return true;
		}
		if (start.Distance(oneEdge.end) < LINEAR_TOLERANCE) {
			start = oneEdge.start;
			theWire.insert(theWire.begin(), oneEdge);
			Edges.erase(Edges.begin() + i);
			return true;
		}
	}
	return false;
}

// 找到并返回一个环
static Ring FindARing(std::vector<myEdge>& Edges) {

	Ring theRing;
	theRing.push_back(Edges.front());
	gp_Pnt start = Edges.front().start;
	gp_Pnt end = Edges.front().end;
	Edges.erase(Edges.begin());

	// 找到匹配的边，并删除该边
	while (!Edges.empty()) {
		bool endFlag = true;
		for (size_t i = 0; i < Edges.size(); ++i) {
			auto oneEdge = Edges[i];
			if (end.Distance(oneEdge.start) < LINEAR_TOLERANCE) {
				end = oneEdge.end;
				theRing.push_back(oneEdge);
				Edges.erase(Edges.begin() + i);
				endFlag = false;
				break;
			}
			if (end.Distance(oneEdge.end) < LINEAR_TOLERANCE) {
				end = oneEdge.start;
				theRing.push_back(oneEdge);
				Edges.erase(Edges.begin() + i);
				endFlag = false;
				break;
			}
			if (start.Distance(oneEdge.end) < LINEAR_TOLERANCE) {
				start = oneEdge.start;
				theRing.insert(theRing.begin(), oneEdge);
				Edges.erase(Edges.begin() + i);
				endFlag = false;
				break;
			}
			if (start.Distance(oneEdge.start) < LINEAR_TOLERANCE) {
				start = oneEdge.end;
				theRing.insert(theRing.begin(), oneEdge);
				Edges.erase(Edges.begin() + i);
				endFlag = false;
				break;
			}
		}
		if (endFlag) { break; }
	}
	return theRing;
}

// 从截面中划分出环
static std::vector<Ring> GetRings(const std::vector<myEdge>& Edges) {
	std::vector<Ring>Rings;
	Ring tempEdges = Edges;
	while (!tempEdges.empty()) {
		// 找出一个 Ring
		Ring aRing = FindARing(tempEdges);

		// 保存 Ring
		Rings.push_back(aRing);

	}
	return Rings;
}

// 把一个环划分为上下两边，取下端的边
static std::vector<myEdge> SplitRing(Ring theRing) {
	// 错误处理，理论上 Ring 不应该少于4条边
	if (theRing.size() < 4) {
		return theRing;
	}

	std::vector<myEdge> removedEdges;
	removedEdges = theRing;


#pragma region 取基准点，并排序

	// 取最远点为基点，在基础上*99，Z为所有点Z的均值，对所有edge的方向进行排序然后用addEdge方法划分两条线
	gp_Pnt maxPoint;
	double maxDis = 0.0;
	for (auto oneEdge : removedEdges) {
		gp_Pnt tempa = gp_Pnt(oneEdge.start);
		tempa.SetZ(0.0);
		if (maxDis < tempa.SquareDistance(gp_Pnt())) {
			maxDis = tempa.SquareDistance(gp_Pnt());
			maxPoint = tempa;
		}
		gp_Pnt tempb = gp_Pnt(oneEdge.end);
		tempb.SetZ(0.0);
		if (maxDis < tempb.SquareDistance(gp_Pnt())) {
			maxDis = tempb.SquareDistance(gp_Pnt());
			maxPoint = tempb;
		}
	}
	// average Z
	double totalZ = 0.0;
	double totalNum = 0.0;
	for (auto oneEdge : removedEdges) {
		totalZ += oneEdge.start.Z();
		totalZ += oneEdge.end.Z();
		totalNum += 2.0;
	}
	// get Max Point
	maxPoint.SetX(maxPoint.X() * 99.0);
	maxPoint.SetY(maxPoint.Y() * 99.0);
	maxPoint.SetZ(totalZ / totalNum);

	// 对Edge两端点重新排序(start更近)
	for (size_t i = 0; i < removedEdges.size(); i++) {
		auto oneEdge = removedEdges[i];
		gp_Pnt tempStart = removedEdges[i].start;
		gp_Pnt  tempEnd = removedEdges[i].end;
		if (maxPoint.SquareDistance(tempStart) > maxPoint.SquareDistance(tempEnd)) {
			std::swap(tempStart, tempEnd);
		}
		removedEdges[i].start = tempStart;
		removedEdges[i].end = tempEnd;
	}

	// 对Edges重新排序(start更近)
	std::sort(removedEdges.begin(), removedEdges.end(),
		[&](const myEdge& edge1, const myEdge& edge2) {
			return maxPoint.SquareDistance(edge1.start) < maxPoint.SquareDistance(edge2.start);
		});

#pragma endregion

	// 构建第一条线
	std::vector<myEdge> firstWire, secondWire;
	gp_Pnt startP = removedEdges.front().start;
	gp_Pnt endP = removedEdges.front().end;
	firstWire.push_back(removedEdges.front());
	removedEdges.erase(removedEdges.begin());
	while (AddEdge(firstWire, removedEdges, startP, endP)) {}

	// 直接取剩余线作为第二条
	for (const auto& oneEdge : removedEdges) {
		bool found = false;
		for (const auto& firstEdge : firstWire) {
			if (oneEdge == firstEdge) {
				found = true;
				break;
			}
		}
		if (!found) {
			secondWire.push_back(oneEdge);
		}
	}

	// 比较高度

	// 去除垂直的边
	gp_Vec Z(0.0, 0.0, 1.0);
	Ring Temp1, Temp2;
	for (myEdge theEdge : firstWire) {
		if (!gp_Vec(theEdge.start, theEdge.end).IsParallel(Z, 0.1)) {
			Temp1.push_back(theEdge);
		}
	}
	firstWire = Temp1;
	for (myEdge theEdge : secondWire) {
		if (!gp_Vec(theEdge.start, theEdge.end).IsParallel(Z, 0.1)) {
			Temp2.push_back(theEdge);
		}
	}
	secondWire = Temp2;

	//! 使用两端的 Z 值比较可能不准确（由于斜着的边缘）
	for (size_t i = 1; i < firstWire.size() - 1; ++i) {
		const myEdge& firstEdge = firstWire[i];
		bool isSame = false;
		for (size_t j = 0; j < secondWire.size(); ++j) {
			const myEdge& secondEdge = secondWire[j];
			gp_Pnt2d a(firstEdge.start.X(), firstEdge.start.Y());
			gp_Pnt2d b(secondEdge.start.X(), secondEdge.start.Y());

			if (a.Distance(b) < 3.0) {
				isSame = true;
			}

			if (isSame) {
				if (firstEdge.start.Z() < secondEdge.start.Z()) {
					return firstWire;
				}
				else {
					return secondWire;
				}
			}
		}
	}

	if ((!firstWire.empty()) && (!secondWire.empty())) {
		if (firstWire.front().middle.Z() < secondWire.front().middle.Z()) {
			return firstWire;
		}
		else {
			return secondWire;
		}
	}
	return firstWire;
}

// 拆分多位整数为单位数
static std::vector<int> GetDigitValues(int number) {
	std::vector<int> digits;
	if (number < 0) {
		number = -number; // 处理负数
	}
	do {
		digits.push_back(number % 10); // 获取当前最低位数字
		number /= 10; // 去掉最低位数字
	} while (number != 0);
	std::reverse(digits.begin(), digits.end()); // 反转结果
	return digits;
}

static std::vector<int> GetNumberString(TCollection_AsciiString numberString) {
	std::vector<int> digits;
	for (size_t i = 1; i < numberString.Length() + 1; ++i) {
		char text = numberString.Value(i);
		// 属于数字0-9
		if (text >= '0' && text <= '9') {
			int digit = text - '0'; // 将字符转换为对应的整数值
			digits.push_back(digit);
		}
		// 属于字母 'X'
		else if (text == 'X') {
			digits.push_back(10);
		}
		// 属于字母 'Y'
		else if (text == 'Y') {
			digits.push_back(11);
		}
		else {
			digits.push_back(12);
		}
	}
	return digits;
}

#pragma endregion

#pragma region 生成底板

// 根据工件的 AABB 包围盒得到两个角点（Z 值为 0）
void GetCorners(TopoDS_Shape theShape, BasePlate& theBasePlate) {
	Bnd_Box bbox;
	BRepBndLib::Add(theShape, bbox);
	double xMin, yMin, zMin, xMax, yMax, zMax;
	bbox.Get(xMin, yMin, zMin, xMax, yMax, zMax);
	theBasePlate.X = xMin;
	theBasePlate.Y = yMin;
	theBasePlate.Z = zMin;
	theBasePlate.dX = xMax - xMin;
	theBasePlate.dY = yMax - yMin;
	theBasePlate.lowestZ = zMin;
	theBasePlate.hight = zMax - zMin;
}

// 根据两个角点和 Z 值创建底板
void MakeBasePlateShape(BasePlate& theBaseplate) {
	// 根据边界框尺寸创建立方体
	gp_Pnt theLowLeft(theBaseplate.X - theBaseplate.offsetX, theBaseplate.Y - theBaseplate.offsetY, theBaseplate.Z);
	gp_Pnt theTopRight(theBaseplate.X + theBaseplate.dX + theBaseplate.offsetX, theBaseplate.Y + theBaseplate.dY + theBaseplate.offsetY, theBaseplate.Z);

	gp_Pnt p1, p2, p3, p4;
	p1 = theLowLeft;
	p2 = gp_Pnt(theLowLeft.X(), theTopRight.Y(), theBaseplate.Z);
	p3 = theTopRight;
	p4 = gp_Pnt(theTopRight.X(), theLowLeft.Y(), theBaseplate.Z);
	// 创建边
	BRepBuilderAPI_MakeEdge l1(p1, p2);
	BRepBuilderAPI_MakeEdge l2(p2, p3);
	BRepBuilderAPI_MakeEdge l3(p3, p4);
	BRepBuilderAPI_MakeEdge l4(p4, p1);
	// 创建线段
	BRepBuilderAPI_MakeWire wire;
	wire.Add(l1);
	wire.Add(l2);
	wire.Add(l3);
	wire.Add(l4);
	// 创建面
	BRepBuilderAPI_MakeFace face(wire.Wire(), true);
	theBaseplate.shape = face.Shape();
}

// 非交互的生成底板
BasePlate MakeBasePlate(TopoDS_Shape theWorkpiece, double theOffsetZ, double theOffsetX, double theOffsetY) {
	ShapeFix_Shape shapeFixer(theWorkpiece);
	shapeFixer.Perform();

	BasePlate theBaseplate;
	theBaseplate.offsetX = theOffsetX;
	theBaseplate.offsetY = theOffsetY;
	theBaseplate.offsetZ = theOffsetZ;
	GetCorners(shapeFixer.Shape(), theBaseplate);
	theBaseplate.Z -= theOffsetZ;
	MakeBasePlateShape(theBaseplate);
	return theBaseplate;
}

#pragma endregion

#pragma region 生成竖板

// 得到切割后的工件平面
static TopoDS_Shape MakeSection(PlatePose thePose, const TopoDS_Shape& theWorkpiece) {
	BRepAlgoAPI_Section aSection = BRepAlgoAPI_Section(theWorkpiece, BRepBuilderAPI_MakeFace(thePose.Plane()));
	aSection.Build();
	if (aSection.IsDone()) {
		TopoDS_Shape sec_shape = aSection.Shape();
		return sec_shape;
	}
	throw std::runtime_error("截面获取失败");
}

// 从截平面得到单块板
VerticalPlate MakeVerticalPieceWithSection(VerticalPlate& thePalte, TopoDS_Shape theSection) {
	PlatePose thePose = thePalte.pose;
	double theZ = thePalte.Z;
#pragma region 得到原始构造线
	//! 首先判断有几个环，对环进行处理
	std::vector<myEdge> TempEdges;
	std::vector<Ring> Rings;
	TopExp_Explorer aEdgeExp = TopExp_Explorer(theSection, TopAbs_EDGE);
	while (aEdgeExp.More()) {
		myEdge anEdge(TopoDS::Edge(aEdgeExp.Current()), thePose);// 遍历得到每一个边
		aEdgeExp.Next();
		TempEdges.push_back(anEdge);
	}
	Rings = GetRings(TempEdges);
	//! 每个环中取最下边的线段作为原始构造线
	std::vector<myEdge> bottomEdges;
	for (Ring aRing : Rings) {
		bottomEdges = SplitRing(aRing);

#pragma region 构造 Pieces
		for (myEdge anEdge : bottomEdges) {
			VerticalPiece anPiece(thePose, anEdge, theZ);
			thePalte.pieces.push_back(anPiece);
		}
#pragma endregion

	}
#pragma endregion

	return thePalte;
}

// 修剪边的两端
static TopoDS_Edge TrimEdge(const TopoDS_Edge theOriginEdge, gp_Pnt p1, gp_Pnt p2) {
	//获取底层曲线
	double first, last;
	Handle(Geom_Curve) aCurve = BRep_Tool::Curve(theOriginEdge, first, last);
	// 有时底层曲线没有创建，要手动创建
	if (aCurve.IsNull()) {
		BRepLib::BuildCurves3d(theOriginEdge, 1.0e-5, GeomAbs_C1);//创建曲线 (一阶导数的连续性)
		aCurve = BRep_Tool::Curve(theOriginEdge, first, last);
	}
	if (!aCurve.IsNull()) {
		//投影点到曲线上，并获取投影点处的参数
		GeomAPI_ProjectPointOnCurve ppc1(p1, aCurve);
		double param1 = ppc1.LowerDistanceParameter();
		GeomAPI_ProjectPointOnCurve ppc2(p2, aCurve);
		double param2 = ppc2.LowerDistanceParameter();
		if (param1 > param2) {
			std::swap(param1, param2);
		}
		//如果投影点参数小于起始参数或大于终止参数，则分割失败  
		if (first > param1 || param1 > last || first > param2 || param2 > last)
			return TopoDS_Edge();
		//处于起始和终止参数中间，则构建两个新边
		else {
			TopoDS_Edge newEdge = BRepBuilderAPI_MakeEdge(aCurve, param1, param2);
			newEdge.Orientation(theOriginEdge.Orientation());//同向
			return newEdge;
		}
	}
	//如果失败了返回它本身
	return theOriginEdge;
}

// 根据长度修剪线
static VerticalPlate TrimEdgeEnds(VerticalPlate& thePlate) {
	std::vector<VerticalPiece> tempPieces;
	for (size_t i = 0; i < thePlate.pieces.size(); ++i) {
		auto aPiece = thePlate.pieces[i];
		if (aPiece.Length() < thePlate.clearances * 2) {
			// 去掉修剪后长度过小的边
			continue;
		}

		gp_Pnt p1 = aPiece.myEdge.start;
		gp_Pnt p2 = aPiece.myEdge.end;
		double ratio = thePlate.clearances / aPiece.Length();
		p1.SetX(p1.X() + (p2.X() - p1.X()) * ratio);
		p1.SetY(p1.Y() + (p2.Y() - p1.Y()) * ratio);

		p2.SetX(p2.X() + (p1.X() - p2.X()) * ratio);
		p2.SetY(p2.Y() + (p1.Y() - p2.Y()) * ratio);

		TopoDS_Edge outEdge = TrimEdge(aPiece.myEdge.edge, p1, p2);
		if (!outEdge.IsNull()) {
			tempPieces.push_back(VerticalPiece(aPiece.pose, myEdge(outEdge, aPiece.pose), aPiece.Z));
		}
	}
	thePlate.pieces = tempPieces;
	return thePlate;
}

// 移除长度过短的边
static VerticalPlate RemoveShortEdge(VerticalPlate& ThePlate) {
	std::vector<VerticalPiece> tempPieces;
	for (auto aPiece : ThePlate.pieces) {
		if (aPiece.Length() > ThePlate.minSupportLen) {
			tempPieces.push_back(aPiece);
		}
	}
	ThePlate.pieces = tempPieces;
	return ThePlate;
}

static std::vector<VerticalPiece> CutEdgeUniform(VerticalPiece thePiece, double supLen, double cutLen) {
	std::vector<VerticalPiece> result;
	// 如果最小支撑长度不足，则不做切割
	if (supLen <= 1) {
		result.push_back(thePiece);
		return result;
	}

	double edgeLength = thePiece.Length();
	// 获取底层曲线
	double first, last, left, right;
	Handle(Geom_Curve) aCurve = BRep_Tool::Curve(thePiece.myEdge.edge, first, last);

	gp_Pnt startP = thePiece.myEdge.start, endP = thePiece.myEdge.end;
	//以中点的Z值XOY平面作为投影平面
	startP.SetZ(thePiece.myEdge.middle.Z());
	endP.SetZ(thePiece.myEdge.middle.Z());
	double param1, param2;


	double currentNum = 0.0;
	while (currentNum * (supLen + cutLen) < thePiece.Length()) {
		gp_Pnt start, end;

		gp_Trsf toStart = gp_Trsf();
		toStart.SetTranslation(gp_Vec(startP, endP).Normalized().Multiplied(currentNum * (supLen + cutLen)));
		start = startP.Transformed(toStart);

		gp_Trsf toEnd = gp_Trsf();
		toEnd.SetTranslation(gp_Vec(startP, endP).Normalized().Multiplied(supLen));
		end = start.Transformed(toEnd);

		//投影点到曲线上，并获取投影点处的参数
		GeomAPI_ProjectPointOnCurve ppc1S(start, aCurve);
		param1 = ppc1S.LowerDistanceParameter();
		GeomAPI_ProjectPointOnCurve ppc2S(end, aCurve);
		param2 = ppc2S.LowerDistanceParameter();

		TopoDS_Edge newEdge = BRepBuilderAPI_MakeEdge(aCurve, param1, param2);
		myEdge newmyEdge(newEdge, thePiece.pose);
		//newEdge.Orientation(thePiece.myEdge.edge.Orientation());//设置线的方向为同向

		VerticalPiece newPiece(thePiece.pose, newmyEdge, thePiece.Z);
		result.push_back(newPiece);
		currentNum += 1.0;
	}

	return result;
}

//static std::vector<VerticalPiece> CutEdgeAlongDir(VerticalPiece thePiece, double theSupportLen, double theCutLen, int theNum) {
//	std::vector<VerticalPiece> result;
//	// 如果最小支撑长度不足，则不做切割
//	if (theSupportLen <= 1) {
//		result.push_back(thePiece);
//		return result;
//	}
//	// 获取底层曲线
//	double first, last;
//	Handle(Geom_Curve) aCurve = BRep_Tool::Curve(thePiece.edge, first, last);
//	double start = 0, end = 0;
//	gp_Pnt startP = thePiece.startPoint, endP = thePiece.startPoint;
//	// 第一条
//	switch (thePiece.dir) {
//	case X:
//		start = thePiece.startPoint.Y();
//		end = thePiece.startPoint.Y() + theSupportLen;
//
//		startP.SetY(start);
//		endP.SetY(end);
//		break;
//	case Y:
//		start = thePiece.startPoint.X();
//		end = thePiece.startPoint.X() + theSupportLen;
//
//		startP.SetX(start);
//		endP.SetX(end);
//		break;
//	default:
//		break;
//	}
//	// 投影点到曲线上，并获取投影点处的参数
//	GeomAPI_ProjectPointOnCurve ppc1S(startP, aCurve);
//	double param1S = ppc1S.LowerDistanceParameter();
//	GeomAPI_ProjectPointOnCurve ppc2S(endP, aCurve);
//	double param2S = ppc2S.LowerDistanceParameter();
//	VerticalPiece newPieceS;
//	newPieceS.dir = thePiece.dir;
//	TopoDS_Edge newEdgeS = BRepBuilderAPI_MakeEdge(aCurve, param1S, param2S);
//	newEdgeS.Orientation(thePiece.edge.Orientation());//同向
//	newPieceS.edge = newEdgeS;
//	result.push_back(newPieceS);
//	// 第一条之后的
//	for (size_t i = 0; i < theNum; i++) {
//		switch (thePiece.dir) {
//		case X:
//			start += theSupportLen + theCutLen;
//			end += theSupportLen + theCutLen;
//
//			startP.SetY(start);
//			endP.SetY(end);
//			break;
//		case Y:
//			start += theSupportLen + theCutLen;
//			end += theSupportLen + theCutLen;
//
//			startP.SetX(start);
//			endP.SetX(end);
//			break;
//		default:
//			break;
//		}
//		//投影点到曲线上，并获取投影点处的参数
//		GeomAPI_ProjectPointOnCurve ppc1(startP, aCurve);
//		double param1 = ppc1.LowerDistanceParameter();
//		GeomAPI_ProjectPointOnCurve ppc2(endP, aCurve);
//		double param2 = ppc2.LowerDistanceParameter();
//		VerticalPiece newPiece;
//		newPiece.dir = thePiece.dir;
//		TopoDS_Edge newEdge = BRepBuilderAPI_MakeEdge(aCurve, param1, param2);
//		newEdge.Orientation(thePiece.edge.Orientation());//同向
//		newPiece.edge = newEdge;
//		result.push_back(newPiece);
//	}
//	return result;
//}

// 切开过长的边，与修剪两端类似的做法
static VerticalPlate CutLongEdge(VerticalPlate& thePlate) {
	double theSupportLen = thePlate.minSupportLen;
	double theCuttingDistance = thePlate.cuttingDistance;
	//如果两个参数值均为 0，则不处理
	if (theSupportLen + theCuttingDistance == 0) {
		return thePlate;
	}

	std::vector<VerticalPiece> tempPieces;
	for (auto aPiece : thePlate.pieces) {
		//跳过不需要切割的长度
		if (aPiece.Length() < theSupportLen * 2 + theCuttingDistance) {
			tempPieces.push_back(aPiece);
			continue;
		}

		// 以 (theSupportLen + theCuttingDistance)作为分割依据，调整实际的 supLen, cutLen 值
		// 如果小于它，同时增加 supLen, cutLen 
		double supLen, cutLen;
		double remainder = std::fmod(aPiece.Length() - theSupportLen, (theSupportLen + theCuttingDistance));
		int quotient = std::trunc((aPiece.Length() - theSupportLen) / (theSupportLen + theCuttingDistance));

		supLen = theSupportLen + remainder / (double)(quotient + 1) * ((double)(quotient + 1) / (double)(quotient * 2 + 1));
		cutLen = theCuttingDistance + remainder / (double)quotient * ((double)quotient / (double)(quotient * 2 + 1));
		for (auto aP : CutEdgeUniform(aPiece, supLen, cutLen)) {
			tempPieces.push_back(aP);
		}

		////计算需要切割的间隙是奇数还是偶数
		//int quotient = (int)(aPiece.Length() - theSupportLen) / (theSupportLen + theCuttingDistance);
		//if (quotient % 2 == 0) {
		//	// 商是偶数
		//	for (auto aP : CutEdgeUniform(aPiece, theSupportLen, quotient)) {
		//		tempPieces.push_back(aP);
		//	}
		//}
		//else {
		//	// 商是奇数，实际切割长度按照数目均分即可
		//	double restLen = (aPiece.Length() - theSupportLen) / quotient;
		//	double cutLen = restLen - theSupportLen;
		//	for (auto aP : CutEdgeAlongDir(aPiece, theSupportLen, cutLen, quotient)) {
		//		tempPieces.push_back(aP);
		//	}
		//}
	}
	thePlate.pieces = tempPieces;
	return thePlate;
}

// 生成仅含 Pieces 的竖板
VerticalPlate MakeVerticalPlate(TopoDS_Shape theWorkpiece, BasePlate theBasePlate, PlatePose thePose, double theClearances, double theMinSupportLen, double theCutDistance) {
	VerticalPlate result;
	result.pose = thePose;
	result.Z = theBasePlate.Z;
	result.clearances = theClearances;
	result.minSupportLen = theMinSupportLen;
	result.cuttingDistance = theCutDistance;

	//切割工件
	TopoDS_Shape aSection = MakeSection(thePose, theWorkpiece);
	result = MakeVerticalPieceWithSection(result, aSection);
	//auto debug1 = result.pieces;
	result = TrimEdgeEnds(result);
	//auto debug2 = result.pieces;
	result = RemoveShortEdge(result);
	//auto debug3 = result.pieces;
	result = CutLongEdge(result);
	//auto debug4 = result.pieces;


	return result;
}

// 合并片体为一块板
VerticalPlate SuturePiece(VerticalPlate& thePlate, const BasePlate theBase, double theAvoidanceHeight, double theConnectionThickness) {
	if (thePlate.pieces.empty()) { return thePlate; }
	//! 板宽度不包含 Offset 的部分
	thePlate.avoidanceHeight = theAvoidanceHeight;
	thePlate.connectionThickness = theConnectionThickness;
	const double Z = thePlate.Z;
	const double ZC = theBase.lowestZ - theAvoidanceHeight;
	Ring tempEdges;
	for (auto anPiece : thePlate.pieces) {
		tempEdges.push_back(anPiece.myEdge);
	}

#pragma region 取基准点，并排序
	double refZ;
	// 取最远点为基点，在基础上*99，Z为所有点Z的均值，对所有edge的方向进行排序
	gp_Pnt maxPoint;
	double maxDis = 0.0;
	for (auto oneEdge : tempEdges) {
		gp_Pnt tempa = gp_Pnt(oneEdge.start);
		tempa.SetZ(0.0);
		if (maxDis < tempa.SquareDistance(gp_Pnt())) {
			maxDis = tempa.SquareDistance(gp_Pnt());
			maxPoint = tempa;
		}
		gp_Pnt tempb = gp_Pnt(oneEdge.end);
		tempb.SetZ(0.0);
		if (maxDis < tempb.SquareDistance(gp_Pnt())) {
			maxDis = tempb.SquareDistance(gp_Pnt());
			maxPoint = tempb;
		}
	}
	// average Z
	double totalZ = 0.0;
	double totalNum = 0.0;
	for (auto oneEdge : tempEdges) {
		totalZ += oneEdge.start.Z();
		totalZ += oneEdge.end.Z();
		totalNum += 2.0;
	}
	// get Max Point
	maxPoint.SetX(maxPoint.X() * 99.0);
	maxPoint.SetY(maxPoint.Y() * 99.0);
	refZ = totalZ / totalNum;
	maxPoint.SetZ(refZ);

	// 对Edge两端点重新排序(start更近)
	for (size_t i = 0; i < tempEdges.size(); i++) {
		auto oneEdge = tempEdges[i];
		gp_Pnt tempStart = tempEdges[i].start;
		gp_Pnt tempEnd = tempEdges[i].end;
		if (maxPoint.SquareDistance(tempStart) > maxPoint.SquareDistance(tempEnd)) {
			std::swap(tempStart, tempEnd);
		}
		tempEdges[i].start = tempStart;
		tempEdges[i].end = tempEnd;
	}

	// 对Edges重新排序(start更近)
	std::sort(tempEdges.begin(), tempEdges.end(),
		[&](const myEdge& edge1, const myEdge& edge2) {
			return maxPoint.SquareDistance(edge1.start) < maxPoint.SquareDistance(edge2.start);
		});

#pragma endregion

	const gp_Vec plateDir(tempEdges.front().start, tempEdges.back().end);

#pragma region 线段生成
	// basePlate
	double X1 = theBase.X; double X2 = theBase.X + theBase.dX;
	if (X1 > X2) { std::swap(X1, X2); }
	double Y1 = theBase.Y; double Y2 = theBase.Y + theBase.dY;
	if (Y1 > Y2) { std::swap(Y1, Y2); }

	// 待求线段
	gp_Pnt pStart = tempEdges.front().start;
	gp_Pnt pEnd = tempEdges.back().end;
	double p1x = pStart.X();
	double p1y = pStart.Y();
	double p2x = pEnd.X();
	double p2y = pEnd.Y();

	gp_Pnt bStart, bEnd;
	std::vector<std::pair<double, double>> tempIs;
	// 有交点，并且在线框范围内
	double TOL = 1e-4;
	double isx1, isy1;
	if (Get2DLineIntersection(p1x, p1y, p2x, p2y, X1, Y1, X1, Y2, isx1, isy1)) {
		if (X1 - TOL <= isx1 && isx1 <= X2 + TOL && Y1 - TOL <= isy1 && isy1 <= Y2 + TOL) {
			tempIs.push_back({ isx1,isy1 });
		}
	}
	double isx2, isy2;
	if (Get2DLineIntersection(p1x, p1y, p2x, p2y, X1, Y2, X2, Y2, isx2, isy2)) {
		if (X1 - TOL <= isx2 && isx2 <= X2 + TOL && Y1 - TOL <= isy2 && isy2 <= Y2 + TOL) {
			tempIs.push_back({ isx2,isy2 });
		}
	}
	double isx3, isy3;
	if (Get2DLineIntersection(p1x, p1y, p2x, p2y, X2, Y2, X2, Y1, isx3, isy3)) {
		if (X1 - TOL <= isx3 && isx3 <= X2 + TOL && Y1 - TOL <= isy3 && isy3 <= Y2 + TOL) {
			tempIs.push_back({ isx3,isy3 });
		}
	}
	double isx4, isy4;
	if (Get2DLineIntersection(p1x, p1y, p2x, p2y, X2, Y1, X1, Y1, isx4, isy4)) {
		if (X1 - TOL <= isx4 && isx4 <= X2 + TOL && Y1 - TOL <= isy4 && isy4 <= Y2 + TOL) {
			tempIs.push_back({ isx4,isy4 });
		}
	}
	if (tempIs.size() >= 2) {
		bStart = gp_Pnt(tempIs[0].first, tempIs[0].second, refZ);
		bEnd = gp_Pnt(tempIs[1].first, tempIs[1].second, refZ);
	}
	else {
		bStart = pStart;
		bEnd = pEnd;
	}
	if (bStart.Distance(pStart) > bEnd.Distance(pStart)) { std::swap(bStart, bEnd); }

	//! 线段生成逻辑见飞书
	BRepBuilderAPI_MakeWire wire;

	// 首段点
	gp_Pnt startP0, startP1, startP2;
	startP0 = bStart;
	startP0.SetZ(Z);
	startP1 = bStart;
	startP1.SetZ(ZC);
	startP2 = pStart;
	startP2.SetZ(ZC);
	thePlate.start = startP0;
	// 末段点
	gp_Pnt endP0, endP1;
	endP0 = bEnd;
	endP0.SetZ(ZC);
	endP1 = bEnd;
	endP1.SetZ(Z);
	thePlate.end = endP1;

	//std::vector<gp_Pnt>debugPs;

	// 首段线
	wire.Add(BRepBuilderAPI_MakeEdge(startP0, startP1));
	wire.Add(BRepBuilderAPI_MakeEdge(startP1, startP2));

	//debugPs.push_back(startP0);
	//debugPs.push_back(startP1);

	// 中间线段
	for (size_t i = 0; i < tempEdges.size(); ++i) {
		gp_Pnt startTempP = tempEdges[i].start;
		gp_Pnt endTempP = tempEdges[i].end;

		// 前半段
		gp_Pnt p1, p2;

		p1 = startTempP;
		p1.SetZ(ZC);

		p2 = startTempP;

		BRepBuilderAPI_MakeEdge l1(p1, p2);
		wire.Add(l1);

		TopoDS_Edge l2 = tempEdges[i].edge; // l2 为 Edge本身
		wire.Add(l2);
		// 后半段
		gp_Pnt p3, p4, p5;
		// 最后一的后半段需要额外处理
		if (i == tempEdges.size() - 1) {
			p3 = endTempP;

			p4 = endTempP;
			p4.SetZ(ZC);

			p5 = endP0;
		}
		else {
			p3 = endTempP;

			p4 = endTempP;
			p4.SetZ(ZC);

			gp_Pnt nextStartTempP = tempEdges[i + 1].start;
			p5 = nextStartTempP;
			p5.SetZ(ZC);
		}
		BRepBuilderAPI_MakeEdge l3(p3, p4);
		BRepBuilderAPI_MakeEdge l4(p4, p5);
		wire.Add(l3);
		wire.Add(l4);
		//debugPs.push_back(p1);
		//debugPs.push_back(p2);
		//debugPs.push_back(p3);
		//debugPs.push_back(p4);
	}

	//// 正中间插槽片的四个点
	//double middleLength = thePlate.slotLength;
	//double middleHight = Z - theConnectionThickness;
	//gp_Pnt bMiddle = gp_Pnt((bStart.X() + bEnd.X()) / 2, (bStart.Y() + bEnd.Y()) / 2, Z);
	//thePlate.middle = bMiddle;
	//
	//gp_Trsf backT = gp_Trsf();
	////auto debugDir1 = plateDir.Normalized();
	////auto debugDir2 = debugDir1.Multiplied(-middleLength / 2);
	//backT.SetTranslationPart(plateDir.Normalized().Multiplied(middleLength / 2));
	//gp_Pnt middleP0 = bMiddle.Transformed(backT);
	//gp_Pnt middleP1 = middleP0;
	//middleP1.SetZ(middleHight);
	//gp_Trsf frontT = gp_Trsf();
	//frontT.SetTranslationPart(plateDir.Normalized().Multiplied(-middleLength / 2));
	//gp_Pnt middleP3 = bMiddle.Transformed(frontT);
	//gp_Pnt middleP2 = middleP3;
	//middleP2.SetZ(middleHight);

	//BRepBuilderAPI_MakeEdge endL1(endP1, middleP0);
	//BRepBuilderAPI_MakeEdge endL2(middleP0, middleP1);
	//BRepBuilderAPI_MakeEdge endL3(middleP1, middleP2);
	//BRepBuilderAPI_MakeEdge endL4(middleP2, middleP3);
	//BRepBuilderAPI_MakeEdge endL5(middleP3, startP0);

	// 末段线
	wire.Add(BRepBuilderAPI_MakeEdge(endP0, endP1));

	//debugPs.push_back(endP0);
	//debugPs.push_back(endP1);
	//debugPs.push_back(startP0);
	//for (gp_Pnt aP : debugPs) {
	//	double x = aP.X();
	//	double y = aP.Y();
	//	double z = aP.Z();
	//	continue;
	//}
#pragma endregion

	// 创建面
	//BRepBuilderAPI_MakeFace face(wire.Wire());
	thePlate._unclosedWire = wire;
	//thePlate.shape = wire.Shape();// debug

	return thePlate;
}

// 在竖板上切连接槽
VerticalPlate SlotVerticalPlate(VerticalPlate& thePlate, std::vector<VerticalPlate> otherPlates, double theFilletRadius, bool middleToDown) {
	if (thePlate.pieces.empty()) { return thePlate; }
	double TOL = 1e-2;
	double offset = 0.9;
	double Z = thePlate.Z;
	// 找到板和其它板的交点
	std::vector<gp_Pnt> cutPoints;
	double x1 = thePlate.start.X();
	double y1 = thePlate.start.Y();
	double x2 = thePlate.end.X();
	double y2 = thePlate.end.Y();
	if (x1 > x2) { std::swap(x1, x2); }
	if (y1 > y2) { std::swap(y1, y2); }
	for (auto other : otherPlates) {
		double x3 = other.start.X();
		double y3 = other.start.Y();
		double x4 = other.end.X();
		double y4 = other.end.Y();
		double cx, cy;
		if (Get2DLineIntersection(x1, y1, x2, y2, x3, y3, x4, y4, cx, cy)) {
			if (x1 - TOL <= cx && cx <= x2 + TOL && y1 - TOL <= cy && cy <= y2 + TOL) {
				gp_Pnt cutPoint = gp_Pnt(cx, cy, Z);
				cutPoints.push_back(cutPoint);
			}
		}
	}
	std::sort(cutPoints.begin(), cutPoints.end(), [&](const gp_Pnt& p1, const gp_Pnt& p2) {
		return p1.Distance(thePlate.end) < p2.Distance(thePlate.end);
		});

	double slotL = thePlate.slotLength / 2;
	double slotH = thePlate.slotHight;
	double ct = thePlate.connectionThickness / 2;
	gp_Pnt lastP = thePlate.end;
	// 在每个位置切槽
	for (gp_Pnt aPnt : cutPoints) {
		gp_Vec plateDir(thePlate.start, thePlate.end);
		plateDir.Normalize();

		gp_Pnt r1 = aPnt;
		gp_Trsf Tr1;
		Tr1.SetTranslation(plateDir.Multiplied(slotL));
		r1.Transform(Tr1);
		gp_Pnt r2 = aPnt;
		gp_Trsf Tr2;
		Tr2.SetTranslation(plateDir.Multiplied(slotL).Added(gp_Vec(0, 0, -ct * 2)));
		r2.Transform(Tr2);
		gp_Pnt l1 = aPnt;
		gp_Trsf Tl1;
		Tl1.SetTranslation(plateDir.Multiplied(-slotL).Added(gp_Vec(0, 0, -ct * 2)));
		l1.Transform(Tl1);
		gp_Pnt l2 = aPnt;
		gp_Trsf Tl2;
		Tl2.SetTranslation(plateDir.Multiplied(-slotL));
		l2.Transform(Tl2);
		if (middleToDown) {
			// 从中间往下
			gp_Pnt p3 = aPnt;
			gp_Trsf Tp3;
			Tp3.SetTranslation(plateDir.Multiplied(ct).Added(gp_Vec(0, 0, -ct * 2)));
			p3.Transform(Tp3);
			gp_Pnt p4 = aPnt;
			gp_Trsf Tp4;
			Tp4.SetTranslation(plateDir.Multiplied(ct).Added(gp_Vec(0, 0, slotH)));
			p4.Transform(Tp4);
			gp_Pnt p5 = aPnt;
			gp_Trsf Tp5;
			Tp5.SetTranslation(plateDir.Multiplied(-ct).Added(gp_Vec(0, 0, slotH)));
			p5.Transform(Tp5);
			gp_Pnt p6 = aPnt;
			gp_Trsf Tp6;
			Tp6.SetTranslation(plateDir.Multiplied(-ct).Added(gp_Vec(0, 0, -ct * 2)));
			p6.Transform(Tp6);

			thePlate._unclosedWire.Add(BRepBuilderAPI_MakeEdge(lastP, r1));
			thePlate._unclosedWire.Add(BRepBuilderAPI_MakeEdge(r1, r2));
			thePlate._unclosedWire.Add(BRepBuilderAPI_MakeEdge(r2, p3));
			thePlate._unclosedWire.Add(BRepBuilderAPI_MakeEdge(p3, p4));
			thePlate._unclosedWire.Add(BRepBuilderAPI_MakeEdge(p4, p5));
			thePlate._unclosedWire.Add(BRepBuilderAPI_MakeEdge(p5, p6));
			thePlate._unclosedWire.Add(BRepBuilderAPI_MakeEdge(p6, l1));
			thePlate._unclosedWire.Add(BRepBuilderAPI_MakeEdge(l1, l2));
		}
		else {
			// 从中间往上
			thePlate._unclosedWire.Add(BRepBuilderAPI_MakeEdge(lastP, r1));
			thePlate._unclosedWire.Add(BRepBuilderAPI_MakeEdge(r1, r2));
			thePlate._unclosedWire.Add(BRepBuilderAPI_MakeEdge(r2, l1));
			thePlate._unclosedWire.Add(BRepBuilderAPI_MakeEdge(l1, l2));
		}
		lastP = l2;

		//TopoDS_Shape theSlot;
		//if (middleToDown) {
		//	// 创建一个圆柱 (从中间往下）
		//	theSlot = BRepPrimAPI_MakeCylinder(gp_Ax2(aPnt, gp_Dir(0.0, 0.0, -1.0)), thePlate.connectionThickness / 2, 999.0).Shape();
		//}
		//else {
		//	// 创建一个圆柱 (从中间往上）
		//	theSlot = BRepPrimAPI_MakeCylinder(gp_Ax2(aPnt, gp_Dir(0.0, 0.0, 1.0)), thePlate.connectionThickness / 2, 999.0).Shape();
		//}
	}
	thePlate._unclosedWire.Add(BRepBuilderAPI_MakeEdge(lastP, thePlate.start));
	TopoDS_Shape finalShape = BRepBuilderAPI_MakeFace(thePlate._unclosedWire).Shape();
	// 还需要额外处理切除
	if (!middleToDown) {
		double ZC = thePlate.Z + thePlate.slotHight;
		// 创建一个圆柱 (从中间往上）
		for (gp_Pnt aPnt : cutPoints) {
			gp_Pnt cutPnt = aPnt;
			cutPnt.SetZ(ZC);
			TopoDS_Shape theSlot = BRepPrimAPI_MakeCylinder(gp_Ax2(cutPnt, gp_Dir(0.0, 0.0, 1.0)), thePlate.connectionThickness, 999.0).Shape();
			finalShape = BRepAlgoAPI_Cut(finalShape, theSlot).Shape();
		}
	}
	thePlate.shape = finalShape;

	//todo 倒圆角 目前构建会失败
	//// 缝合组合体为一个面
	//BRepBuilderAPI_Sewing aSewingTool;
	//aSewingTool.Init();
	//aSewingTool.Load(thePlate.shape);
	//aSewingTool.Perform();
	//TopoDS_Shape sewedShape = aSewingTool.SewedShape();
	//// 遍历面，得到Wire
	//TopExp_Explorer edgeExplorer(sewedShape, TopAbs_EDGE);
	//BRepBuilderAPI_MakeWire tempWire = BRepBuilderAPI_MakeWire();
	//while (edgeExplorer.More()) {
	//	tempWire.Add(TopoDS::Edge(edgeExplorer.Current()));
	//	edgeExplorer.Next();
	//}
	//// 执行倒圆角
	//BRepFilletAPI_MakeFillet2d filleter(TopoDS::Face(sewedShape));
	//BRepTools_WireExplorer wireExplorer(tempWire.Wire());
	//while (wireExplorer.More()) {
	//	TopoDS_Vertex vertex = wireExplorer.CurrentVertex();
	//	filleter.AddFillet(vertex, theFilletRadius);
	//	wireExplorer.Next();
	//}
	//if (filleter.IsDone()) {
	//	thePlate.shape = filleter.Shape();
	//}

	// 缝合组合体为一个面
	if (middleToDown) {
		// 执行倒圆角
		BRepFilletAPI_MakeFillet2d filleter(TopoDS::Face(thePlate.shape));
		BRepTools_WireExplorer wireExplorer(thePlate._unclosedWire.Wire());
		while (wireExplorer.More()) {
			TopoDS_Vertex vertex = wireExplorer.CurrentVertex();
			filleter.AddFillet(vertex, theFilletRadius);
			wireExplorer.Next();
		}
		if (filleter.IsDone()) {
			thePlate.shape = filleter.Shape();
		}
	}
	return thePlate;
}

// 在横板上开槽
BasePlate SlotBasePlate(BasePlate& theBasePlate, std::vector<VerticalPlate> middleDownPlates, std::vector<VerticalPlate> middleUpPlates) {
	double TOL = 1e-2;
	std::vector<gp_Pnt> cutPoints;

	//! test
	TopoDS_Compound test;
	BRep_Builder builder;
	builder.MakeCompound(test);
	for (size_t i = 0; i < middleDownPlates.size(); ++i) {
		auto downPlate = middleDownPlates[i];
		for (size_t j = 0; j < middleUpPlates.size(); ++j) {
			auto upPlate = middleUpPlates[j];
			double x1 = downPlate.start.X();
			double y1 = downPlate.start.Y();
			double x2 = downPlate.end.X();
			double y2 = downPlate.end.Y();
			if (x1 > x2) { std::swap(x1, x2); }
			if (y1 > y2) { std::swap(y1, y2); }
			double x3 = upPlate.start.X();
			double y3 = upPlate.start.Y();
			double x4 = upPlate.end.X();
			double y4 = upPlate.end.Y();
			double cx, cy;
			if (Get2DLineIntersection(x1, y1, x2, y2, x3, y3, x4, y4, cx, cy)) {
				if (x1 - TOL <= cx && cx <= x2 + TOL && y1 - TOL <= cy && cy <= y2 + TOL) {
					double slotLen = downPlate.slotLength / 2 + 1;
					gp_Pnt currentPnt = gp_Pnt(cx, cy, theBasePlate.Z - 1);

					gp_Vec v1 = gp_Vec(downPlate.pose.dir).Normalized();
					double l1 = upPlate.connectionThickness / 2;
					gp_Vec v2 = gp_Vec(upPlate.pose.dir).Normalized();
					double l2 = downPlate.connectionThickness / 2;
					gp_Pnt p1 = currentPnt;
					gp_Trsf T1;
					T1.SetTranslationPart(v1.Multiplied(l1).Added(v2.Multiplied(l2)));
					p1.Transform(T1);
					gp_Pnt p2 = currentPnt;
					gp_Trsf T2;
					T2.SetTranslationPart(v1.Multiplied(slotLen).Added(v2.Multiplied(l2)));
					p2.Transform(T2);
					gp_Pnt p3 = currentPnt;
					gp_Trsf T3;
					T3.SetTranslationPart(v1.Multiplied(slotLen).Added(v2.Multiplied(-l2)));
					p3.Transform(T3);
					gp_Pnt p4 = currentPnt;
					gp_Trsf T4;
					T4.SetTranslationPart(v1.Multiplied(l1).Added(v2.Multiplied(-l2)));
					p4.Transform(T4);
					gp_Pnt p5 = currentPnt;
					gp_Trsf T5;
					T5.SetTranslationPart(v1.Multiplied(l1).Added(v2.Multiplied(-slotLen)));
					p5.Transform(T5);
					gp_Pnt p6 = currentPnt;
					gp_Trsf T6;
					T6.SetTranslationPart(v1.Multiplied(-l1).Added(v2.Multiplied(-slotLen)));
					p6.Transform(T6);
					gp_Pnt p7 = currentPnt;
					gp_Trsf T7;
					T7.SetTranslationPart(v1.Multiplied(-l1).Added(v2.Multiplied(-l2)));
					p7.Transform(T7);
					gp_Pnt p8 = currentPnt;
					gp_Trsf T8;
					T8.SetTranslationPart(v1.Multiplied(-slotLen).Added(v2.Multiplied(-l2)));
					p8.Transform(T8);
					gp_Pnt p9 = currentPnt;
					gp_Trsf T9;
					T9.SetTranslationPart(v1.Multiplied(-slotLen).Added(v2.Multiplied(l2)));
					p9.Transform(T9);
					gp_Pnt p10 = currentPnt;
					gp_Trsf T10;
					T10.SetTranslationPart(v1.Multiplied(-l1).Added(v2.Multiplied(l2)));
					p10.Transform(T10);
					gp_Pnt p11 = currentPnt;
					gp_Trsf T11;
					T11.SetTranslationPart(v1.Multiplied(-l1).Added(v2.Multiplied(slotLen)));
					p11.Transform(T11);
					gp_Pnt p12 = currentPnt;
					gp_Trsf T12;
					T12.SetTranslationPart(v1.Multiplied(l1).Added(v2.Multiplied(slotLen)));
					p12.Transform(T12);
					std::vector<gp_Pnt> wirePnts = { p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12 };
					BRepBuilderAPI_MakeWire wireMaker = BRepBuilderAPI_MakeWire();
					gp_Pnt lastP = p12;
					for (size_t k = 0; k < wirePnts.size(); k++) {
						wireMaker.Add(BRepBuilderAPI_MakeEdge(lastP, wirePnts[k]));
						lastP = wirePnts[k];
					}
					BRepBuilderAPI_MakeFace faceMaker(wireMaker);
					BRepPrimAPI_MakePrism prismer(faceMaker, gp_Vec(0, 0, 2));
					theBasePlate.shape = BRepAlgoAPI_Cut(theBasePlate.shape, prismer).Shape();
				}
			}
		}
	}
	return theBasePlate;
}

// 构建数字 Face (默认在原点沿着XOZ平面正方向构建)
static TopoDS_Compound MakeNumber(TCollection_AsciiString numberString, double scaleRatio, double spacing) {
	std::map<int, std::vector<std::array<double, 2>>> numberMap;
	numberMap[0] = {
		{0.0, 0.0},
		{30.0, 0.0},
		{30.0, 29.0},
		{20.0, 29.0},
		{20.0, 10.0},
		{10.0, 10.0},
		{10.0, 50.0},
		{20.0, 50.0},
		{20.0, 31.0},
		{30.0, 31.0},
		{30.0, 60.0},
		{0.0, 60.0},
	};
	numberMap[1] = {
		{0.0, 0.0},
		{0.0, 10.0},
		{10.0, 10.0},
		{10.0, 50.0},
		{0.0, 50.0},
		{0.0, 60.0},
		{20.0, 60.0},
		{20.0, 10.0},
		{30.0, 10.0},
		{30.0, 0.0}
	};
	numberMap[2] = {
			{0.0, 0.0},
			{0.0, 10.0},
			{18.94, 50.0},
			{10.0, 50.0},
			{10.0, 40.0},
			{0.0, 40.0},
			{0.0, 60.0},
			{30.0, 60.0},
			{30.0, 50.0},
			{11.06, 10.0},
			{30.0, 10.0},
			{30.0, 0.0},
	};
	numberMap[3] = {
			{0.0, 0.0},
			{30.0, 0.0},
			{30.0, 60.0},
			{0.0, 60.0},
			{0.0, 50.0},
			{20.0, 50.0},
			{20.0, 35.0},
			{8.0, 35.0},
			{8.0, 25.0},
			{20.0, 25.0},
			{20.0, 10.0},
			{0.0, 10.0},
	};
	numberMap[4] = {
		{15.0, 0.0},
		{15.0, 10.0},
		{0.0, 10.0},
		{0.0, 20.0},
		{5.0, 60.0},
		{15.0, 60.0},
		{10.0, 20.0},
		{15.0, 20.0},
		{15.0, 30.0},
		{25.0, 30.0},
		{25.0, 20.0},
		{30.0, 20.0},
		{30.0, 10.0},
		{25.0, 10.0},
		{25.0, 0.0},
	};
	numberMap[5] = {
			{0.0, 0.0},
			{0.0, 10.0},
			{20.0, 10.0},
			{20.0, 25.0},
			{0.0, 25.0},
			{0.0, 60.0},
			{30.0, 60.0},
			{30.0, 50.0},
			{10.0, 50.0},
			{10.0, 35.0},
			{30.0, 35.0},
			{30.0, 0.0},
	};
	numberMap[6] = {
		{0.0, 0.0},
		{0.0, 60.0},
		{30.0, 60.0},
		{30.0, 50.0},
		{10.0, 50.0},
		{10.0, 10.0},
		{20.0, 10.0},
		{20.0, 20.0},
		{12.0, 20.0},
		{12.0, 30.0},
		{30.0, 30.0},
		{30.0, 0.0},
	};
	numberMap[7] = {
		{0.0, 0.0},
		{20.0, 50.0},
		{10.0, 50.0},
		{10.0, 40.0},
		{0.0, 40.0},
		{0.0, 60.0},
		{30.0, 60.0},
		{30.0, 50.0},
		{10.0, 0.0},
	};
	numberMap[8] = {
		{0.0, 0.0},
		{0.0, 35.0},
		{20.0, 35.0},
		{20.0, 50.0},
		{10.0, 50.0},
		{10.0, 38.0},
		{0.0, 38.0},
		{0.0, 60.0},
		{30.0, 60.0},
		{30.0, 25.0},
		{10.0, 25.0},
		{10.0, 10.0},
		{20.0, 10.0},
		{20.0, 22.0},
		{30.0, 22.0},
		{30.0, 0.0},
	};
	numberMap[9] = {
		{20.0, 0.0},
		{20.0, 50.0},
		{10.0, 50.0},
		{10.0, 40.0},
		{18.0, 40.0},
		{18.0, 30.0},
		{0.0, 30.0},
		{0.0, 60.0},
		{30.0, 60.0},
		{30.0, 0.0},
	};
	// X
	numberMap[10] = {
	{0.0, 0.0},
	{10.0, 0.0},
	{15.0, 25.0},
	{20.0, 0.0},
	{30.0, 0.0},
	{24.0, 30.0},
	{30.0, 60.0},
	{20.0, 60.0},
	{15.0, 35.0},
	{10.0, 60.0},
	{0.0, 60.0},
	{6.0, 30.0},
	};
	// Y
	numberMap[11] = {
		{10.0, 0.0},
		{20.0, 0.0},
		{20.0, 30.0},
		{30.0, 60.0},
		{20.0, 60.0},
		{15.0, 45.0},
		{10.0, 60.0},
		{0.0, 60.0},
		{10.0, 30.0},
	};
	// I
	numberMap[12] = {
		{0.0, 0.0},
		{30.0, 0.0},
		{30.0, 10.0},
		{20.0, 10.0},
		{20.0, 50.0},
		{30.0, 50.0},
		{30.0, 60.0},
		{0.0, 60.0},
		{0.0, 50.0},
		{10.0, 50.0},
		{10.0, 10.0},
		{0.0, 10.0},
	};

	TopoDS_Compound result;
	BRep_Builder builder;
	builder.MakeCompound(result);
	double offset = (spacing + 30.0) * scaleRatio;

	auto numbers = GetNumberString(numberString);
	for (size_t i = 0; i < numbers.size(); ++i) {
		int currentIndex = numbers[i];
		// 开始构建
		BRepBuilderAPI_MakeWire wireMaker;
		std::array<double, 3> lastValue{ 999.0,999.0,999.0 };
		for (std::array<double, 2> p : numberMap[currentIndex]) {
			if (std::all_of(lastValue.begin(), lastValue.end(), [](double v) { return v == 999.0; })) {
				lastValue[0] = p[0] + i * offset;
				lastValue[1] = 0.0;
				lastValue[2] = p[1];
				continue;
			}
			std::array<double, 3> value{ p[0] + i * offset , 0.0, p[1] };

			wireMaker.Add(BRepBuilderAPI_MakeEdge(gp_Pnt(lastValue[0] * scaleRatio, 0.0, lastValue[2] * scaleRatio), gp_Pnt(value[0] * scaleRatio, 0.0, value[2] * scaleRatio)));
			lastValue = value;
		}
		// 最后连接
		std::array<double, 3>end_value{ numberMap[currentIndex][0][0] + i * offset ,0.0,numberMap[currentIndex][0][1] };
		wireMaker.Add(BRepBuilderAPI_MakeEdge(gp_Pnt(lastValue[0] * scaleRatio, 0.0, lastValue[2] * scaleRatio), gp_Pnt(end_value[0] * scaleRatio, 0.0, end_value[2] * scaleRatio)));
		// 构造面
		BRepBuilderAPI_MakeFace faceMaker(wireMaker.Wire());
		builder.Add(result, faceMaker.Face());
	}
	return result;
}

// 在板上烙印数字（在左下角点，借助输入判断左下角）
VerticalPlate BrandNumber(VerticalPlate thePlate, double hight = 60.0) {
	if (thePlate.pieces.empty()) {
		return thePlate;
	}
	// 设置start为左边点
	gp_Trsf tempswapT;
	tempswapT.SetRotation(gp_Ax1(thePlate.pose.point, gp_Dir(0, 0, 1)), thePlate.pose.dir.AngleWithRef(gp_Vec(0.0, 1.0, 0.0), gp_Vec(0.0, 0.0, 1.0)));
	gp_Pnt tempStart = thePlate.start.Transformed(tempswapT);
	gp_Pnt tempEnd = thePlate.end.Transformed(tempswapT);
	if (tempStart.X() > tempEnd.X()) {
		std::swap(thePlate.start, thePlate.end);
	}

	TCollection_AsciiString numberString = thePlate.numberString;
	double scaleFactor = hight / 60.0;
	// todo 字符间的间距暂不开放
	double spacing = 10.0;
	TopoDS_Compound originFace = MakeNumber(numberString, 1.0, spacing);
	//往前(Y)挪1(为了烙印板)
	gp_Trsf temp;
	temp.SetTranslation(gp_Pnt(), gp_Pnt(0, -1, 0));
	BRepBuilderAPI_Transform theFace(originFace, temp);
	// 缩放
	gp_Trsf scaleT;
	scaleT.SetScaleFactor(scaleFactor);
	// 平移
	gp_Trsf transT;
	gp_Trsf tempT;
	tempT.SetTranslation(gp_Vec(thePlate.start, thePlate.end).Normalized().Multiplied(5.0));
	gp_Pnt aimPnt = thePlate.start.Transformed(tempT);
	aimPnt.SetZ(thePlate.Z + 5.0);
	transT.SetTranslation(gp_Pnt(), aimPnt);
	// 旋转
	gp_Trsf rotT;
	gp_Quaternion quat = gp_Quaternion(gp_Vec(0.0, 0.0, 1.0), gp_Vec(0.0, 1.0, 0.0).AngleWithRef(gp_Vec(thePlate.pose.dir), gp_Vec(0.0, 0.0, 1.0)));
	rotT.SetRotation(quat);
	// 往后(Y)走2，相当于前后各1
	BRepPrimAPI_MakePrism prismer(theFace, gp_Vec(0, 2, 0));

	BRepBuilderAPI_Transform transformedFace = BRepBuilderAPI_Transform(prismer.Shape(), transT.Multiplied(rotT.Multiplied(scaleT)));

	BRepAlgoAPI_Cut cutter(thePlate.shape, transformedFace.Shape());

	thePlate.shape = cutter.Shape();
	//! test
	//thePlate.shape = transformedFace.Shape();

	//TopoDS_Compound testShape;
	//BRep_Builder b;
	//b.MakeCompound(testShape);
	//b.Add(testShape, thePlate.shape);
	//b.Add(testShape, transformedFace);
	//thePlate.shape = testShape;

	return thePlate;
}

// 在板上烙印数字（在指定点）
VerticalPlate BrandNumber(VerticalPlate thePlate, gp_Pnt aimPoint, double hight = 60.0) {
	if (thePlate.pieces.empty()) {
		return thePlate;
	}
	// 设置start为左边点
	gp_Trsf tempswapT;
	tempswapT.SetRotation(gp_Ax1(thePlate.pose.point, gp_Dir(0, 0, 1)), thePlate.pose.dir.AngleWithRef(gp_Vec(0.0, 1.0, 0.0), gp_Vec(0.0, 0.0, 1.0)));
	gp_Pnt tempStart = thePlate.start.Transformed(tempswapT);
	gp_Pnt tempEnd = thePlate.end.Transformed(tempswapT);
	if (tempStart.X() > tempEnd.X()) {
		std::swap(thePlate.start, thePlate.end);
	}

	TCollection_AsciiString numberString = thePlate.numberString;
	VerticalPlate newPlate = thePlate;
	double scaleFactor = hight / 60.0;
	// todo 字符间的间距暂不开放
	double spacing = 10.0;
	TopoDS_Compound originFace = MakeNumber(numberString, 1.0, spacing);
	//往前(Y)挪1(为了烙印板)
	gp_Trsf temp;
	temp.SetTranslation(gp_Pnt(), gp_Pnt(0, -1, 0));
	BRepBuilderAPI_Transform theFace(originFace, temp);
	// 缩放
	gp_Trsf scaleT;
	scaleT.SetScaleFactor(scaleFactor);
	// 平移
	gp_Trsf transT;
	transT.SetTranslation(gp_Pnt(), aimPoint);
	// 旋转
	gp_Trsf rotT;
	gp_Quaternion quat = gp_Quaternion(gp_Vec(0.0, 0.0, 1.0), gp_Vec(0.0, 1.0, 0.0).AngleWithRef(gp_Vec(thePlate.pose.dir), gp_Vec(0.0, 0.0, 1.0)));
	rotT.SetRotation(quat);
	// 往后(Y)走2，相当于前后各1
	BRepPrimAPI_MakePrism prismer(theFace, gp_Vec(0, 2, 0));

	BRepBuilderAPI_Transform transformedFace = BRepBuilderAPI_Transform(prismer.Shape(), scaleT.Multiplied(transT.Multiplied(rotT)));

	BRepAlgoAPI_Cut cutter(thePlate.shape, transformedFace.Shape());

	thePlate.shape = cutter.Shape();

	return thePlate;
}

#pragma endregion

// 把所有板平铺
TopoDS_Shape DeployPlates(BasePlate theBasePlate, std::vector<VerticalPlate> middleToDownPlates, std::vector<VerticalPlate> middleToUpPlates) {
	TopoDS_Compound result;
	BRep_Builder builder;
	builder.MakeCompound(result);
	double gap = 10.0;
#pragma region 横板
	double X = theBasePlate.X;
	double Y = theBasePlate.Y;
	double Z = theBasePlate.Z;
	gp_Trsf base_trsf;
	base_trsf.SetTranslation(gp_Pnt(X - theBasePlate.offsetX, Y - theBasePlate.offsetY, Z), gp_Pnt());

	gp_Trsf baseR;
	gp_Quaternion baseQuat;
	baseQuat.SetEulerAngles(gp_Extrinsic_ZYX, 0.0 * (M_PI / 180.0), 0.0 * (M_PI / 180.0), 90.0 * (M_PI / 180.0));
	baseR.SetRotation(baseQuat);

	TopoDS_Shape newBP = theBasePlate.shape.Moved(TopLoc_Location(baseR.Multiplied(base_trsf)), true);
	builder.Add(result, newBP);

#pragma endregion

#pragma region 竖板

	double rightX = 0.0;
	double rightY = 0.0;
	// X 方向
	for (size_t i = 0; i < middleToDownPlates.size(); ++i) {
		VerticalPlate thePlate = middleToDownPlates[i];
		// 首先全部旋转到XOZ平面
		gp_Trsf toOriginR;
		toOriginR.SetRotation(gp_Ax1(thePlate.pose.point, gp_Dir(0, 0, 1)), thePlate.pose.dir.AngleWithRef(gp_Vec(0.0, 1.0, 0.0), gp_Vec(0.0, 0.0, 1.0)));
		BRepBuilderAPI_Transform toOriginRshape(thePlate.shape, toOriginR);
		// 平移到原点正上方
		gp_Pnt tempStart = thePlate.start.Transformed(toOriginR);
		gp_Trsf toOriginT;
		toOriginT.SetTranslation(tempStart, gp_Pnt(0.0, 0.0, theBasePlate.dY + theBasePlate.offsetY * 2 + thePlate.connectionThickness + gap));
		BRepBuilderAPI_Transform toOrigin(toOriginRshape, toOriginT);
		// 向右偏移
		Bnd_Box bbox;
		BRepBndLib::Add(toOrigin, bbox);
		double xMin, yMin, zMin, xMax, yMax, zMax;
		bbox.Get(xMin, yMin, zMin, xMax, yMax, zMax);

		gp_Trsf toRight;
		toRight.SetTranslation(gp_Vec(rightX, 0, 0));
		BRepBuilderAPI_Transform FinalShape(toOrigin, toRight);
		builder.Add(result, FinalShape);
		rightX += xMax - xMin + gap;
	}
	// Y 方向
	for (size_t i = 0; i < middleToUpPlates.size(); ++i) {
		VerticalPlate thePlate = middleToUpPlates[i];
		// 首先全部旋转到XOZ平面
		gp_Trsf toOriginR;
		toOriginR.SetRotation(gp_Ax1(thePlate.pose.point, gp_Dir(0, 0, 1)), thePlate.pose.dir.AngleWithRef(gp_Vec(0.0, 1.0, 0.0), gp_Vec(0.0, 0.0, 1.0)));
		BRepBuilderAPI_Transform toOriginRshape(thePlate.shape, toOriginR);
		// 平移到原点正上方
		gp_Pnt tempStart = thePlate.start.Transformed(toOriginR);
		gp_Trsf toOriginT;
		toOriginT.SetTranslation(tempStart, gp_Pnt(0, 0, theBasePlate.dY + theBasePlate.offsetY * 2 + thePlate.connectionThickness + gap));
		BRepBuilderAPI_Transform toOrigin(toOriginRshape, toOriginT);
		// 向上平移
		gp_Trsf upT;
		upT.SetTranslation(gp_Vec(0.0, 0.0, theBasePlate.hight + theBasePlate.offsetZ + gap));
		BRepBuilderAPI_Transform uped(toOrigin, upT);
		// 向右偏移
		Bnd_Box bbox;
		BRepBndLib::Add(uped, bbox);
		double xMin, yMin, zMin, xMax, yMax, zMax;
		bbox.Get(xMin, yMin, zMin, xMax, yMax, zMax);

		gp_Trsf toRight;
		toRight.SetTranslation(gp_Vec(rightY, 0, 0));
		BRepBuilderAPI_Transform FinalShape(uped, toRight);
		builder.Add(result, FinalShape);
		rightY += xMax - xMin + gap;
	}
#pragma endregion
	return result;
}

}
}