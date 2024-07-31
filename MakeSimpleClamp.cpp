#pragma once
#include "MakeSimpleClamp.h"
#include <algorithm>
#include <array>
#include <Bnd_Box.hxx>
#include <BRep_Builder.hxx>//test
#include <BRepAdaptor_Curve.hxx>
#include <BRepAlgoAPI_Cut.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepAlgoAPI_Section.hxx>
#include <BRepBndLib.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepLib.hxx>
#include <cmath>
#include <GC_MakeSegment.hxx>
#include <GCPnts_TangentialDeflection.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <gp_EulerSequence.hxx>
#include <gp_Quaternion.hxx>
#include <GProp_PEquation.hxx>
#include <IFSelect_ReturnStatus.hxx>
#include <Interface_Static.hxx>
#include <map>
#include <STEPCAFControl_Writer.hxx>
#include <STEPControl_StepModelType.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopExp_Explorer.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <utility>
#include <vector>
#include <BRepPrimAPI_MakePrism.hxx>
#include <queue>
#include <list>

static const double LINEAR_TOLERANCE = 1e-2;

namespace OCCTK {
namespace SimpleClamp {

#pragma region 静态方法函数

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

static std::vector<Ring> GetRings(const std::vector<myEdge>& Edges) {
	std::vector<Ring>Rings;
	Ring tempEdges = Edges;
	while (not tempEdges.empty()) {
		// 找出一个 Ring
		Ring aRing = FindARing(tempEdges);

		// 保存 Ring
		Rings.push_back(aRing);

	}
	return Rings;
}

static std::vector<myEdge> SplitRing(Ring theRing) {

	std::vector<myEdge> removedEdges;
	gp_Vec Z(0.0, 0.0, 1.0);
	// 去除垂直的边
	for (myEdge theEdge : theRing) {
		if (!gp_Vec(theEdge.start, theEdge.end).IsParallel(Z, 0.1)) {
			removedEdges.push_back(theEdge);
		}
	}

	// 错误处理，理论上 Ring 不应该少于4条边
	if (theRing.size() < 4) {
		return removedEdges;
	}
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
	for (auto oneEdge : removedEdges) {
		for (auto firstEdge : firstWire) {
			if (oneEdge == firstEdge) {
				break;
			}
			secondWire.push_back(oneEdge);
		}
	}
	// 比较高度
	//! 使用两端的 Z 值比较可能不准确（由于斜着的边缘）
	for (size_t i = 1; i < firstWire.size() - 1; ++i) {
		const myEdge& firstEdge = firstWire[i];
		bool isSame = false;
		for (size_t i = 0; i < secondWire.size(); ++i) {
			const myEdge& secondEdge = secondWire[i];
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

//static bool IsLine(Handle(Geom_Curve) theGeomCurve, double Tol) {
//	double distanceTol = 0.2;
//	const Standard_Integer numPoints = 10; // 减少点的数量以提高效率
//	TColgp_Array1OfPnt Pnts(1, numPoints);
//
//	double FirstParam = theGeomCurve->FirstParameter();
//	double LastParam = theGeomCurve->LastParameter();
//	double paramIncrement = (LastParam - FirstParam) / (numPoints - 1);
//
//	// 填充点数组
//	for (Standard_Integer i = 1; i <= numPoints; ++i) {
//		double Upara = FirstParam + paramIncrement * (i - 1);
//		Pnts(i) = theGeomCurve->Value(Upara);
//	}
//
//	// 计算曲线端点之间的距离
//	double PntDistance = Pnts(1).Distance(Pnts(numPoints));
//
//	// 如果端点之间的距离小于指定阈值，则认为是直线
//	if (PntDistance < distanceTol) {
//		return true;
//	}
//
//	// 使用拟合曲线的最大偏差来判断是否为直线
//	GProp_PEquation aGPE(Pnts, Tol);
//	return aGPE.IsLinear();
//}

//static std::vector<TopoDS_Edge> ConvertBSplineToLine(TopoDS_Edge theBSpline) {
//	std::vector<TopoDS_Edge> results;
//	double anADeflect = 0.25 * 3.1415; // 角度偏差 Angular deflection
//	double aCDeflect = 1e-1; // 曲率偏差 Curvature deflection
//	//得到 Geom_Curve
//	BRepAdaptor_Curve aBAC(theBSpline);
//	double first, last;
//	Handle(Geom_Curve) aCurve = BRep_Tool::Curve(theBSpline, first, last);
//	//离散为点
//	GCPnts_TangentialDeflection aPointsOnCurve;// 切矢量偏离算法
//	GeomAdaptor_Curve aCurveAdaptor(aCurve);
//	aPointsOnCurve.Initialize(aCurveAdaptor, anADeflect, aCDeflect);
//	for (int i = 1; i < aPointsOnCurve.NbPoints(); ++i) {
//		double firstP = aPointsOnCurve.Parameter(i);
//		double lastP = aPointsOnCurve.Parameter(i + 1);
//		Handle(Geom_TrimmedCurve) tricurve = new Geom_TrimmedCurve(aCurve, firstP, lastP);
//		if (IsLine(tricurve, 0.15)) {
//			TopoDS_Edge aEdge = BRepBuilderAPI_MakeEdge(aPointsOnCurve.Value(i), aPointsOnCurve.Value(i + 1));
//			results.push_back(aEdge);
//		}
//		//double aU = aPointsOnCurve.Parameter(i);
//		//gp_Pnt aPnt = aPointsOnCurve.Value(i);
//	}
//	return results;
//}

//todo 采用直接删去垂直线段（沿 Z 方向）的逻辑，如果有超过两段线是垂直的，将会出错。
//static std::vector<localEdge>GetConstructionLinesFromRing(Ring theRing) {
//	std::vector<localEdge> results;
//	gp_Dir theZDir = gp_Dir(0, 0, 1);
//	gp_Dir theMinusZDir = gp_Dir(0, 0, -1);
//	std::vector<localEdge> theEdges = theRing.Edges;
//	std::vector<localEdge> addedEdge;
//	//排除所有非直线线段
//	for (size_t i = 0; i < theEdges.size(); i++) {
//		BRepAdaptor_Curve aBAC(theEdges[i].edge);
//		// 曲线类型
//		auto theType = aBAC.GetType();
//		if (aBAC.GetType() != GeomAbs_Line) {
//			addedEdge.push_back(theEdges[i]);
//		}
//	}
//	for (localEdge anEdge : theEdges) {
//		//跳过已添加的线
//		if (std::find(addedEdge.begin(), addedEdge.end(), anEdge) != addedEdge.end())//在数组中
//		{
//			continue;
//		}
//		//排除垂直的线
//		gp_Dir edgeDir = gp_Dir(gp_Vec(anEdge.startPoint, anEdge.endPoint));
//		if (edgeDir.IsParallel(theZDir, 1e-2) || edgeDir.IsParallel(theMinusZDir, 1e-2)) {
//			addedEdge.push_back(anEdge);
//		}
//
//		//根据两端点找到较低的边
//		else {
//			addedEdge.push_back(anEdge);
//			gp_Pnt2d p1(anEdge.startPoint.X(), anEdge.startPoint.Y());
//			gp_Pnt2d p2(anEdge.endPoint.X(), anEdge.endPoint.Y());
//			for (localEdge otherEdge : theEdges) {
//				//跳过已添加的线
//				if (std::find(addedEdge.begin(), addedEdge.end(), otherEdge) != addedEdge.end())//在数组中
//				{
//					continue;
//				}
//				gp_Pnt2d p3(otherEdge.startPoint.X(), otherEdge.startPoint.Y());
//				gp_Pnt2d p4(otherEdge.endPoint.X(), otherEdge.endPoint.Y());
//				double distanceTol = 1;
//				if ((p1.Distance(p3) < distanceTol && p2.Distance(p4) < distanceTol) || (p1.Distance(p4) < distanceTol && p2.Distance(p3) < distanceTol)) {
//					gp_Pnt middlePoint1 = gp_Pnt((anEdge.startPoint.XYZ() + anEdge.endPoint.XYZ()) / 2);
//					gp_Pnt middlePoint2 = gp_Pnt((otherEdge.startPoint.XYZ() + otherEdge.endPoint.XYZ()) / 2);
//					if (middlePoint1.Z() < middlePoint2.Z()) {
//						results.push_back(anEdge);
//					}
//					else {
//						results.push_back(otherEdge);
//					}
//					//找到了对应的边，跳出循环
//					addedEdge.push_back(otherEdge);
//					break;
//				}
//			}
//		}
//	}
//	return results;
//}

// 构造连接板
//static TopoDS_Shape MakeConnectionPiece(double theX, double theY, double theZ, double theLength, double theHight, Direction theDirection) {
//	// 左上角点
//	gp_Pnt p1(theX, theY, theZ + theHight);
//	// 左下角点
//	gp_Pnt p2(theX, theY, theZ);
//	// 右下角点
//	gp_Pnt p3;
//	// 右上角点
//	gp_Pnt p4;
//	switch (theDirection)
//	{
//	case X:
//		p3 = gp_Pnt(theX, theY + theLength, theZ);
//		p4 = gp_Pnt(theX, theY + theLength, theZ + theHight);
//		break;
//	case Y:
//		p3 = gp_Pnt(theX + theLength, theY, theZ);
//		p4 = gp_Pnt(theX + theLength, theY, theZ + theHight);
//		break;
//	default:
//		break;
//	}
//	// 创建边
//	BRepBuilderAPI_MakeEdge l1(GC_MakeSegment(p1, p2).Value());
//	BRepBuilderAPI_MakeEdge l2(GC_MakeSegment(p2, p3).Value());
//	BRepBuilderAPI_MakeEdge l3(GC_MakeSegment(p3, p4).Value());
//	BRepBuilderAPI_MakeEdge l4(GC_MakeSegment(p4, p1).Value());
//	// 创建线段
//	BRepBuilderAPI_MakeWire wire;
//	wire.Add(l1);
//	wire.Add(l2);
//	wire.Add(l3);
//	wire.Add(l4);
//	// 创建面
//	BRepBuilderAPI_MakeFace face(wire.Wire());
//	return face.Shape();
//}

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
	BasePlate theBaseplate;
	theBaseplate.offsetX = theOffsetX;
	theBaseplate.offsetY = theOffsetY;
	GetCorners(theWorkpiece, theBaseplate);
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
	result = TrimEdgeEnds(result);
	result = RemoveShortEdge(result);
	result = CutLongEdge(result);

	return result;
}

// 合并片体为一块板
//void SuturePiece(const VerticalPlate& thePlate, BasePlate theBasePlate, double theConnectionHight) {
//	//! 板宽度不包含 Offset 的部分
//	double Z = theBasePlate.Z;
//	double ZC = theBasePlate.Z + theConnectionHight;
//	gp_Pnt start_p0, start_p1, start_p2, end_p0, end_p1;
//	gp_Pnt start_pnt, end_pnt;
//	BRepBuilderAPI_MakeEdge start_l0, start_l1, end_l0, end_l1;
//	std::vector<VerticalPiece> thePieces;
//	BRepBuilderAPI_MakeWire wire;
//	double theX = theBasePlate.X, theY = theBasePlate.Y;
//	// 线段生成逻辑见飞书
//	if (thePlate.dir == X)
//	{
//		/// 首先对输入的 Pieces 排序
//		for (VerticalPiece onePiece : thePlate.pieces) {
//			thePieces.push_back(onePiece);
//		}
//		// 使用 lambda 函数进行排序
//		std::sort(thePieces.begin(), thePieces.end(), [](const VerticalPiece& a, const VerticalPiece& b) {
//			return a.startPoint.Y() < b.startPoint.Y();
//			});
//		// 首段点
//		start_p0 = gp_Pnt(thePlate.location, theBasePlate.Y, Z);
//		start_p1 = gp_Pnt(thePlate.location, theBasePlate.Y, ZC);
//		// start_p2 为首个 Edge 的 start.X&Y
//		start_pnt = thePieces.front().startPoint;
//		start_p2 = gp_Pnt(thePlate.location, start_pnt.Y(), ZC);
//		// 末段点
//		// end_p0 为最后一个 Edge 的 End.X&Y
//		end_pnt = thePieces.back().endPoint;
//		end_p0 = gp_Pnt(thePlate.location, theBasePlate.Y + theBasePlate.dY, ZC);
//		end_p1 = gp_Pnt(thePlate.location, theBasePlate.Y + theBasePlate.dY, Z);
//
//		/// 首段
//		start_l0 = BRepBuilderAPI_MakeEdge(start_p0, start_p1);
//		start_l1 = BRepBuilderAPI_MakeEdge(start_p1, start_p2);
//		wire.Add(start_l0);
//		wire.Add(start_l1);
//
//		/// 中间段
//		for (size_t i = 0; i < thePieces.size(); i++)
//		{
//			VerticalPiece onePiece = thePieces[i];
//			gp_Pnt nextPnt;
//			if (i + 1 < thePieces.size()) {
//				nextPnt = thePieces[i + 1].startPoint;
//			}
//			else {
//				nextPnt = end_p0;
//			}
//			gp_Pnt p1, p2, p3, p4, p5;
//			p1 = gp_Pnt(thePlate.location, onePiece.startPoint.Y(), ZC);
//			p2 = onePiece.startPoint;
//			p3 = onePiece.endPoint;
//			//更新 theY
//			p4 = gp_Pnt(thePlate.location, p3.Y(), ZC);
//			p5 = gp_Pnt(thePlate.location, nextPnt.Y(), ZC);
//			// 创建边
//			BRepBuilderAPI_MakeEdge l1(p1, p2);
//			// l2 为 Edge
//			BRepBuilderAPI_MakeEdge l3(p3, p4);
//			BRepBuilderAPI_MakeEdge l4(p4, p5);
//			wire.Add(l1);
//			wire.Add(onePiece.edge);
//			wire.Add(l3);
//			wire.Add(l4);
//		}
//
//		/// 末段
//		end_l0 = BRepBuilderAPI_MakeEdge(end_p0, end_p1);
//		end_l1 = BRepBuilderAPI_MakeEdge(end_p1, start_p0);
//		wire.Add(end_l0);
//		wire.Add(end_l1);
//	}
//	if (thePlate.dir == Y)
//	{
//		/// 首先对输入的 Pieces 排序
//		for (VerticalPiece onePiece : thePlate.pieces) {
//			thePieces.push_back(onePiece);
//		}
//		// 使用 lambda 函数进行排序
//		std::sort(thePieces.begin(), thePieces.end(), [](const VerticalPiece& a, const VerticalPiece& b) {
//			return a.startPoint.X() < b.startPoint.X();
//			});
//		// 首段点
//		start_p0 = gp_Pnt(theBasePlate.X, thePlate.location, Z);
//		start_p1 = gp_Pnt(theBasePlate.X, thePlate.location, ZC);
//		// start_p2 为首个 Edge 的 start.X&Y
//		start_pnt = thePieces.front().startPoint;
//		start_p2 = gp_Pnt(start_pnt.X(), thePlate.location, ZC);
//		// 末段点
//		// end_p0 为最后一个 Edge 的 End.X&Y
//		end_pnt = thePieces.back().endPoint;
//		end_p0 = gp_Pnt(theBasePlate.X + theBasePlate.dX, thePlate.location, ZC);
//		end_p1 = gp_Pnt(theBasePlate.X + theBasePlate.dX, thePlate.location, Z);
//
//		/// 首段
//		start_l0 = BRepBuilderAPI_MakeEdge(start_p0, start_p1);
//		start_l1 = BRepBuilderAPI_MakeEdge(start_p1, start_p2);
//		wire.Add(start_l0);
//		wire.Add(start_l1);
//
//		/// 中间段
//		for (size_t i = 0; i < thePieces.size(); i++)
//		{
//			VerticalPiece onePiece = thePieces[i];
//			gp_Pnt nextPnt;
//			if (i + 1 < thePieces.size()) {
//				nextPnt = thePieces[i + 1].startPoint;
//			}
//			else {
//				nextPnt = end_p0;
//			}
//			gp_Pnt p1, p2, p3, p4, p5;
//			p1 = gp_Pnt(onePiece.startPoint.X(), thePlate.location, ZC);
//			p2 = onePiece.startPoint;
//			p3 = onePiece.endPoint;
//			//更新 theY
//			p4 = gp_Pnt(p3.X(), thePlate.location, ZC);
//			p5 = gp_Pnt(nextPnt.X(), thePlate.location, ZC);
//			// 创建边
//			BRepBuilderAPI_MakeEdge l1(p1, p2);
//			// l2 为 Edge
//			BRepBuilderAPI_MakeEdge l3(p3, p4);
//			BRepBuilderAPI_MakeEdge l4(p4, p5);
//			wire.Add(l1);
//			wire.Add(onePiece.edge);
//			wire.Add(l3);
//			wire.Add(l4);
//		}
//
//		/// 末段
//		end_l0 = BRepBuilderAPI_MakeEdge(end_p0, end_p1);
//		end_l1 = BRepBuilderAPI_MakeEdge(end_p1, start_p0);
//		wire.Add(end_l0);
//		wire.Add(end_l1);
//	}
//
//	// 创建面
//	BRepBuilderAPI_MakeFace face(wire.Wire());
//
//	thePlate.shape = face.Face();
//}

// 在竖板上切连接槽
//void Slotting(const VerticalPlate& thePlate, BasePlate theBasePlate, std::vector<double> theLocations, double theConnectionHight, double theConnectWidth, double theFilletRadius) {
//	double offset = 0.9;
//	if (thePlate.dir == X)
//	{
//		// 先在 Y 平面创建一个槽 (从下往上）
//		gp_Pnt p1(thePlate.location, -theConnectWidth / 2, theBasePlate.Z - 999);
//		gp_Pnt p3(thePlate.location, theConnectWidth / 2, theBasePlate.Z + (theConnectionHight / 2) * (2 - offset));
//		TopoDS_Face theSlot = MakePiece(p1, p3, Direction::X, theFilletRadius);
//
//		// 在每个位置切槽
//		for (double aloc : theLocations) {
//			gp_Trsf move;
//			move.SetTranslation(gp_Vec(0, aloc, 0)); // 沿着 Y
//			BRepBuilderAPI_Transform transform(theSlot, move);
//			transform.Build();
//			TopoDS_Shape localSlot = transform.Shape();
//			//更新 shape
//			thePlate.shape = BRepAlgoAPI_Cut(thePlate.shape, localSlot).Shape();
//		}
//	}
//	if (thePlate.dir == Y)
//	{
//		// 先在 X 平面创建一个槽 (从上往下）
//		gp_Pnt p1(-theConnectWidth / 2, thePlate.location, theBasePlate.Z + (theConnectionHight / 2) * offset);
//		gp_Pnt p3(theConnectWidth / 2, thePlate.location, theBasePlate.Z + 99999);
//		TopoDS_Face theSlot = MakePiece(p1, p3, Direction::Y, theFilletRadius);
//		// 在每个位置切槽
//		for (double aloc : theLocations) {
//			gp_Trsf move;
//			move.SetTranslation(gp_Vec(aloc, 0, 0)); // 沿着 X
//			BRepBuilderAPI_Transform transform(theSlot, move);
//			transform.Build();
//			TopoDS_Shape localSlot = transform.Shape();
//			//更新 shape
//			thePlate.shape = BRepAlgoAPI_Cut(thePlate.shape, localSlot).Shape();
//		}
//	}
//}

// 构建数字 Face
//static TopoDS_Face MakeNumber(int number, Direction dir) {
//	std::map<int, std::vector<std::array<double, 2>>> numberMap;
//	numberMap[1] = {
//		{0.0, 0.0},
//		{0.0, 10.0},
//		{10.0, 10.0},
//		{10.0, 50.0},
//		{0.0, 50.0},
//		{0.0, 60.0},
//		{20.0, 60.0},
//		{20.0, 10.0},
//		{30.0, 10.0},
//		{30.0, 0.0}
//	};
//	numberMap[2] = {
//			{0.0, 0.0},
//			{0.0, 10.0},
//			{18.94, 50.0},
//			{10.0, 50.0},
//			{10.0, 40.0},
//			{0.0, 40.0},
//			{0.0, 60.0},
//			{30.0, 60.0},
//			{30.0, 50.0},
//			{11.06, 10.0},
//			{30.0, 10.0},
//			{30.0, 0.0},
//	};
//	numberMap[3] = {
//			{0.0, 0.0},
//			{30.0, 0.0},
//			{30.0, 60.0},
//			{0.0, 60.0},
//			{0.0, 50.0},
//			{20.0, 50.0},
//			{20.0, 35.0},
//			{8.0, 35.0},
//			{8.0, 25.0},
//			{20.0, 25.0},
//			{20.0, 10.0},
//			{0.0, 10.0},
//	};
//	numberMap[4] = {
//		{15.0, 0.0},
//		{15.0, 10.0},
//		{0.0, 10.0},
//		{0.0, 20.0},
//		{5.0, 60.0},
//		{15.0, 60.0},
//		{10.0, 20.0},
//		{15.0, 20.0},
//		{15.0, 30.0},
//		{25.0, 30.0},
//		{25.0, 20.0},
//		{30.0, 20.0},
//		{30.0, 10.0},
//		{25.0, 10.0},
//		{25.0, 0.0},
//	};
//	numberMap[5] = {
//			{0.0, 0.0},
//			{0.0, 10.0},
//			{20.0, 10.0},
//			{20.0, 25.0},
//			{0.0, 25.0},
//			{0.0, 60.0},
//			{30.0, 60.0},
//			{30.0, 50.0},
//			{10.0, 50.0},
//			{10.0, 35.0},
//			{30.0, 35.0},
//			{30.0, 0.0},
//	};
//	numberMap[6] = {
//		{0.0, 0.0},
//		{0.0, 60.0},
//		{30.0, 60.0},
//		{30.0, 50.0},
//		{10.0, 50.0},
//		{10.0, 10.0},
//		{20.0, 10.0},
//		{20.0, 20.0},
//		{12.0, 20.0},
//		{12.0, 30.0},
//		{30.0, 30.0},
//		{30.0, 0.0},
//	};
//	numberMap[7] = {
//		{0.0, 0.0},
//		{20.0, 50.0},
//		{10.0, 50.0},
//		{10.0, 40.0},
//		{0.0, 40.0},
//		{0.0, 60.0},
//		{30.0, 60.0},
//		{30.0, 50.0},
//		{10.0, 0.0},
//	};
//	numberMap[8] = {
//		{0.0, 0.0},
//		{0.0, 35.0},
//		{20.0, 35.0},
//		{20.0, 50.0},
//		{10.0, 50.0},
//		{10.0, 38.0},
//		{0.0, 38.0},
//		{0.0, 60.0},
//		{30.0, 60.0},
//		{30.0, 25.0},
//		{10.0, 25.0},
//		{10.0, 10.0},
//		{20.0, 10.0},
//		{20.0, 22.0},
//		{30.0, 22.0},
//		{30.0, 0.0},
//	};
//	numberMap[9] = {
//		{20.0, 0.0},
//		{20.0, 50.0},
//		{10.0, 50.0},
//		{10.0, 40.0},
//		{18.0, 40.0},
//		{18.0, 30.0},
//		{0.0, 30.0},
//		{0.0, 60.0},
//		{30.0, 60.0},
//		{30.0, 0.0},
//	};
//	numberMap[0] = {
//		{0.0, 0.0},
//		{0.0, 60.0},
//		{30.0, 60.0},
//		{30.0, 0.0},
//		{15, 0.0},
//		{15, 10.0},
//		{20, 10.0},
//		{20, 50.0},
//		{10, 50.0},
//		{10, 10.0},
//		{14, 10.0},
//		{14, 0.0},
//	};
//
//	BRepBuilderAPI_MakeWire wireMaker;
//	std::array<double, 3> last_value{ 0.0,0.0,0.0 };
//	if (dir == Y)
//	{
//		for (std::array<double, 2> p : numberMap[number])
//		{
//			if (std::all_of(last_value.begin(), last_value.end(), [](double v) { return v == 0.0; }))
//			{
//				last_value[0] = p[0];
//				last_value[1] = 0.0;
//				last_value[2] = p[1];
//				continue;
//			}
//			std::array<double, 3> value{ p[0] , 0.0, p[1] };
//			wireMaker.Add(BRepBuilderAPI_MakeEdge(gp_Pnt(last_value[0], last_value[1], last_value[2]), gp_Pnt(value[0], value[1], value[2])));
//			last_value = value;
//		}
//		std::array<double, 3>end_value{ numberMap[number][0][0] ,0.0,numberMap[number][0][1] };
//		wireMaker.Add(BRepBuilderAPI_MakeEdge(gp_Pnt(last_value[0], last_value[1], last_value[2]), gp_Pnt(end_value[0], end_value[1], end_value[2])));
//		BRepBuilderAPI_MakeFace faceMaker(wireMaker.Wire());
//		return faceMaker.Face();
//	}
//	if (dir == X)
//	{
//		for (std::array<double, 2> p : numberMap[number])
//		{
//			if (std::all_of(last_value.begin(), last_value.end(), [](double v) { return v == 0.0; }))
//			{
//				last_value[0] = 0.0;
//				last_value[1] = p[0];
//				last_value[2] = p[1];
//				continue;
//			}
//			std::array<double, 3> value{ 0.0,p[0] , p[1] };
//			wireMaker.Add(BRepBuilderAPI_MakeEdge(gp_Pnt(last_value[0], last_value[1], last_value[2]), gp_Pnt(value[0], value[1], value[2])));
//			last_value = value;
//		}
//		std::array<double, 3>end_value{ 0.0, numberMap[number][0][0] , numberMap[number][0][1] };
//		wireMaker.Add(BRepBuilderAPI_MakeEdge(gp_Pnt(last_value[0], last_value[1], last_value[2]), gp_Pnt(end_value[0], end_value[1], end_value[2])));
//		BRepBuilderAPI_MakeFace faceMaker(wireMaker.Wire());
//		return faceMaker.Face();
//	}
//	throw std::runtime_error("Unexpected Direction");
//}

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

// 在板上烙印数字
//VerticalPlate BrandNumber(VerticalPlate thePlate, double hight, int number)
//{
//	VerticalPlate newPlate = thePlate;
//	double scaleFactor = hight / 60.0;
//	std::vector<int> theNumbers = GetDigitValues(number);
//	for (size_t i = 0; i < theNumbers.size(); i++)
//	{
//		gp_Trsf theTrsf;
//		gp_Trsf scaleTrsf;
//		scaleTrsf.SetScale(gp_Pnt(), scaleFactor);
//
//		if (thePlate.dir == X)
//		{
//			TopoDS_Face theFace = MakeNumber(theNumbers[i], thePlate.dir);
//			// 缩放
//			TopoDS_Shape scaledFace = BRepBuilderAPI_Transform(theFace, scaleTrsf).Shape();
//			// 移动到定位点
//			theTrsf.SetTranslation(gp_Vec(thePlate.location, thePlate.XY + (5.0 + i * 35.0) * scaleFactor, thePlate.Z + 5.0 * scaleFactor));
//			scaledFace.Move(TopLoc_Location(theTrsf));
//
//			BRepPrimAPI_MakePrism prismer(scaledFace, gp_Vec(99, 0, 0));
//			BRepAlgoAPI_Cut cutter(newPlate.shape, prismer.Shape());
//			//newPlate.shape = prismer.Shape();
//			newPlate.shape = cutter.Shape();
//		}
//		if (thePlate.dir == Y)
//		{
//			TopoDS_Face theFace = MakeNumber(theNumbers[i], thePlate.dir);
//			TopoDS_Shape scaledFace = BRepBuilderAPI_Transform(theFace, scaleTrsf).Shape();
//			theTrsf.SetTranslation(gp_Vec(thePlate.XY + (5.0 + i * 35.0) * scaleFactor, thePlate.location, thePlate.Z + 5.0 * scaleFactor));
//			scaledFace.Move(TopLoc_Location(theTrsf));
//
//			BRepPrimAPI_MakePrism prismer(scaledFace, gp_Vec(0, 99, 0));
//			BRepAlgoAPI_Cut cutter(newPlate.shape, prismer.Shape());
//			//newPlate.shape = prismer.Shape();
//			newPlate.shape = cutter.Shape();
//		}
//	}
//	return newPlate;
//}

// 在板上烙印数字，数字左下角位于 thePoint 的位置
//VerticalPlate BrandNumber(VerticalPlate thePlate, double hight, int number, gp_Pnt thePoint)
//{
//	VerticalPlate newPlate = thePlate;
//	double scaleFactor = hight / 60.0;
//	std::vector<int> theNumbers = GetDigitValues(number);
//	for (size_t i = 0; i < theNumbers.size(); i++)
//	{
//		gp_Trsf theTrsf;
//		gp_Trsf scaleTrsf;
//		scaleTrsf.SetScale(gp_Pnt(), scaleFactor);
//
//		if (thePlate.dir == X)
//		{
//			TopoDS_Face theFace = MakeNumber(theNumbers[i], thePlate.dir);
//			TopoDS_Shape scaledFace = BRepBuilderAPI_Transform(theFace, scaleTrsf).Shape();
//			gp_Pnt toPoint = thePoint;
//			toPoint.SetY(thePoint.Y() + i * 35.0 * scaleFactor);
//			gp_Vec theVec(gp_Pnt(), toPoint);
//			theVec.SetX(0);
//			theTrsf.SetTranslation(theVec);
//			scaledFace.Move(TopLoc_Location(theTrsf));
//
//			BRepPrimAPI_MakePrism prismer(scaledFace, gp_Vec(99, 0, 0));
//			BRepAlgoAPI_Cut cutter(newPlate.shape, prismer.Shape());
//			//newPlate.shape = prismer.Shape();
//			newPlate.shape = cutter.Shape();
//		}
//		if (thePlate.dir == Y)
//		{
//			TopoDS_Face theFace = MakeNumber(theNumbers[i], thePlate.dir);
//			TopoDS_Shape scaledFace = BRepBuilderAPI_Transform(theFace, scaleTrsf).Shape();
//			gp_Pnt toPoint = thePoint;
//			toPoint.SetX(thePoint.X() + i * 35.0 * scaleFactor);
//			gp_Vec theVec(gp_Pnt(), toPoint);
//			theVec.SetY(0);
//			theTrsf.SetTranslation(theVec);
//			scaledFace.Move(TopLoc_Location(theTrsf));
//
//			BRepPrimAPI_MakePrism prismer(scaledFace, gp_Vec(0, 99, 0));
//			BRepAlgoAPI_Cut cutter(newPlate.shape, prismer.Shape());
//			//newPlate.shape = prismer.Shape();
//			newPlate.shape = cutter.Shape();
//		}
//	}
//	return newPlate;
//}

#pragma endregion

// 把所有板平铺
//TopoDS_Shape DeployPlates(BasePlate theBasePlate, std::vector<VerticalPlate> theVerticalPlates)
//{
//	double X = theBasePlate.X;
//	double Y = theBasePlate.Y;
//	double Z = theBasePlate.Z;
//	double gap = 10.0;
//#pragma region 横板
//	gp_Trsf base_trsf = gp_Trsf();
//	base_trsf.SetTranslationPart(gp_Vec(-X + theBasePlate.offsetX + gap / 2, -Y + theBasePlate.offsetY + gap / 2, -Z));
//	TopoDS_Shape newBP = theBasePlate.shape.Moved(TopLoc_Location(base_trsf), true);
//#pragma endregion
//	double theX = 0.0;
//	double theY = 0.0;
//	std::vector<TopoDS_Shape> newVPs;
//	for (auto theVP : theVerticalPlates)
//	{
//		switch (theVP.dir)
//		{
//		case Direction::X:
//		{
//			theY += gap / 2;
//			gp_Trsf vp_toOrigin = gp_Trsf();
//			gp_Trsf vp_trsf = gp_Trsf();
//			vp_toOrigin.SetTranslationPart(gp_Vec(-theVP.location, -Y, -Z));
//			theVP.shape = theVP.shape.Located(TopLoc_Location(vp_toOrigin));
//			//! 注意，齐次变换是先旋转后平移的
//			gp_Quaternion vp_quat = gp_Quaternion();
//			vp_quat.SetEulerAngles(gp_Extrinsic_ZYX, 0, -M_PI / 2, 0);
//			vp_trsf.SetRotationPart(vp_quat);
//			vp_trsf.SetTranslationPart(gp_Vec(-gap / 2, theY, 0));
//			TopoDS_Shape newVP = theVP.shape.Moved(TopLoc_Location(vp_trsf), true);
//			newVPs.push_back(newVP);
//			theY += theBasePlate.dY + gap / 2;
//			break;
//		}
//		case Direction::Y:
//		{
//			theX += gap / 2;
//			gp_Trsf vp_toOrigin = gp_Trsf();
//			gp_Trsf vp_trsf = gp_Trsf();
//			vp_toOrigin.SetTranslationPart(gp_Vec(-X, -theVP.location, -Z));
//			theVP.shape = theVP.shape.Located(TopLoc_Location(vp_toOrigin));
//			//! 注意，齐次变换是先旋转后平移的
//			gp_Quaternion vp_quat = gp_Quaternion();
//			vp_quat.SetEulerAngles(gp_Extrinsic_ZYX, 0, 0, M_PI / 2);
//			vp_trsf.SetRotationPart(vp_quat);
//			vp_trsf.SetTranslationPart(gp_Vec(theX, -gap / 2, 0));
//			TopoDS_Shape newVP = theVP.shape.Moved(TopLoc_Location(vp_trsf), true);
//			newVPs.push_back(newVP);
//			theX += theBasePlate.dX + gap / 2;
//			break;
//		}
//		default:
//			break;
//		}
//	}
//	TopoDS_Compound result = TopoDS_Compound();
//	BRep_Builder builder = BRep_Builder();
//	builder.MakeCompound(result);
//	builder.Add(result, newBP);
//	for (auto anewVP : newVPs)
//	{
//		builder.Add(result, anewVP);
//	}
//	return result;
//}

}
}