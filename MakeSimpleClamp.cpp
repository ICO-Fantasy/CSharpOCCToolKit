#pragma once
#include "MakeSimpleClamp.h"
#include <algorithm>
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
#include <cmath>
#include <GC_MakeSegment.hxx>
#include <GCPnts_TangentialDeflection.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <gp_EulerSequence.hxx>
#include <gp_Quaternion.hxx>
#include <GProp_PEquation.hxx>
#include <IFSelect_ReturnStatus.hxx>
#include <STEPCAFControl_Writer.hxx>
#include <STEPControl_StepModelType.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <vector>
#include <Interface_Static.hxx>


namespace OCCTK {
namespace SimpleClamp {

#pragma region 方法函数

static void AddEdgeToRing(Ring& theRing, std::vector<localEdge>& Edges) {
	// 使用迭代器遍历 Edges
	for (auto it = Edges.begin(); it != Edges.end(); ++it) {
		gp_Pnt p1 = theRing.startPoint;
		gp_Pnt p2 = theRing.endPoint;
		gp_Pnt p3 = it->startPoint;
		gp_Pnt p4 = it->endPoint;
		if (theRing.endPoint.IsEqual(it->startPoint, 0.1)) {
			theRing.endPoint = it->endPoint;
			theRing.Edges.push_back(*it);
			Edges.erase(it); // 删除匹配的边，并更新迭代器
			AddEdgeToRing(theRing, Edges);
			break;
		}
		else if (theRing.endPoint.IsEqual(it->endPoint, 0.1)) {
			theRing.endPoint = it->startPoint;
			theRing.Edges.push_back(*it);
			Edges.erase(it); // 删除匹配的边，并更新迭代器
			AddEdgeToRing(theRing, Edges);
			break;
		}
		else if (theRing.startPoint.IsEqual(it->startPoint, 0.1)) {
			theRing.startPoint = it->endPoint;
			theRing.Edges.push_back(*it);
			Edges.erase(it); // 删除匹配的边，并更新迭代器
			AddEdgeToRing(theRing, Edges);
			break;
		}
		else if (theRing.startPoint.IsEqual(it->endPoint, 0.1)) {
			theRing.startPoint = it->startPoint;
			theRing.Edges.push_back(*it);
			Edges.erase(it); // 删除匹配的边，并更新迭代器
			AddEdgeToRing(theRing, Edges);
			break;
		}
	}
	return;
}
static std::vector<Ring> GetRings(std::vector<localEdge> Edges) {
	Ring aRing;
	std::vector<Ring>Rings;

	while (not Edges.empty()) {
		aRing.startPoint = Edges[0].startPoint;
		aRing.endPoint = Edges[0].endPoint;
		aRing.Edges.push_back(Edges[0]);
		Edges.erase(Edges.begin());
		AddEdgeToRing(aRing, Edges);
		Rings.push_back(aRing);
		//重新初始化ring
		aRing.Edges.clear();
	}
	return Rings;
}
static bool IsLine(Handle(Geom_Curve) theGeomCurve, double Tol) {
	double distanceTol = 0.2;
	const Standard_Integer numPoints = 10; // 减少点的数量以提高效率
	TColgp_Array1OfPnt Pnts(1, numPoints);

	double FirstParam = theGeomCurve->FirstParameter();
	double LastParam = theGeomCurve->LastParameter();
	double paramIncrement = (LastParam - FirstParam) / (numPoints - 1);

	// 填充点数组
	for (Standard_Integer i = 1; i <= numPoints; ++i) {
		double Upara = FirstParam + paramIncrement * (i - 1);
		Pnts(i) = theGeomCurve->Value(Upara);
	}

	// 计算曲线端点之间的距离
	double PntDistance = Pnts(1).Distance(Pnts(numPoints));

	// 如果端点之间的距离小于指定阈值，则认为是直线
	if (PntDistance < distanceTol) {
		return true;
	}

	// 使用拟合曲线的最大偏差来判断是否为直线
	GProp_PEquation aGPE(Pnts, Tol);
	return aGPE.IsLinear();
}
static std::vector<TopoDS_Edge> ConvertBSplineToLine(TopoDS_Edge theBSpline) {
	std::vector<TopoDS_Edge> results;
	double anADeflect = 0.25 * 3.1415; // 角度偏差Angular deflection
	double aCDeflect = 1e-1; // 曲率偏差Curvature deflection
	//得到Geom_Curve
	BRepAdaptor_Curve aBAC(theBSpline);
	double first, last;
	Handle(Geom_Curve) aCurve = BRep_Tool::Curve(theBSpline, first, last);
	//离散为点
	GCPnts_TangentialDeflection aPointsOnCurve;// 切矢量偏离算法
	GeomAdaptor_Curve aCurveAdaptor(aCurve);
	aPointsOnCurve.Initialize(aCurveAdaptor, anADeflect, aCDeflect);
	for (int i = 1; i < aPointsOnCurve.NbPoints(); ++i) {
		double firstP = aPointsOnCurve.Parameter(i);
		double lastP = aPointsOnCurve.Parameter(i + 1);
		Handle(Geom_TrimmedCurve) tricurve = new Geom_TrimmedCurve(aCurve, firstP, lastP);
		if (IsLine(tricurve, 0.15)) {
			TopoDS_Edge aEdge = BRepBuilderAPI_MakeEdge(aPointsOnCurve.Value(i), aPointsOnCurve.Value(i + 1));
			results.push_back(aEdge);
		}
		//double aU = aPointsOnCurve.Parameter(i);
		//gp_Pnt aPnt = aPointsOnCurve.Value(i);
	}
	return results;
}
static Ring ConvertRing(Ring theRing) {
	Ring newRing;
	for (auto aEdge : theRing.Edges) {
		BRepAdaptor_Curve aBAC(aEdge.Edge);
		// 曲线类型
		if (aBAC.GetType() == GeomAbs_BSplineCurve) {
			std::vector<TopoDS_Edge>newEdges = ConvertBSplineToLine(aEdge.Edge);
			for (auto newEdge : newEdges) {
				newRing.Edges.push_back(localEdge(newEdge));
			}
		}
		else {
			auto theType = aBAC.GetType();
			newRing.Edges.push_back(aEdge);
		}
	}
	return newRing;
}
//todo 采用直接删去垂直线段（沿Z方向）的逻辑，如果有超过两段线是垂直的，将会出错。
static std::vector<localEdge>GetConstructionLinesFromRing(Ring theRing) {
	std::vector<localEdge> results;
	gp_Dir theZDir = gp_Dir(0, 0, 1);
	gp_Dir theMinusZDir = gp_Dir(0, 0, -1);
	std::vector<localEdge> theEdges = theRing.Edges;
	std::vector<localEdge> addedEdge;
	//排除所有非直线线段
	for (size_t i = 0; i < theEdges.size(); i++) {
		BRepAdaptor_Curve aBAC(theEdges[i].Edge);
		// 曲线类型
		auto theType = aBAC.GetType();
		if (aBAC.GetType() != GeomAbs_Line) {
			addedEdge.push_back(theEdges[i]);
		}
	}
	for (localEdge anEdge : theEdges) {
		//跳过已添加的线
		if (std::find(addedEdge.begin(), addedEdge.end(), anEdge) != addedEdge.end())//在数组中
		{
			continue;
		}
		//排除垂直的线
		gp_Dir edgeDir = gp_Dir(gp_Vec(anEdge.startPoint, anEdge.endPoint));
		if (edgeDir.IsParallel(theZDir, 1e-2) || edgeDir.IsParallel(theMinusZDir, 1e-2)) {
			addedEdge.push_back(anEdge);
		}
		//根据长度找到另一个线（行不通）
		//else
		//{
		//	addedEdge.push_back(anEdge);
		//	double edgeLen = anEdge.startPoint.Distance(anEdge.endPoint);
		//	for (size_t j = 0; j < theEdges.size(); j++)
		//	{
		//		localEdge otherEdge = theEdges[j];
		//		if (std::find(addedEdge.begin(), addedEdge.end(), otherEdge) != addedEdge.end())//在数组中
		//		{
		//			continue;
		//		}
		//		double otherLen = otherEdge.startPoint.Distance(otherEdge.endPoint);
		//		if (std::fabs(edgeLen - otherLen) < 0.2)
		//		{
		//			gp_Pnt middlePoint1 = gp_Pnt((anEdge.startPoint.XYZ() + anEdge.endPoint.XYZ()) / 2);
		//			gp_Pnt middlePoint2 = gp_Pnt((otherEdge.startPoint.XYZ() + otherEdge.endPoint.XYZ()) / 2);
		//			if (middlePoint1.Z() < middlePoint2.Z())
		//			{
		//				results.push_back(anEdge);
		//			}
		//			else
		//			{
		//				results.push_back(otherEdge);
		//			}
		//			//找到了对应的边，跳出循环
		//			addedEdge.push_back(otherEdge);
		//			break;
		//		}
		//	}
		//}

		//根据两端点找到较低的边
		else {
			addedEdge.push_back(anEdge);
			gp_Pnt2d p1(anEdge.startPoint.X(), anEdge.startPoint.Y());
			gp_Pnt2d p2(anEdge.endPoint.X(), anEdge.endPoint.Y());
			for (localEdge otherEdge : theEdges) {
				//跳过已添加的线
				if (std::find(addedEdge.begin(), addedEdge.end(), otherEdge) != addedEdge.end())//在数组中
				{
					continue;
				}
				gp_Pnt2d p3(otherEdge.startPoint.X(), otherEdge.startPoint.Y());
				gp_Pnt2d p4(otherEdge.endPoint.X(), otherEdge.endPoint.Y());
				double distanceTol = 1;
				if ((p1.Distance(p3) < distanceTol && p2.Distance(p4) < distanceTol) || (p1.Distance(p4) < distanceTol && p2.Distance(p3) < distanceTol)) {
					gp_Pnt middlePoint1 = gp_Pnt((anEdge.startPoint.XYZ() + anEdge.endPoint.XYZ()) / 2);
					gp_Pnt middlePoint2 = gp_Pnt((otherEdge.startPoint.XYZ() + otherEdge.endPoint.XYZ()) / 2);
					if (middlePoint1.Z() < middlePoint2.Z()) {
						results.push_back(anEdge);
					}
					else {
						results.push_back(otherEdge);
					}
					//找到了对应的边，跳出循环
					addedEdge.push_back(otherEdge);
					break;
				}
			}
		}
	}
	return results;
}
static TopoDS_Shape MakeFaceFromLocalEdge(localEdge theEdge, double theZ) {
	gp_Pnt p1, p2, p3, p4;
	p1 = theEdge.startPoint;
	p2 = theEdge.endPoint;
	gp_Trsf t1 = gp_Trsf();
	t1.SetTranslationPart(gp_Vec(0, 0, -(theEdge.endPoint.Z() - theZ)));
	gp_Trsf t2 = gp_Trsf();
	t2.SetTranslationPart(gp_Vec(0, 0, -(theEdge.startPoint.Z() - theZ)));
	p3 = theEdge.endPoint.Transformed(t1);
	p4 = theEdge.startPoint.Transformed(t2);
	auto e1 = BRepBuilderAPI_MakeEdge(p1, p2).Edge();
	auto e2 = BRepBuilderAPI_MakeEdge(p2, p3).Edge();
	auto e3 = BRepBuilderAPI_MakeEdge(p3, p4).Edge();
	auto e4 = BRepBuilderAPI_MakeEdge(p4, p1).Edge();
	auto w = BRepBuilderAPI_MakeWire(e1, e2, e3, e4).Wire();
	auto result = BRepBuilderAPI_MakeFace(w).Shape();
	return result;
}
//todo 这个算法鲁棒性不高，需要修正
static std::vector<PolyLine> SplitRing(Ring theRing, Direction theDirection) {
	std::vector<localEdge>addedEdges;
	std::vector<localEdge> TempEdges;
	//!排除垂直的线
	gp_Dir theZDir = gp_Dir(0, 0, 1);
	gp_Dir theMinusZDir = gp_Dir(0, 0, -1);
	for (auto anEdge : theRing.Edges) {
		gp_Dir edgeDir = gp_Dir(gp_Vec(anEdge.startPoint, anEdge.endPoint));
		if (edgeDir.IsParallel(theZDir, 1e-1) || edgeDir.IsParallel(theMinusZDir, 1e-1)) {
			addedEdges.push_back(anEdge);
		}
		else {
			TempEdges.push_back(anEdge);
		}
	}
	//!找出下端线
	bool IsDone;
	int begin = 0;
	std::vector<PolyLine> Lines, addedLines, resultLines, removedLines;
	int N1;
	switch (theDirection) {
	case X:
		//首先对每个线段中的首末点排序
		for (auto& anEdge : TempEdges) {
			if (anEdge.startPoint.Y() > anEdge.endPoint.Y()) {
				std::swap(anEdge.startPoint, anEdge.endPoint);
			}
		}
		//按照 Y 值从小到大排序，使用 lambda 表达式
		std::sort(TempEdges.begin(), TempEdges.end(), [](const auto& a, const auto& b) {
			return a.middlePoint.Y() < b.middlePoint.Y();
			});
		//{//test
		//	PolyLine test;
		//	for (auto& anEdge : TempEdges) {
		//		test.Edges.push_back(anEdge);
		//	}
		//	resultLines.push_back(test);
		//	return resultLines;
		//}
#pragma region 找出所有段
		while (begin < TempEdges.size()) {
			//已添加
			if (std::find(addedEdges.begin(), addedEdges.end(), TempEdges[begin]) != addedEdges.end()) {
				begin++;
				continue;
			}
			addedEdges.push_back(TempEdges[begin]);

			PolyLine aLine;
			aLine.Edges.push_back(TempEdges[begin]);
			aLine.startPoint = TempEdges[begin].startPoint;
			aLine.endPoint = TempEdges[begin].endPoint;
			//从起始段开始，往后查找
			IsDone = true;
			while (IsDone) {
				bool continueF = false;
				for (localEdge anEdge : TempEdges) {
					if (std::find(addedEdges.begin(), addedEdges.end(), anEdge) != addedEdges.end()) {
						continue;
					}
					//拼接两点，并确保Y递增
					if (anEdge.startPoint.IsEqual(aLine.endPoint, 1e-2) && anEdge.endPoint.Y() > aLine.endPoint.Y()) {
						addedEdges.push_back(anEdge);
						//Edge加入line中，并更新末端点
						aLine.Edges.push_back(anEdge);
						aLine.endPoint = anEdge.endPoint;
						continueF = true;
						break;
					}
				}
				if (continueF) {
					continue;
				}
				IsDone = false;
				Lines.push_back(aLine);
			}
		}
#pragma endregion
		//todo比较逻辑需要重写
		//首先创建线段（上一步）
		//然后比较两线段是否重叠
		//如果重叠，取出两个线段的公共部分
		//比较公共部分的中心点Z值大小
		//抛弃Z大的那个（加入added）
		//继续比较，直到栈为空
#pragma region 比较逻辑
		N1 = 0;
		while (N1 < Lines.size()) {
			PolyLine L1 = Lines[N1];
			if (std::find(addedLines.begin(), addedLines.end(), L1) != addedLines.end()) {
				N1++;
				continue;
			}
			addedLines.push_back(L1);//待比较的项，先加入added
			int N2 = 0;
			while (N2 < Lines.size()) {
				PolyLine L2 = Lines[N2];
				if (std::find(addedLines.begin(), addedLines.end(), L2) != addedLines.end()) {
					N2++;
					continue;
				}
				// 比较L1和L2
				//如果区间重叠
				double p1 = L1.startPoint.Y(), p2 = L1.endPoint.Y(), p3 = L2.startPoint.Y(), p4 = L2.endPoint.Y();
				double middle = -999999.0;
				if (p1 <= p3 && p3 < p2) {
					middle = (p3 + p2) / 2;
				}
				else if (p3 <= p1 && p1 < p4) {
					auto middle = (p1 + p4) / 2;
				}
				else if (p3 <= p1 && p2 <= p4) {
					auto middle = (p1 + p2) / 2;
				}
				else if (p1 <= p3 && p4 <= p2) {
					auto middle = (p3 + p4) / 2;
				}
				if (middle != -999999.0) {//找到中点对应的边上对应的点
					double z1 = 0, z2 = 0;
					for (auto anEdge : L1.Edges) {
						if (anEdge.startPoint.Y() <= middle && middle <= anEdge.endPoint.Y()) {
							BRepAdaptor_Curve aBAC(anEdge.Edge);
							double ap1 = aBAC.FirstParameter();
							double ap2 = aBAC.LastParameter();
							if (ap1 > ap2) {
								std::swap(ap1, ap2);
							}
							z1 = aBAC.Value(ap1 + (ap2 - ap1) * (middle - anEdge.startPoint.Y()) / (anEdge.endPoint.Y() - anEdge.startPoint.Y())).Z();
							break;
						}
					}
					for (auto anEdge : L2.Edges) {
						if (anEdge.startPoint.Y() <= middle && middle <= anEdge.endPoint.Y()) {
							BRepAdaptor_Curve aBAC(anEdge.Edge);
							double ap1 = aBAC.FirstParameter();
							double ap2 = aBAC.LastParameter();
							if (ap1 > ap2) {
								std::swap(ap1, ap2);
							}
							z2 = aBAC.Value(ap1 + (ap2 - ap1) * (middle - anEdge.startPoint.Y()) / (anEdge.endPoint.Y() - anEdge.startPoint.Y())).Z();
							break;
						}
					}
					//比较边上对应的点的Z值大小，取小的线段
					if (z1 < z2) {
						removedLines.push_back(L2);
					}
					else {
						removedLines.push_back(L1);
					}
					addedLines.push_back(L2);
				}
				N2++;
			}
			N1++;

		}
		//最后把没被移除的加入结果
		for (auto theLine : Lines) {
			if (std::find(removedLines.begin(), removedLines.end(), theLine) == removedLines.end()) {
				resultLines.push_back(theLine);
			}
		}
#pragma endregion
		break;
	case Y:
		//首先对每个线段中的首末点排序
		for (auto& anEdge : TempEdges) {
			if (anEdge.startPoint.X() > anEdge.endPoint.X()) {
				std::swap(anEdge.startPoint, anEdge.endPoint);
			}
		}
		//按照 X 值从小到大排序，使用 lambda 表达式
		std::sort(TempEdges.begin(), TempEdges.end(), [](const auto& a, const auto& b) {
			return a.middlePoint.X() < b.middlePoint.X();
			});
#pragma region 找出所有段

		while (begin < TempEdges.size()) {
			//只有两条线的情况下不用找，直接比较
			if (TempEdges.size() <= 2) {
				// 逐个将TempEdges中的元素插入到Lines的末尾
				for (const auto& edge : TempEdges) {
					PolyLine tLine;
					tLine.Edges.push_back(edge);
					tLine.startPoint = edge.startPoint;
					tLine.endPoint = edge.endPoint;
					Lines.push_back(tLine);
				}
				break;
			}
			//已添加
			if (std::find(addedEdges.begin(), addedEdges.end(), TempEdges[begin]) != addedEdges.end()) {
				begin++;
				continue;
			}
			addedEdges.push_back(TempEdges[begin]);

			PolyLine aLine;
			aLine.Edges.push_back(TempEdges[begin]);
			aLine.startPoint = TempEdges[begin].startPoint;
			aLine.endPoint = TempEdges[begin].endPoint;
			//从起始段开始，往后查找
			IsDone = true;
			while (IsDone) {
				bool continueF = false;
				for (localEdge anEdge : TempEdges) {
					if (std::find(addedEdges.begin(), addedEdges.end(), anEdge) != addedEdges.end()) {
						continue;
					}
					//拼接两点，并确保Y递增
					if (anEdge.startPoint.IsEqual(aLine.endPoint, 1e-2) && anEdge.endPoint.X() > aLine.endPoint.X()) {
						addedEdges.push_back(anEdge);
						//Edge加入line中，并更新末端点
						aLine.Edges.push_back(anEdge);
						aLine.endPoint = anEdge.endPoint;
						continueF = true;
						break;
					}
				}
				if (continueF) {
					continue;
				}
				IsDone = false;
				Lines.push_back(aLine);
			}
		}
#pragma endregion
		//todo比较逻辑需要重写
		//首先创建线段（上一步）
		//然后比较两线段是否重叠
		//如果重叠，取出两个线段的公共部分
		//比较公共部分的中心点Z值大小
		//抛弃Z大的那个（加入added）
		//继续比较，直到栈为空
#pragma region 比较逻辑
		N1 = 0;
		while (N1 < Lines.size()) {
			PolyLine L1 = Lines[N1];
			if (std::find(addedLines.begin(), addedLines.end(), L1) != addedLines.end()) {
				N1++;
				continue;
			}
			addedLines.push_back(L1);//待比较的项，先加入added
			int N2 = 0;
			while (N2 < Lines.size()) {
				PolyLine L2 = Lines[N2];
				if (std::find(addedLines.begin(), addedLines.end(), L2) != addedLines.end()) {
					N2++;
					continue;
				}
				// 比较L1和L2
				//如果区间重叠
				double p1 = L1.startPoint.X(), p2 = L1.endPoint.X(), p3 = L2.startPoint.X(), p4 = L2.endPoint.X();
				double middle = -999999.0;
				if (p1 <= p3 && p3 < p2) {
					middle = (p3 + p2) / 2;
				}
				else if (p3 <= p1 && p1 < p4) {
					auto middle = (p1 + p4) / 2;
				}
				else if (p3 <= p1 && p2 <= p4) {
					auto middle = (p1 + p2) / 2;
				}
				else if (p1 <= p3 && p4 <= p2) {
					auto middle = (p3 + p4) / 2;
				}
				if (middle != -999999.0) {//找到中点对应的边上对应的点
					double z1 = 0, z2 = 0;
					for (auto anEdge : L1.Edges) {
						if (anEdge.startPoint.X() <= middle && middle <= anEdge.endPoint.X()) {
							BRepAdaptor_Curve aBAC(anEdge.Edge);
							double ap1 = aBAC.FirstParameter();
							double ap2 = aBAC.LastParameter();
							if (ap1 > ap2) {
								std::swap(ap1, ap2);
							}
							z1 = aBAC.Value(ap1 + (ap2 - ap1) * (middle - anEdge.startPoint.X()) / (anEdge.endPoint.X() - anEdge.startPoint.X())).Z();
							break;
						}
					}
					for (auto anEdge : L2.Edges) {
						if (anEdge.startPoint.X() <= middle && middle <= anEdge.endPoint.X()) {
							BRepAdaptor_Curve aBAC(anEdge.Edge);
							double ap1 = aBAC.FirstParameter();
							double ap2 = aBAC.LastParameter();
							if (ap1 > ap2) {
								std::swap(ap1, ap2);
							}
							z2 = aBAC.Value(ap1 + (ap2 - ap1) * (middle - anEdge.startPoint.X()) / (anEdge.endPoint.X() - anEdge.startPoint.X())).Z();
							break;
						}
					}
					//比较边上对应的点的Z值大小，取小的线段
					if (z1 < z2) {
						removedLines.push_back(L2);
					}
					else {
						removedLines.push_back(L1);
					}
					addedLines.push_back(L2);
				}
				N2++;
			}
			N1++;

		}
		//最后把没被移除的加入结果
		for (auto theLine : Lines) {
			if (std::find(removedLines.begin(), removedLines.end(), theLine) == removedLines.end()) {
				resultLines.push_back(theLine);
			}
		}
#pragma endregion
		break;
	default:
		//todo错误处理
		break;
	}
	return resultLines;
}
static TopoDS_Shape MakeConnectionPiece(double theX, double theY, double theZ, double theLength, double theHight, Direction theDirection) {
	// 左上角点
	gp_Pnt p1(theX, theY, theZ + theHight);
	// 左下角点
	gp_Pnt p2(theX, theY, theZ);
	// 右下角点
	gp_Pnt p3;
	// 右上角点
	gp_Pnt p4;
	switch (theDirection)
	{
	case X:
		p3 = gp_Pnt(theX, theY + theLength, theZ);
		p4 = gp_Pnt(theX, theY + theLength, theZ + theHight);
		break;
	case Y:
		p3 = gp_Pnt(theX + theLength, theY, theZ);
		p4 = gp_Pnt(theX + theLength, theY, theZ + theHight);
		break;
	default:
		break;
	}
	// 创建边
	BRepBuilderAPI_MakeEdge l1(GC_MakeSegment(p1, p2).Value());
	BRepBuilderAPI_MakeEdge l2(GC_MakeSegment(p2, p3).Value());
	BRepBuilderAPI_MakeEdge l3(GC_MakeSegment(p3, p4).Value());
	BRepBuilderAPI_MakeEdge l4(GC_MakeSegment(p4, p1).Value());
	// 创建线段
	BRepBuilderAPI_MakeWire wire;
	wire.Add(l1.Edge());
	wire.Add(l2.Edge());
	wire.Add(l3.Edge());
	wire.Add(l4.Edge());
	// 创建面
	BRepBuilderAPI_MakeFace face(wire.Wire());
	return face.Shape();
}

#pragma endregion

#pragma region 生成底板

/// <summary>
/// 根据工件的AABB包围盒得到两个角点（Z值为0）
/// </summary>
/// <param name="theShape"></param>
/// <returns></returns>
static void GetCorners(TopoDS_Shape theShape, BasePlate& theBasePlate) {
	Bnd_Box bbox;
	BRepBndLib::Add(theShape, bbox);
	double xmin, ymin, zmin, xmax, ymax, zmax;
	bbox.Get(xmin, ymin, zmin, xmax, ymax, zmax);
	theBasePlate.X = xmin;
	theBasePlate.Y = ymin;
	theBasePlate.Z = zmin;
	theBasePlate.dX = xmax - xmin;
	theBasePlate.dY = ymax - ymin;
}
/// <summary>
/// 根据两个角点和Z值创建底板
/// </summary>
/// <param name="theShape"></param>
/// <param name="theZ"></param>
/// <returns></returns>
static void MakeBasePlateShape(BasePlate& theBaseplate, double theZ) {
	// 根据边界框尺寸创建立方体
	gp_Pnt theLowLeft(theBaseplate.X - theBaseplate.OffsetX, theBaseplate.Y - theBaseplate.OffsetY, theZ);
	gp_Pnt theTopRight(theBaseplate.X + theBaseplate.dX + theBaseplate.OffsetX, theBaseplate.Y + theBaseplate.dY + theBaseplate.OffsetY, theZ);
	theBaseplate.shape = MakePiece(theLowLeft, theTopRight, Direction::Z);
}
/// <summary>
/// 非交互的生成底板
/// </summary>
/// <param name="theWorkpiece"></param>
/// <param name="theOffsetZ"></param>
/// <param name="theOffsetX"></param>
/// <param name="theOffsetY"></param>
/// <returns></returns>
BasePlate MakeBasePlate(TopoDS_Shape theWorkpiece, double theOffsetZ, double theOffsetX, double theOffsetY) {
	BasePlate theBaseplate;
	theBaseplate.OffsetX = theOffsetX;
	theBaseplate.OffsetY = theOffsetY;
	GetCorners(theWorkpiece, theBaseplate);
	theBaseplate.Z -= theOffsetZ;
	MakeBasePlateShape(theBaseplate, theBaseplate.Z);
	return theBaseplate;
}

#pragma endregion

#pragma region 生成竖板

/// <summary>
/// 创建拓扑平面
/// </summary>
/// <returns></returns>
static TopoDS_Face MakeCrossPlane(double thePointStart, Direction theDirection) {
	gp_Dir PlaneNormal;
	gp_Pnt startPnt;
	switch (theDirection) {
	case X:
		PlaneNormal = gp_Dir(1.0, 0.0, 0.0);
		startPnt = gp_Pnt(thePointStart, 0.0, 0);
		break;
	case Y:
		PlaneNormal = gp_Dir(0.0, 1.0, 0.0);
		startPnt = gp_Pnt(0.0, thePointStart, 0);
		break;
	default:
		throw std::invalid_argument("Invalid direction. Only X or Y are supported.");
		break;
	}
	gp_Pln thePln(startPnt, PlaneNormal);
	TopoDS_Face thePlate = BRepBuilderAPI_MakeFace(thePln).Face();
	return thePlate;
}

/// <summary>
/// 得到截平面
/// </summary>
/// <param name="theFace"></param>
/// <param name="theShape"></param>
/// <returns></returns>
static TopoDS_Shape MakeSection(TopoDS_Face theFace, TopoDS_Shape theShape) {
	// Computes Shape / Plane intersection;
	BRepAlgoAPI_Section aSection = BRepAlgoAPI_Section(theShape, theFace);
	aSection.Build();
	if (aSection.IsDone()) {
		TopoDS_Shape sec_shape = aSection.Shape();
		return sec_shape;
	}
	throw std::runtime_error("截面获取失败");
}

/// <summary>
/// 从截平面得到单块板
/// </summary>
/// <param name="theSection"></param>
/// <param name="theDirection"></param>
/// <returns></returns>
static std::vector<VerticalPiece> MakeVerticalPieceWithSection(TopoDS_Shape theSection, Direction theDirection) {
	std::vector<VerticalPiece>result;
#pragma region 得到原始构造线
	//!首先判断有几个环，对环进行处理
	std::vector<localEdge>TempEdges;
	std::vector<Ring>Rings;
	TopExp_Explorer aEdgeExp = TopExp_Explorer(theSection, TopAbs_EDGE);
	while (aEdgeExp.More()) {
		localEdge anEdge(TopoDS::Edge(aEdgeExp.Current()));// 遍历得到每一个边
		aEdgeExp.Next();
		TempEdges.push_back(anEdge);
	}
	Rings = GetRings(TempEdges);
	//! 每个环中取最下边的线段作为原始构造线
	std::vector<localEdge>OriginEdges;
	for (auto aRing : Rings) {
		std::vector <PolyLine> SplitedPolyLine = SplitRing(aRing, theDirection);
		for (auto PL : SplitedPolyLine) {
			for (auto LE : PL.Edges) {
				OriginEdges.push_back(LE);
			}
		}
	}
#pragma endregion
#pragma region 输出Pieces
	for (auto anEdge : OriginEdges) {
		VerticalPiece anPiece;
		anPiece.dir = theDirection;
		anPiece.Edge = anEdge.Edge;
		result.push_back(anPiece);
	}
	return result;
#pragma endregion
}

/// <summary>
/// 更新Piece从线得到板的形状
/// </summary>
/// <param name="thePiece"></param>
/// <param name="theZ"></param>
static void MakePieceFromEdge(VerticalPiece& thePiece, double theZ) {
	gp_Pnt p1 = thePiece.StartPoint();
	gp_Pnt p2 = thePiece.EndPoint();
	gp_Pnt p3(p2.X(), p2.Y(), theZ);
	gp_Pnt p4(p1.X(), p1.Y(), theZ);
	//todo没有检查GC_MakeSegment是否构造成功
	//Handle(Geom_TrimmedCurve) L1 = GC_MakeSegment(p1, p2);
	Handle(Geom_TrimmedCurve) L2 = GC_MakeSegment(p2, p3);
	Handle(Geom_TrimmedCurve) L3 = GC_MakeSegment(p3, p4);
	Handle(Geom_TrimmedCurve) L4 = GC_MakeSegment(p4, p1);
	BRepBuilderAPI_MakeWire aWireMk = BRepBuilderAPI_MakeWire();
	aWireMk.Add(thePiece.Edge);
	aWireMk.Add(BRepBuilderAPI_MakeEdge(L2));
	aWireMk.Add(BRepBuilderAPI_MakeEdge(L3));
	aWireMk.Add(BRepBuilderAPI_MakeEdge(L4));
	TopoDS_Face myFaceProfile = BRepBuilderAPI_MakeFace(aWireMk.Wire());
	thePiece.Shape = myFaceProfile;
}

/// <summary>
/// 移除长度过短的边
/// </summary>
/// <param name="thePieces"></param>
/// <param name="theMinSupportingLen"></param>
/// <returns></returns>
static std::vector<VerticalPiece> RemoveShortEdge(std::vector<VerticalPiece> thePieces, double theMinSupportingLen) {
	std::vector<VerticalPiece> result;
	for (auto aPiece : thePieces) {
		if (aPiece.Length() > theMinSupportingLen) {
			result.push_back(aPiece);
		}
	}
	return result;
}

/// <summary>
/// 修剪边的两端
/// </summary>
/// <param name="theOriginEdge"></param>
/// <param name="p1"></param>
/// <param name="p2"></param>
/// <returns></returns>
static TopoDS_Edge TrimEdge(TopoDS_Edge theOriginEdge, gp_Pnt p1, gp_Pnt p2) {
	//获取底层曲线
	double first, last;
	Handle(Geom_Curve) aCurve = BRep_Tool::Curve(theOriginEdge, first, last);
	//有时底层曲线没有创建，要手动创建
	//if (aCurve.IsNull())
	//{
	//	BRepLib::BuildCurves3d(theOriginEdge, 1.0e-5, GeomAbs_C1);//创建曲线(一阶导数的连续性)
	//	aCurve = BRep_Tool::Curve(theOriginEdge, first, last);
	//}
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

static void TrimEdgeEnds(std::vector<VerticalPiece>& thePieces, double theClearances) {
	for (VerticalPiece& aPiece : thePieces) {
		BRepAdaptor_Curve aBAC(aPiece.Edge);
		gp_Pnt p1 = aBAC.Value(aBAC.FirstParameter());
		gp_Pnt p2 = aBAC.Value(aBAC.LastParameter());
		switch (aPiece.dir) {
		case X:
		{
			if (p1.Y() > p2.Y()) {
				std::swap(p1, p2);
			}
			p1.SetY(p1.Y() + theClearances);
			p2.SetY(p2.Y() - theClearances);
			TopoDS_Edge outEdge = TrimEdge(aPiece.Edge, p1, p2);
			if (!outEdge.IsNull()) {
				aPiece.Edge = outEdge;
			}
			break;
		}
		case Y:
		{
			if (p1.X() > p2.X()) {
				std::swap(p1, p2);
			}
			p1.SetX(p1.X() + theClearances);
			p2.SetX(p2.X() - theClearances);
			TopoDS_Edge outEdge = TrimEdge(aPiece.Edge, p1, p2);
			if (!outEdge.IsNull()) {
				aPiece.Edge = outEdge;
			}
			break;
		}
		default:
			break;
		}
	}
}

/// <summary>
/// 切开过长的边，与修剪两端类似的做法
/// </summary>
/// <param name="thePieces"></param>
/// <param name="theDirection"></param>
/// <param name="VerticalPlateClearances"></param>
static std::vector<VerticalPiece> CutEdgeUniform(VerticalPiece thePiece, double theSupportLen, int theNum) {
	std::vector<VerticalPiece> result;
	//如果最小支撑长度不足，则不做切割
	if (theSupportLen <= 1)
	{
		result.push_back(thePiece);
		return result;
	}
	//获取底层曲线
	double first, last, left, right;
	Handle(Geom_Curve) aCurve = BRep_Tool::Curve(thePiece.Edge, first, last);
	double increment = thePiece.Length() / theNum;
	double start = 0, end = 0;
	gp_Pnt startP = thePiece.StartPoint(), endP = thePiece.StartPoint();
	double param1, param2;
	switch (thePiece.dir) {
	case X:
	{
		// 首段直接生成
		start = thePiece.StartPoint().Y();
		end = thePiece.StartPoint().Y() + theSupportLen;
		//startP
		endP.SetY(end);
		//投影点到曲线上，并获取投影点处的参数
		GeomAPI_ProjectPointOnCurve ppc1S(startP, aCurve);
		param1 = ppc1S.LowerDistanceParameter();
		GeomAPI_ProjectPointOnCurve ppc2S(endP, aCurve);
		param2 = ppc2S.LowerDistanceParameter();
		VerticalPiece newPieceStart;
		newPieceStart.dir = thePiece.dir;
		TopoDS_Edge newEdgeStart = BRepBuilderAPI_MakeEdge(aCurve, param1, param2);
		newEdgeStart.Orientation(thePiece.Edge.Orientation());//同向
		newPieceStart.Edge = newEdgeStart;
		result.push_back(newPieceStart);

		// 中间段从中间往两边生成
		for (size_t i = 0; i < theNum - 1; i++) {
			start += increment;
			left = start - theSupportLen / 2;
			right = start + theSupportLen / 2;

			startP.SetY(left);
			endP.SetY(right);
			//投影点到曲线上，并获取投影点处的参数
			GeomAPI_ProjectPointOnCurve ppc1(startP, aCurve);
			param1 = ppc1.LowerDistanceParameter();
			GeomAPI_ProjectPointOnCurve ppc2(endP, aCurve);
			param2 = ppc2.LowerDistanceParameter();
			VerticalPiece newPiece;
			newPiece.dir = thePiece.dir;
			TopoDS_Edge newEdge = BRepBuilderAPI_MakeEdge(aCurve, param1, param2);
			newEdge.Orientation(thePiece.Edge.Orientation());//同向
			newPiece.Edge = newEdge;
			result.push_back(newPiece);
		}
		// 尾端直接生成
		start = thePiece.EndPoint().Y() - theSupportLen;
		//end
		startP.SetY(start);
		endP = thePiece.EndPoint();
		//投影点到曲线上，并获取投影点处的参数
		GeomAPI_ProjectPointOnCurve ppc1E(startP, aCurve);
		param1 = ppc1E.LowerDistanceParameter();
		GeomAPI_ProjectPointOnCurve ppc2E(endP, aCurve);
		param2 = ppc2E.LowerDistanceParameter();
		VerticalPiece newPieceEnd;
		newPieceEnd.dir = thePiece.dir;
		TopoDS_Edge newEdgeEnd = BRepBuilderAPI_MakeEdge(aCurve, param1, param2);
		newEdgeEnd.Orientation(thePiece.Edge.Orientation());//同向
		newPieceEnd.Edge = newEdgeEnd;
		result.push_back(newPieceEnd);
		break;
	}
	case Y:
	{
		// 首段直接生成
		start = thePiece.StartPoint().X();
		end = thePiece.StartPoint().X() + theSupportLen;
		//startP
		endP.SetX(end);
		//投影点到曲线上，并获取投影点处的参数
		GeomAPI_ProjectPointOnCurve ppc1S(startP, aCurve);
		param1 = ppc1S.LowerDistanceParameter();
		GeomAPI_ProjectPointOnCurve ppc2S(endP, aCurve);
		param2 = ppc2S.LowerDistanceParameter();
		VerticalPiece newPieceStart;
		newPieceStart.dir = thePiece.dir;
		TopoDS_Edge newEdgeStart = BRepBuilderAPI_MakeEdge(aCurve, param1, param2);
		newEdgeStart.Orientation(thePiece.Edge.Orientation());//同向
		newPieceStart.Edge = newEdgeStart;
		result.push_back(newPieceStart);

		// 中间段从中间往两边生成
		for (size_t i = 0; i < theNum - 1; i++) {
			start += increment;
			left = start - theSupportLen / 2;
			right = start + theSupportLen / 2;

			startP.SetX(left);
			endP.SetX(right);
			//投影点到曲线上，并获取投影点处的参数
			GeomAPI_ProjectPointOnCurve ppc1(startP, aCurve);
			param1 = ppc1.LowerDistanceParameter();
			GeomAPI_ProjectPointOnCurve ppc2(endP, aCurve);
			param2 = ppc2.LowerDistanceParameter();
			VerticalPiece newPiece;
			newPiece.dir = thePiece.dir;
			TopoDS_Edge newEdge = BRepBuilderAPI_MakeEdge(aCurve, param1, param2);
			newEdge.Orientation(thePiece.Edge.Orientation());//同向
			newPiece.Edge = newEdge;
			result.push_back(newPiece);
		}
		// 尾端直接生成
		start = thePiece.EndPoint().X() - theSupportLen;
		//end
		startP.SetX(start);
		endP = thePiece.EndPoint();
		//投影点到曲线上，并获取投影点处的参数
		GeomAPI_ProjectPointOnCurve ppc1E(startP, aCurve);
		param1 = ppc1E.LowerDistanceParameter();
		GeomAPI_ProjectPointOnCurve ppc2E(endP, aCurve);
		param2 = ppc2E.LowerDistanceParameter();
		VerticalPiece newPieceEnd;
		newPieceEnd.dir = thePiece.dir;
		TopoDS_Edge newEdgeEnd = BRepBuilderAPI_MakeEdge(aCurve, param1, param2);
		newEdgeEnd.Orientation(thePiece.Edge.Orientation());//同向
		newPieceEnd.Edge = newEdgeEnd;
		result.push_back(newPieceEnd);
		break;
	}
	default:
		break;
	}
	return result;
}
static std::vector<VerticalPiece> CutEdgeAlongDir(VerticalPiece thePiece, double theSupportLen, double theCutLen, int theNum) {
	std::vector<VerticalPiece> result;
	//如果最小支撑长度不足，则不做切割
	if (theSupportLen <= 1)
	{
		result.push_back(thePiece);
		return result;
	}
	//获取底层曲线
	double first, last;
	Handle(Geom_Curve) aCurve = BRep_Tool::Curve(thePiece.Edge, first, last);
	double start = 0, end = 0;
	gp_Pnt startP = thePiece.StartPoint(), endP = thePiece.StartPoint();
	//第一条
	switch (thePiece.dir) {
	case X:
		start = thePiece.StartPoint().Y();
		end = thePiece.StartPoint().Y() + theSupportLen;

		startP.SetY(start);
		endP.SetY(end);
		break;
	case Y:
		start = thePiece.StartPoint().X();
		end = thePiece.StartPoint().X() + theSupportLen;

		startP.SetX(start);
		endP.SetX(end);
		break;
	default:
		break;
	}
	//投影点到曲线上，并获取投影点处的参数
	GeomAPI_ProjectPointOnCurve ppc1S(startP, aCurve);
	double param1S = ppc1S.LowerDistanceParameter();
	GeomAPI_ProjectPointOnCurve ppc2S(endP, aCurve);
	double param2S = ppc2S.LowerDistanceParameter();
	VerticalPiece newPieceS;
	newPieceS.dir = thePiece.dir;
	TopoDS_Edge newEdgeS = BRepBuilderAPI_MakeEdge(aCurve, param1S, param2S);
	newEdgeS.Orientation(thePiece.Edge.Orientation());//同向
	newPieceS.Edge = newEdgeS;
	result.push_back(newPieceS);
	//第一条之后的
	for (size_t i = 0; i < theNum; i++) {
		switch (thePiece.dir) {
		case X:
			start += theSupportLen + theCutLen;
			end += theSupportLen + theCutLen;

			startP.SetY(start);
			endP.SetY(end);
			break;
		case Y:
			start += theSupportLen + theCutLen;
			end += theSupportLen + theCutLen;

			startP.SetX(start);
			endP.SetX(end);
			break;
		default:
			break;
		}
		//投影点到曲线上，并获取投影点处的参数
		GeomAPI_ProjectPointOnCurve ppc1(startP, aCurve);
		double param1 = ppc1.LowerDistanceParameter();
		GeomAPI_ProjectPointOnCurve ppc2(endP, aCurve);
		double param2 = ppc2.LowerDistanceParameter();
		VerticalPiece newPiece;
		newPiece.dir = thePiece.dir;
		TopoDS_Edge newEdge = BRepBuilderAPI_MakeEdge(aCurve, param1, param2);
		newEdge.Orientation(thePiece.Edge.Orientation());//同向
		newPiece.Edge = newEdge;
		result.push_back(newPiece);
	}
	return result;
}
static std::vector<VerticalPiece> CutLongEdge(std::vector<VerticalPiece>& thePieces, double theSupportLen, double theCuttingDistance) {
	//如果两个参数值均为0，则不处理
	if (theSupportLen + theCuttingDistance == 0) {
		return thePieces;
	}
	std::vector<VerticalPiece> result;
	for (auto aPiece : thePieces) {
		//跳过不需要切割的长度
		if (aPiece.Length() < theSupportLen * 2 + theCuttingDistance) {
			result.push_back(aPiece);
			continue;
		}
		//计算需要切割的间隙是奇数还是偶数
		int quotient = (int)(aPiece.Length() - theSupportLen) / (theSupportLen + theCuttingDistance);
		if (quotient % 2 == 0) {
			// 商是偶数
			for (auto aP : CutEdgeUniform(aPiece, theSupportLen, quotient)) {
				result.push_back(aP);
			}
		}
		else {
			// 商是奇数，实际切割长度按照数目均分即可
			double restLen = (aPiece.Length() - theSupportLen) / quotient;
			double cutLen = restLen - theSupportLen;
			for (auto aP : CutEdgeAlongDir(aPiece, theSupportLen, cutLen, quotient)) {
				result.push_back(aP);
			}
		}
	}
	return result;
}

/// <summary>
/// 合并片体为一块板
/// </summary>
/// <param name="thePieces"></param>
/// <param name="theSupportLen"></param>
/// <param name="theCuttingDistance"></param>
/// <param name="theZ"></param>
/// <returns></returns>
void SuturePiece(VerticalPlate& thePlate, BasePlate theBasePlate, double theConnectionHight) {
	//创造一块连接板
	//! 板宽度不包含Offset的部分
	TopoDS_Face connectionPiece;
	gp_Pnt leftLow, p2, rightTop;
	switch (thePlate.dir)
	{
	case X:
		leftLow = gp_Pnt(thePlate.location, theBasePlate.Y, theBasePlate.Z);
		rightTop = gp_Pnt(thePlate.location, theBasePlate.Y + theBasePlate.dY, theBasePlate.Z + theConnectionHight);
		connectionPiece = MakePiece(leftLow, rightTop, Direction::X);
		break;
	case Y:
		leftLow = gp_Pnt(theBasePlate.X, thePlate.location, theBasePlate.Z);
		rightTop = gp_Pnt(theBasePlate.X + theBasePlate.dX, thePlate.location, theBasePlate.Z + theConnectionHight);
		connectionPiece = MakePiece(leftLow, rightTop, Direction::Y);
		break;
	default:
		throw std::runtime_error("Unexpected Direction in switch");
	}
	//把Piece和连接板融合
	TopoDS_Shape result = connectionPiece;
	auto theOrientation = connectionPiece.Orientation();
	for (VerticalPiece onePiece : thePlate.pieces) {
		TopoDS::Face(onePiece.Shape);
		auto oriShape = onePiece.Shape.Oriented(theOrientation);//todo 这一步并不管用
		BRepAlgoAPI_Fuse aFuse(result, oriShape);
		aFuse.SimplifyResult();// 简化结果
		result = aFuse.Shape();
	}


	////todo 内边界的融合会失败，不清楚原因
	////去掉融合后的内边界
	//ShapeUpgrade_UnifySameDomain unifyer(result, true, true, true);
	//unifyer.Build();
	//TopoDS_Shape result_unified = unifyer.Shape();
	////最终结果保存在Plate中
	//thePlate.shape = result_unified;
	thePlate.shape = result;
}

/// <summary>
/// 在竖板上切连接槽
/// </summary>
/// <param name="thePlate"></param>
/// <param name="theBasePlate"></param>
/// <param name="theLocations"></param>
/// <param name="theConnectionHight"></param>
/// <param name="theConnectWidth"></param>
/// <param name="theFilletRadius"></param>
void Slotting(VerticalPlate& thePlate, BasePlate theBasePlate, std::vector<double> theLocations, double theConnectionHight, double theConnectWidth, double theFilletRadius) {
	switch (thePlate.dir)
	{
	case Direction::X:
	{
		// 先在Y平面创建一个槽(从下往上）
		gp_Pnt p1(thePlate.location, -theConnectWidth / 2, theBasePlate.Z - 999);
		gp_Pnt p3(thePlate.location, theConnectWidth / 2, theBasePlate.Z + theConnectionHight / 2);
		TopoDS_Face theSlot = MakePiece(p1, p3, Direction::X, theFilletRadius);

		// 在每个位置切槽
		for (double aloc : theLocations) {
			gp_Trsf move;
			move.SetTranslation(gp_Vec(0, aloc, 0)); // 沿着Y
			BRepBuilderAPI_Transform transform(theSlot, move);
			transform.Build();
			TopoDS_Shape localSlot = transform.Shape();
			//更新shape
			thePlate.shape = BRepAlgoAPI_Cut(thePlate.shape, localSlot).Shape();
		}
		break;
	}
	case Direction::Y:
	{
		// 先在X平面创建一个槽(从上往下）
		gp_Pnt p1(-theConnectWidth / 2, thePlate.location, theBasePlate.Z + theConnectionHight / 2);
		gp_Pnt p3(theConnectWidth / 2, thePlate.location, theBasePlate.Z + 99999);
		TopoDS_Face theSlot = MakePiece(p1, p3, Direction::Y, theFilletRadius);
		// 在每个位置切槽
		for (double aloc : theLocations) {
			gp_Trsf move;
			move.SetTranslation(gp_Vec(aloc, 0, 0)); // 沿着X
			BRepBuilderAPI_Transform transform(theSlot, move);
			transform.Build();
			TopoDS_Shape localSlot = transform.Shape();
			//更新shape
			thePlate.shape = BRepAlgoAPI_Cut(thePlate.shape, localSlot).Shape();
		}
		break;
	}
	default:
		break;
	}
}

VerticalPlate MakeVerticalPlate(TopoDS_Shape theWorkpiece, BasePlate theBasePlate, Direction theDir, double theLoc, double theClearances, double theMinSupportLen, double theCutDistance)
{
	VerticalPlate onePlate; // result
	onePlate.dir = theDir;
	onePlate.location = theLoc;
	onePlate.clearances = theClearances;
	onePlate.minSupportLen = theMinSupportLen;
	onePlate.cuttiDistance = theCutDistance;

	TopoDS_Face aPlane = MakeCrossPlane(theLoc, theDir);
	TopoDS_Shape aSection = MakeSection(aPlane, theWorkpiece);
	std::vector<VerticalPiece> theOriginPieces = MakeVerticalPieceWithSection(aSection, theDir);
	TrimEdgeEnds(theOriginPieces, theClearances);
	std::vector<VerticalPiece> theRemovedPieces = RemoveShortEdge(theOriginPieces, theMinSupportLen);
	std::vector<VerticalPiece> theCuttedPieces = CutLongEdge(theRemovedPieces, theMinSupportLen, theCutDistance);

	//前面都是在操作Edge，这一步需要把Edge生成为Piece片体形状
	for (auto& aP : theCuttedPieces) {
		MakePieceFromEdge(aP, theBasePlate.Z);
		onePlate.pieces.push_back(aP);
	}
	return onePlate;
}

#pragma endregion
/// <summary>
/// 把所有板平铺
/// </summary>
/// <param name="theBasePlate"></param>
/// <param name="theVerticalPlates"></param>
/// <returns></returns>
TopoDS_Shape DeployPlates(BasePlate theBasePlate, std::vector<VerticalPlate> theVerticalPlates)
{
	double X = theBasePlate.X;
	double Y = theBasePlate.Y;
	double Z = theBasePlate.Z;
	double gap = 10.0;
#pragma region 横板
	gp_Trsf base_trsf = gp_Trsf();
	base_trsf.SetTranslationPart(gp_Vec(-X + theBasePlate.OffsetX + gap / 2, -Y + theBasePlate.OffsetY + gap / 2, -Z));
	TopoDS_Shape newBP = theBasePlate.shape.Moved(TopLoc_Location(base_trsf), true);
#pragma endregion
	double theX = 0.0;
	double theY = 0.0;
	std::vector<TopoDS_Shape> newVPs;
	for (auto theVP : theVerticalPlates)
	{
		switch (theVP.dir)
		{
		case Direction::X:
		{
			theY += gap / 2;
			gp_Trsf vp_toOrigin = gp_Trsf();
			gp_Trsf vp_trsf = gp_Trsf();
			vp_toOrigin.SetTranslationPart(gp_Vec(-theVP.location, -Y, -Z));
			theVP.shape = theVP.shape.Located(TopLoc_Location(vp_toOrigin));
			//! 注意,齐次变换是先旋转后平移的
			gp_Quaternion vp_quat = gp_Quaternion();
			vp_quat.SetEulerAngles(gp_Extrinsic_ZYX, 0, -M_PI / 2, 0);
			vp_trsf.SetRotationPart(vp_quat);
			vp_trsf.SetTranslationPart(gp_Vec(-gap / 2, theY, 0));
			TopoDS_Shape newVP = theVP.shape.Moved(TopLoc_Location(vp_trsf), true);
			newVPs.push_back(newVP);
			theY += theBasePlate.dY + gap / 2;
			break;
		}
		case Direction::Y:
		{
			theX += gap / 2;
			gp_Trsf vp_toOrigin = gp_Trsf();
			gp_Trsf vp_trsf = gp_Trsf();
			vp_toOrigin.SetTranslationPart(gp_Vec(-X, -theVP.location, -Z));
			theVP.shape = theVP.shape.Located(TopLoc_Location(vp_toOrigin));
			//! 注意,齐次变换是先旋转后平移的
			gp_Quaternion vp_quat = gp_Quaternion();
			vp_quat.SetEulerAngles(gp_Extrinsic_ZYX, 0, 0, M_PI / 2);
			vp_trsf.SetRotationPart(vp_quat);
			vp_trsf.SetTranslationPart(gp_Vec(theX, -gap / 2, 0));
			TopoDS_Shape newVP = theVP.shape.Moved(TopLoc_Location(vp_trsf), true);
			newVPs.push_back(newVP);
			theX += theBasePlate.dX + gap / 2;
			break;
		}
		default:
			break;
		}
	}
	TopoDS_Compound result = TopoDS_Compound();
	BRep_Builder builder = BRep_Builder();
	builder.MakeCompound(result);
	builder.Add(result, newBP);
	for (auto anewVP : newVPs)
	{
		builder.Add(result, anewVP);
	}
	return result;
}

bool saveSTEP(TopoDS_Shape theShape, TCollection_AsciiString filePath) {
	//初始化写入对象
	STEPControl_Writer aWriter;

	aWriter.Transfer(theShape, STEPControl_AsIs);
	IFSelect_ReturnStatus status = aWriter.Write(filePath.ToCString());

	if (status == IFSelect_RetDone)
	{
		return true;
	}
	return false;

}
}
}