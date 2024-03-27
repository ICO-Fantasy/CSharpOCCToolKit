#pragma once
#include "MakeSimpleClamp.h"
#include <BRepAdaptor_Surface.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <Bnd_Box.hxx>
#include <BRepBndLib.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepAlgoAPI_Section.hxx>
#include <vector>
#include <TopExp_Explorer.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <TopoDS.hxx>
#include <algorithm>
#include <BRepAlgoAPI_Fuse.hxx>
#include <GProp_GProps.hxx>
#include <BRepGProp.hxx>
#include <BRep_Builder.hxx>//test
#include <map>
#include <GC_MakeSegment.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepPrimAPI_MakePrism.hxx>
#include <GCPnts_TangentialDeflection.hxx>
#include <GProp_PEquation.hxx>
///==============================
///生成底板 
///==============================

/// <summary>
/// 测试用方法，获取底板
/// </summary>
/// <param name="theShape"></param>
/// <returns></returns>
static TopoDS_Shape GetTestFace(TopoDS_Shape theShape) {
	TopExp_Explorer aExp = TopExp_Explorer(theShape, TopAbs_FACE);
	Standard_Real maxArea = 0.0;
	Standard_Real minZ = 999.0;
	TopoDS_Shape result;
	while (aExp.More())
	{
		TopoDS_Shape curentShape = aExp.Current();  // 遍历得到每一个面
		aExp.Next();
		if (curentShape.ShapeType() == TopAbs_FACE)
		{ // 检查是否为面
			TopoDS_Face aFace = TopoDS::Face(curentShape);
			GProp_GProps aGprop = GProp_GProps();
			BRepGProp::SurfaceProperties(aFace, aGprop);
			Standard_Real theArea = aGprop.Mass();
			if (theArea > maxArea)
			{
				maxArea = theArea;
				BRepAdaptor_Surface aBAS = BRepAdaptor_Surface(aFace);
				// 曲线类型
				if (aBAS.GetType() == GeomAbs_Plane)
				{
					Standard_Real start_param = aBAS.FirstUParameter();
					Standard_Real end_param = aBAS.LastUParameter();
					gp_Pnt thePnt = aBAS.Value(start_param, end_param);
					Standard_Real theZ = thePnt.Z();
					minZ = theZ;
					result = curentShape;
				}
			}
			else if (theArea == maxArea)
			{
				BRepAdaptor_Surface aBAS = BRepAdaptor_Surface(aFace);
				// 曲线类型
				if (aBAS.GetType() == GeomAbs_Plane)
				{
					Standard_Real start_param = aBAS.FirstUParameter();
					Standard_Real end_param = aBAS.LastUParameter();
					gp_Pnt thePnt = aBAS.Value(start_param, end_param);
					Standard_Real theZ = thePnt.Z();
					if (theZ < minZ)
					{
						minZ = theZ;
						result = curentShape;
					}
				}
			}
		}
	}
	return result;
}
/// <summary>
/// 交互获取表面，得到表面的Z值（然后在C#中计算返回的Z值减去底板高dz）
/// </summary>
/// <param name="theFace"></param>
/// <returns></returns>
static Standard_Real GetZFromFace(TopoDS_Shape theFace) {
	BRepAdaptor_Surface aAdp_Face = BRepAdaptor_Surface(TopoDS::Face(theFace));
	Standard_Real centerU = aAdp_Face.FirstUParameter() + (aAdp_Face.LastUParameter() - aAdp_Face.FirstUParameter()) / 2;
	Standard_Real centerV = aAdp_Face.FirstVParameter() + (aAdp_Face.LastVParameter() - aAdp_Face.FirstVParameter()) / 2;
	gp_Pnt centerPoint = aAdp_Face.Value(centerU, centerV);
	Standard_Real theZ = centerPoint.Z();
	return theZ;
}
/// <summary>
/// 根据工件的AABB包围盒和输入的Z高度，得到两个角点（角点的Z值为0）
/// </summary>
/// <param name="theShape"></param>
/// <param name="theZ"></param>
/// <param name="plateThickness"></param>
/// <returns></returns>
static std::pair<gp_Pnt, gp_Pnt> GetCorners(TopoDS_Shape theShape) {
	Bnd_Box bbox;
	BRepBndLib::Add(theShape, bbox);
	Standard_Real xmin, ymin, zmin, xmax, ymax, zmax;
	bbox.Get(xmin, ymin, zmin, xmax, ymax, zmax);
	gp_Pnt lowerLeftPoint(xmin, ymin, 0);
	gp_Pnt topRightPoint(xmax, ymax, 0);
	return std::make_pair(lowerLeftPoint, topRightPoint);
}
/// <summary>
/// 根据两个角点和z值创建底板（需要首先在C#中创建两个角点）
/// </summary>
/// <param name="theShape"></param>
/// <param name="theZ"></param>
/// <param name="plateThickness"></param>
/// <returns></returns>
static TopoDS_Shape MakeBasePlateShape(gp_Pnt theLowerLeftPoint, gp_Pnt theTopRightPoint, Standard_Real theZ, Standard_Real plateThickness, Standard_Real dmx, Standard_Real dmy) {

	// 根据边界框尺寸创建立方体
	Standard_Real xmin = theLowerLeftPoint.X();
	Standard_Real ymin = theLowerLeftPoint.Y();
	Standard_Real xmax = theTopRightPoint.X();
	Standard_Real ymax = theTopRightPoint.Y();
	gp_Pnt lowerLeftPointWithZ(xmin - dmx, ymin - dmy, theZ);
	gp_Pnt topRightPointWithZ(xmax + dmx, ymax + dmy, theZ + plateThickness);
	BRepPrimAPI_MakeBox aBoxMaker(lowerLeftPointWithZ, topRightPointWithZ);
	return aBoxMaker.Shape();
}
///==============================
///生成竖板 
///==============================
namespace
{
	enum Direction {
		X,
		Y
	};
}
/// <summary>
/// 创建拓扑平面
/// </summary>
/// <returns></returns>
static TopoDS_Face MakeVerticalPlane(Standard_Real thePointStart, Direction theDirection) {
	gp_Dir PlaneNormal;
	gp_Pnt startPnt;
	switch (theDirection)
	{
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
	if (aSection.IsDone())
	{
		TopoDS_Shape sec_shape = aSection.Shape();
		return sec_shape;
	}
	throw std::runtime_error("截面获取失败");
}
namespace LocalSpace
{
	struct localEdge {
		localEdge(TopoDS_Edge theEdge) {
			Edge = theEdge;
			BRepAdaptor_Curve aBAC(theEdge);
			startPoint = aBAC.Value(aBAC.FirstParameter());
			endPoint = aBAC.Value(aBAC.LastParameter());
			middlePoint = aBAC.Value(aBAC.FirstParameter() + (aBAC.LastParameter() - aBAC.FirstParameter()) / 2);
		};
		TopoDS_Edge Edge;
		gp_Pnt startPoint;
		gp_Pnt endPoint;
		gp_Pnt middlePoint;
		// 重载 operator== 操作符，用于比较两个 localEdge 对象是否相等
		bool operator==(const localEdge& other) const {
			return startPoint.IsEqual(other.startPoint, 1e-4) && endPoint.IsEqual(other.endPoint, 1e-4);
		}
	};
	struct Ring {
		std::vector<localEdge> Edges;
		gp_Pnt startPoint;
		gp_Pnt endPoint;
	};
	struct PolyLine {
		std::vector<localEdge> Edges;
		gp_Pnt startPoint;
		gp_Pnt endPoint;
		// 重载 operator== 操作符，用于比较两个 PolyLine 对象是否相等
		bool operator==(const PolyLine& other) const {
			return startPoint.IsEqual(other.startPoint, 1e-4) && endPoint.IsEqual(other.endPoint, 1e-4);
		}
	};
	static void AddEdgeToRing(Ring& theRing, std::vector<localEdge>& Edges) {
		// 使用迭代器遍历 Edges
		for (auto it = Edges.begin(); it != Edges.end(); ++it)
		{
			gp_Pnt p1 = theRing.startPoint;
			gp_Pnt p2 = theRing.endPoint;
			gp_Pnt p3 = it->startPoint;
			gp_Pnt p4 = it->endPoint;
			if (theRing.endPoint.IsEqual(it->startPoint, 0.1))
			{
				theRing.endPoint = it->endPoint;
				theRing.Edges.push_back(*it);
				Edges.erase(it); // 删除匹配的边，并更新迭代器
				AddEdgeToRing(theRing, Edges);
				break;
			}
			else if (theRing.endPoint.IsEqual(it->endPoint, 0.1))
			{
				theRing.endPoint = it->startPoint;
				theRing.Edges.push_back(*it);
				Edges.erase(it); // 删除匹配的边，并更新迭代器
				AddEdgeToRing(theRing, Edges);
				break;
			}
			else if (theRing.startPoint.IsEqual(it->startPoint, 0.1))
			{
				theRing.startPoint = it->endPoint;
				theRing.Edges.push_back(*it);
				Edges.erase(it); // 删除匹配的边，并更新迭代器
				AddEdgeToRing(theRing, Edges);
				break;
			}
			else if (theRing.startPoint.IsEqual(it->endPoint, 0.1))
			{
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

		while (not Edges.empty())
		{
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
	static bool CurveIsLine(Handle(Geom_Curve) theGeomCurve, Standard_Real Tol) {
		Standard_Real distanceTol = 0.2;
		const Standard_Integer numPoints = 10; // 减少点的数量以提高效率
		TColgp_Array1OfPnt Pnts(1, numPoints);

		Standard_Real FirstParam = theGeomCurve->FirstParameter();
		Standard_Real LastParam = theGeomCurve->LastParameter();
		Standard_Real paramIncrement = (LastParam - FirstParam) / (numPoints - 1);

		// 填充点数组
		for (Standard_Integer i = 1; i <= numPoints; ++i)
		{
			Standard_Real Upara = FirstParam + paramIncrement * (i - 1);
			Pnts(i) = theGeomCurve->Value(Upara);
		}

		// 计算曲线端点之间的距离
		Standard_Real PntDistance = Pnts(1).Distance(Pnts(numPoints));

		// 如果端点之间的距离小于指定阈值，则认为是直线
		if (PntDistance < distanceTol)
		{
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
		Standard_Real first, last;
		Handle(Geom_Curve) aCurve = BRep_Tool::Curve(theBSpline, first, last);
		//离散为点
		GCPnts_TangentialDeflection aPointsOnCurve;// 切矢量偏离算法
		GeomAdaptor_Curve aCurveAdaptor(aCurve);
		aPointsOnCurve.Initialize(aCurveAdaptor, anADeflect, aCDeflect);
		for (int i = 1; i < aPointsOnCurve.NbPoints(); ++i)
		{
			Standard_Real firstP = aPointsOnCurve.Parameter(i);
			Standard_Real lastP = aPointsOnCurve.Parameter(i + 1);
			Handle(Geom_TrimmedCurve) tricurve = new Geom_TrimmedCurve(aCurve, firstP, lastP);
			if (CurveIsLine(tricurve, 0.15))
			{
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
		for (auto aEdge : theRing.Edges)
		{
			BRepAdaptor_Curve aBAC(aEdge.Edge);
			// 曲线类型
			if (aBAC.GetType() == GeomAbs_BSplineCurve)
			{
				std::vector<TopoDS_Edge>newEdges = ConvertBSplineToLine(aEdge.Edge);
				for (auto newEdge : newEdges)
				{
					newRing.Edges.push_back(localEdge(newEdge));
				}
			}
			else
			{
				auto theType = aBAC.GetType();
				newRing.Edges.push_back(aEdge);
			}
		}
		return newRing;
	}
	//todo采用直接删去垂直线段（沿Z方向）的逻辑，如果有超过两段线是垂直的，将会出错。
	static std::vector<localEdge>GetConstructionLinesFromRing(Ring theRing) {
		std::vector<localEdge> results;
		gp_Dir theZDir = gp_Dir(0, 0, 1);
		gp_Dir theMinusZDir = gp_Dir(0, 0, -1);
		std::vector<localEdge> theEdges = theRing.Edges;
		std::vector<localEdge> addedEdge;
		//排除所有非直线线段
		for (size_t i = 0; i < theEdges.size(); i++)
		{
			BRepAdaptor_Curve aBAC(theEdges[i].Edge);
			// 曲线类型
			auto theType = aBAC.GetType();
			if (aBAC.GetType() != GeomAbs_Line)
			{
				addedEdge.push_back(theEdges[i]);
			}
		}
		for (localEdge anEdge : theEdges)
		{
			//跳过已添加的线
			if (std::find(addedEdge.begin(), addedEdge.end(), anEdge) != addedEdge.end())//在数组中
			{
				continue;
			}
			//排除垂直的线
			gp_Dir edgeDir = gp_Dir(gp_Vec(anEdge.startPoint, anEdge.endPoint));
			if (edgeDir.IsParallel(theZDir, 1e-2) || edgeDir.IsParallel(theMinusZDir, 1e-2))
			{
				addedEdge.push_back(anEdge);
			}
			//根据长度找到另一个线（行不通）
			//else
			//{
			//	addedEdge.push_back(anEdge);
			//	Standard_Real edgeLen = anEdge.startPoint.Distance(anEdge.endPoint);
			//	for (size_t j = 0; j < theEdges.size(); j++)
			//	{
			//		localEdge otherEdge = theEdges[j];
			//		if (std::find(addedEdge.begin(), addedEdge.end(), otherEdge) != addedEdge.end())//在数组中
			//		{
			//			continue;
			//		}
			//		Standard_Real otherLen = otherEdge.startPoint.Distance(otherEdge.endPoint);
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
			else
			{
				addedEdge.push_back(anEdge);
				gp_Pnt2d p1(anEdge.startPoint.X(), anEdge.startPoint.Y());
				gp_Pnt2d p2(anEdge.endPoint.X(), anEdge.endPoint.Y());
				for (localEdge otherEdge : theEdges)
				{
					//跳过已添加的线
					if (std::find(addedEdge.begin(), addedEdge.end(), otherEdge) != addedEdge.end())//在数组中
					{
						continue;
					}
					gp_Pnt2d p3(otherEdge.startPoint.X(), otherEdge.startPoint.Y());
					gp_Pnt2d p4(otherEdge.endPoint.X(), otherEdge.endPoint.Y());
					double distanceTol = 1;
					if ((p1.Distance(p3) < distanceTol && p2.Distance(p4) < distanceTol) || (p1.Distance(p4) < distanceTol && p2.Distance(p3) < distanceTol))
					{
						gp_Pnt middlePoint1 = gp_Pnt((anEdge.startPoint.XYZ() + anEdge.endPoint.XYZ()) / 2);
						gp_Pnt middlePoint2 = gp_Pnt((otherEdge.startPoint.XYZ() + otherEdge.endPoint.XYZ()) / 2);
						if (middlePoint1.Z() < middlePoint2.Z())
						{
							results.push_back(anEdge);
						}
						else
						{
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
	static TopoDS_Shape MakeFaceFromLocalEdge(localEdge theEdge, Standard_Real theZ) {
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
	static std::vector<PolyLine> SplitRing(Ring theRing, Direction theDirection) {
		std::vector<localEdge>addedEdges;
		std::vector<localEdge> TempEdges;
		//!排除垂直的线
		gp_Dir theZDir = gp_Dir(0, 0, 1);
		gp_Dir theMinusZDir = gp_Dir(0, 0, -1);
		for (auto anEdge : theRing.Edges)
		{
			gp_Dir edgeDir = gp_Dir(gp_Vec(anEdge.startPoint, anEdge.endPoint));
			if (edgeDir.IsParallel(theZDir, 1e-1) || edgeDir.IsParallel(theMinusZDir, 1e-1))
			{
				addedEdges.push_back(anEdge);
			}
			else
			{
				TempEdges.push_back(anEdge);
			}
		}
		//!找出下端线
		bool IsDone;
		int begin = 0;
		std::vector<PolyLine> Lines, addedLines, resultLines, removedLines;
		int N1;
		switch (theDirection)
		{
		case X:
			//首先对每个线段中的首末点排序
			for (auto& anEdge : TempEdges)
			{
				if (anEdge.startPoint.Y() > anEdge.endPoint.Y())
				{
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
			while (begin < TempEdges.size())
			{
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
				while (IsDone)
				{
					bool continueF = false;
					for (localEdge anEdge : TempEdges)
					{
						if (std::find(addedEdges.begin(), addedEdges.end(), anEdge) != addedEdges.end()) {
							continue;
						}
						//拼接两点，并确保Y递增
						if (anEdge.startPoint.IsEqual(aLine.endPoint, 1e-2) && anEdge.endPoint.Y() > aLine.endPoint.Y())
						{
							addedEdges.push_back(anEdge);
							//Edge加入line中，并更新末端点
							aLine.Edges.push_back(anEdge);
							aLine.endPoint = anEdge.endPoint;
							continueF = true;
							break;
						}
					}
					if (continueF)
					{
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
			while (N1 < Lines.size())
			{
				PolyLine L1 = Lines[N1];
				if (std::find(addedLines.begin(), addedLines.end(), L1) != addedLines.end())
				{
					N1++;
					continue;
				}
				addedLines.push_back(L1);//待比较的项，先加入added
				int N2 = 0;
				while (N2 < Lines.size())
				{
					PolyLine L2 = Lines[N2];
					if (std::find(addedLines.begin(), addedLines.end(), L2) != addedLines.end())
					{
						N2++;
						continue;
					}
					// 比较L1和L2
					//如果区间重叠
					Standard_Real p1 = L1.startPoint.Y(), p2 = L1.endPoint.Y(), p3 = L2.startPoint.Y(), p4 = L2.endPoint.Y();
					Standard_Real middle = -999999.0;
					if (p1 < p3 && p3 < p2) {
						middle = (p3 + p2) / 2;
					}
					else if (p3 < p1 && p1 < p4) {
						auto middle = (p1 + p4) / 2;
					}
					else if (p3 < p1 && p2 < p4) {
						auto middle = (p1 + p2) / 2;
					}
					else if (p1 < p3 && p4 < p2) {
						auto middle = (p3 + p4) / 2;
					}
					if (middle != -999999.0) {//找到中点对应的边上对应的点
						Standard_Real z1 = 0, z2 = 0;
						for (auto anEdge : L1.Edges)
						{
							if (anEdge.startPoint.Y() <= middle && middle <= anEdge.endPoint.Y())
							{
								BRepAdaptor_Curve aBAC(anEdge.Edge);
								Standard_Real ap1 = aBAC.FirstParameter();
								Standard_Real ap2 = aBAC.LastParameter();
								if (ap1 > ap2)
								{
									std::swap(ap1, ap2);
								}
								z1 = aBAC.Value(ap1 + (ap2 - ap1) * (middle - anEdge.startPoint.Y()) / (anEdge.endPoint.Y() - anEdge.startPoint.Y())).Z();
								break;
							}
						}
						for (auto anEdge : L2.Edges)
						{
							if (anEdge.startPoint.Y() <= middle && middle <= anEdge.endPoint.Y())
							{
								BRepAdaptor_Curve aBAC(anEdge.Edge);
								Standard_Real ap1 = aBAC.FirstParameter();
								Standard_Real ap2 = aBAC.LastParameter();
								if (ap1 > ap2)
								{
									std::swap(ap1, ap2);
								}
								z2 = aBAC.Value(ap1 + (ap2 - ap1) * (middle - anEdge.startPoint.Y()) / (anEdge.endPoint.Y() - anEdge.startPoint.Y())).Z();
								break;
							}
						}
						//比较边上对应的点的Z值大小，取小的线段
						if (z1 < z2)
						{
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
			for (auto theLine : Lines)
			{
				if (std::find(removedLines.begin(), removedLines.end(), theLine) == removedLines.end())
				{
					resultLines.push_back(theLine);
				}
			}
#pragma endregion
			break;
		case Y:
			//todo 还没做
			break;
		default:
			//todo错误处理
			break;
		}
		return resultLines;
	}
}
static std::vector<TopoDS_Shape> MakeVerticalPieceWithSection(TopoDS_Shape theSection, Direction theDirection, Standard_Real theZ, Standard_Real VerticalPlateClearances, Standard_Real VerticalPlateMinSupportingLen, Standard_Real VerticalPlateCuttingDistance) {
	std::vector<TopoDS_Shape>result;
#pragma region 得到原始构造线
	//!首先判断有几个环，对环进行处理
	std::vector<LocalSpace::localEdge>TempEdges;
	std::vector<LocalSpace::Ring>Rings;
	TopExp_Explorer aEdgeExp = TopExp_Explorer(theSection, TopAbs_EDGE);
	while (aEdgeExp.More())
	{
		LocalSpace::localEdge anEdge(TopoDS::Edge(aEdgeExp.Current()));// 遍历得到每一个边
		aEdgeExp.Next();
		TempEdges.push_back(anEdge);
	}
	Rings = GetRings(TempEdges);
	//!每个环中取最下边的线段作为原始构造线
	std::vector<LocalSpace::localEdge> originConstructionLines;
	for (auto aRing : Rings)
	{
		auto tempPLs = SplitRing(aRing, theDirection);
		for (auto pl : tempPLs) {
			for (auto l : pl.Edges)
			{
				result.push_back(l.Edge);
			}
		}
		////todo将LocalSpace::Ring中的线都转化为直线，去掉圆弧
		//Ring newRing = ConvertRing(aRing);
		////todo采用直接删去垂直线段（沿Z方向）的逻辑，如果有超过两段线是垂直的，将会出错。
		//std::vector<localEdge>theEdges = GetConstructionLinesFromRing(newRing);
		//if (!theEdges.empty())
		//{
		//	originConstructionLines.insert(originConstructionLines.end(), theEdges.begin(), theEdges.end());
		//}
	}
#pragma endregion
	return result;
}
// 定义一个结构体
struct myEdge {
	TopoDS_Edge topo;
	Standard_Real len;
	gp_Pnt point1;
	gp_Pnt point2;
	gp_Dir dir;
};
static bool RmoveParallelX(myEdge anEdge) {
	gp_Dir theDir = gp_Dir(1, 0, 0);
	return anEdge.dir.IsParallel(theDir, 1e-2);
}
static bool RmoveParallelY(myEdge anEdge) {
	gp_Dir theDir = gp_Dir(0, 1, 0);
	return anEdge.dir.IsParallel(theDir, 1e-2);
}
static bool IsEdgePiar(myEdge edge1, myEdge edge2) {
	if (std::abs(edge1.len - edge2.len) < 1e-4)
	{
		if (((fabs(edge1.point1.X() - edge2.point1.X()) < 1e-4 && fabs(edge1.point1.Y() - edge2.point1.Y()) < 1e-4) &&
			(fabs(edge1.point2.X() - edge2.point2.X()) < 1e-4 && fabs(edge1.point2.Y() - edge2.point2.Y()) < 1e-4)) ||
			((fabs(edge1.point1.X() - edge2.point2.X()) < 1e-4 && fabs(edge1.point1.Y() - edge2.point2.Y()) < 1e-4) &&
				(fabs(edge1.point2.X() - edge2.point1.X()) < 1e-4 && fabs(edge1.point2.Y() - edge2.point1.Y()) < 1e-4)))
		{

			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

///==============================
///生成竖板 (需求变更，以下代码废弃)
///==============================
/// <summary>
/// 生成竖板
/// </summary>
/// <param name="theSection"></param>
/// <param name="theDirection"></param>
/// <param name="otherSections"></param>
/// <param name="theX"></param>
/// <param name="theY"></param>
/// <param name="theZ"></param>
/// <returns></returns>
static TopoDS_Shape MakeVerticalPlateWithSection(std::pair<double, TopoDS_Shape> theSectionPair, Direction theDirection, std::map<double, TopoDS_Shape> otherSections, Standard_Real BasePlateLength, Standard_Real theX, Standard_Real theY, Standard_Real theZ, Standard_Real VerticalPlateThickness, Standard_Real VerticalPlateConnectionHeight, Standard_Real VerticalPlateClearances, Standard_Real VerticalPlateMinSupportingLen, Standard_Real VerticalPlateCuttingDistance) {
	//todo首先生成板，再考虑连接

	//!首先判断有几个环，对环进行处理
	std::vector<LocalSpace::localEdge>TempEdges;
	std::vector<LocalSpace::Ring>Rings;
	TopExp_Explorer aEdgeExp = TopExp_Explorer(theSectionPair.second, TopAbs_EDGE);
	while (aEdgeExp.More())
	{
		LocalSpace::localEdge anEdge(TopoDS::Edge(aEdgeExp.Current()));// 遍历得到每一个边
		aEdgeExp.Next();
		TempEdges.push_back(anEdge);
	}
	Rings = GetRings(TempEdges);
	//!每个环中取最下边的线段作为原始构造线
	std::vector<LocalSpace::localEdge> originConstructionLines;
	//for (auto aRing = Rings.begin(); aRing != Rings.end(); ++aRing)
	for (auto aRing : Rings)
	{
		//todo采用直接删去垂直线段（沿Z方向）的逻辑，如果有超过两段线是垂直的，将会出错。
		std::vector<LocalSpace::localEdge>theEdges = GetConstructionLinesFromRing(aRing);
		originConstructionLines.insert(originConstructionLines.end(), theEdges.begin(), theEdges.end());
	}
#pragma region 将原始构造线进行切分和缩短
	std::vector<LocalSpace::localEdge> cuttedConstructionLines;
	for (auto& originEdge : originConstructionLines)
	{
		gp_Vec moveDir = gp_Vec(originEdge.startPoint, originEdge.endPoint).Normalized();
#pragma region 缩短
		gp_Trsf start_trsf = gp_Trsf();
		start_trsf.SetTranslation(moveDir * VerticalPlateClearances);
		originEdge.startPoint = originEdge.startPoint.Transformed(start_trsf);

		gp_Trsf end_trsf = gp_Trsf();
		end_trsf.SetTranslation(moveDir * -VerticalPlateClearances);
		originEdge.endPoint = originEdge.endPoint.Transformed(end_trsf);
#pragma endregion
#pragma region 切割
		//!优先保证两侧有支撑，然后在中间切割
		Standard_Real originEdgeLen = originEdge.startPoint.Distance(originEdge.endPoint);
		Standard_Real originEdge2DLen = gp_Pnt2d(originEdge.startPoint.X(), originEdge.startPoint.Y()).Distance(gp_Pnt2d(originEdge.endPoint.X(), originEdge.endPoint.Y()));
		//如果线不平行于XoY平面，需要乘上cos系数
		Standard_Real cosValue = 1.0;
		if (originEdgeLen != originEdge2DLen)
		{
			cosValue = originEdge2DLen / originEdgeLen;
		}
		if (originEdge2DLen > 2 * VerticalPlateMinSupportingLen)
		{
			//todo因为段数少，先直接套if判断。后续优化
			if (originEdge2DLen > 3 * VerticalPlateMinSupportingLen + 2 * VerticalPlateCuttingDistance)
			{
				if (originEdge2DLen > 4 * VerticalPlateMinSupportingLen + 3 * VerticalPlateCuttingDistance)
				{
					//生成4段
					//todo先不做
				}
				else
				{
					//生成3段
					LocalSpace::localEdge firstEdge = originEdge;
					LocalSpace::localEdge secondEdge = originEdge;
					LocalSpace::localEdge thirdEdge = originEdge;
					gp_Pnt middlePnt = gp_Pnt((originEdge.startPoint.XYZ() + originEdge.endPoint.XYZ()) / 2);

					gp_Trsf firstTrsf = gp_Trsf();
					firstTrsf.SetTranslation(moveDir * VerticalPlateMinSupportingLen);
					firstEdge.endPoint = firstEdge.startPoint.Transformed(firstTrsf);

					gp_Trsf secondTrsf_Start = gp_Trsf();
					gp_Trsf secondTrsf_End = gp_Trsf();
					secondTrsf_Start.SetTranslation(moveDir * -VerticalPlateMinSupportingLen);
					secondTrsf_End.SetTranslation(moveDir * VerticalPlateMinSupportingLen);
					secondEdge.startPoint = middlePnt.Transformed(secondTrsf_Start);
					secondEdge.endPoint = middlePnt.Transformed(secondTrsf_End);

					gp_Trsf thirdTrsf = gp_Trsf();
					thirdTrsf.SetTranslation(moveDir * -VerticalPlateMinSupportingLen);
					thirdEdge.startPoint = secondEdge.endPoint.Transformed(thirdTrsf);

					cuttedConstructionLines.push_back(firstEdge);
					cuttedConstructionLines.push_back(secondEdge);
					cuttedConstructionLines.push_back(thirdEdge);
				}
			}
			else
			{
				//生成对称2段支撑
				LocalSpace::localEdge firstEdge = originEdge;
				LocalSpace::localEdge secondEdge = originEdge;

				gp_Trsf firstTrsf = gp_Trsf();
				firstTrsf.SetTranslation(moveDir * VerticalPlateMinSupportingLen / cosValue);
				firstEdge.endPoint = firstEdge.startPoint.Transformed(firstTrsf);

				gp_Trsf secondTrsf = gp_Trsf();
				secondTrsf.SetTranslation(moveDir * -VerticalPlateMinSupportingLen / cosValue);
				secondEdge.startPoint = secondEdge.endPoint.Transformed(secondTrsf);

				cuttedConstructionLines.push_back(firstEdge);
				cuttedConstructionLines.push_back(secondEdge);
			}
		}
		else
		{
			//不修改支撑线
			cuttedConstructionLines.push_back(originEdge);
		}
#pragma endregion
	}
#pragma endregion
#pragma region 从支撑线生成板
	Standard_Real plateZ = theZ + VerticalPlateConnectionHeight;
	std::vector<gp_Pnt> ConstructionPoints;
	for (LocalSpace::localEdge anEdge : cuttedConstructionLines)
	{
		ConstructionPoints.push_back(anEdge.startPoint);
		ConstructionPoints.push_back(anEdge.endPoint);
	}
	std::vector<Handle(Geom_TrimmedCurve)> theSegs;
	TopoDS_Shape theVerticalPlate;
	switch (theDirection)
	{
	case X:
	{
		Standard_Real plateFixed = theSectionPair.first;
		//对所有构建的点进行排序
		//按照 Y 值从小到大排序，使用 lambda 表达式
		std::sort(ConstructionPoints.begin(), ConstructionPoints.end(), [](const auto& a, const auto& b) {
			return a.Y() < b.Y();
			});
		//todo没有检查GC_MakeSegment是否构造成功
		for (size_t i = 0; i < ConstructionPoints.size(); i++)
		{
			gp_Pnt currentPnt = ConstructionPoints[i];
			if (i == 0)
			{
				Handle(Geom_TrimmedCurve) firstVerticalLine = GC_MakeSegment(gp_Pnt(plateFixed, theY, theZ), gp_Pnt(plateFixed, theY, plateZ));//原点到支撑高
				theSegs.push_back(firstVerticalLine);
				Handle(Geom_TrimmedCurve) startSeg = GC_MakeSegment(gp_Pnt(plateFixed, theY, plateZ), gp_Pnt(currentPnt.X(), currentPnt.Y(), plateZ));//默认初始没有支撑，为连接段
				theSegs.push_back(startSeg);
			}
			else if (i == ConstructionPoints.size() - 1)
			{
				//重复一次偶数段
				//竖向向下连接
				Handle(Geom_TrimmedCurve) theVerticalConnectionSeg = GC_MakeSegment(currentPnt, gp_Pnt(currentPnt.X(), currentPnt.Y(), plateZ));
				theSegs.push_back(theVerticalConnectionSeg);
				//偶数点时构造连接段
				Handle(Geom_TrimmedCurve) theConnectionSeg = GC_MakeSegment(gp_Pnt(currentPnt.X(), currentPnt.Y(), plateZ), gp_Pnt(plateFixed, theY + BasePlateLength, plateZ));
				theSegs.push_back(theConnectionSeg);

				Handle(Geom_TrimmedCurve) lastVerticalLine = GC_MakeSegment(gp_Pnt(plateFixed, theY + BasePlateLength, plateZ), gp_Pnt(plateFixed, theY + BasePlateLength, theZ));//末段下降
				theSegs.push_back(lastVerticalLine);
				Handle(Geom_TrimmedCurve) baseLine = GC_MakeSegment(gp_Pnt(plateFixed, theY + BasePlateLength, theZ), gp_Pnt(plateFixed, theY, theZ));//基平面段
				theSegs.push_back(baseLine);
				break;//避免后续奇数段i+1超范围
			}
			if (i % 2 == 0)
			{
				gp_Pnt nextPnt = ConstructionPoints[i + 1];
				//竖向向上连接
				Handle(Geom_TrimmedCurve) theVerticalSupportSeg = GC_MakeSegment(gp_Pnt(currentPnt.X(), currentPnt.Y(), plateZ), currentPnt);
				theSegs.push_back(theVerticalSupportSeg);
				//偶数点时构造支撑段（从0开始，为第1+2n个点）
				Handle(Geom_TrimmedCurve) theSupportSeg = GC_MakeSegment(currentPnt, nextPnt);
				theSegs.push_back(theSupportSeg);
			}
			else
			{
				gp_Pnt nextPnt = ConstructionPoints[i + 1];
				//竖向向下连接
				Handle(Geom_TrimmedCurve) theVerticalConnectionSeg = GC_MakeSegment(currentPnt, gp_Pnt(currentPnt.X(), currentPnt.Y(), plateZ));
				theSegs.push_back(theVerticalConnectionSeg);
				//奇数点时构造连接段（从0开始，为第2n个点）
				Handle(Geom_TrimmedCurve) theConnectionSeg = GC_MakeSegment(gp_Pnt(currentPnt.X(), currentPnt.Y(), plateZ), gp_Pnt(nextPnt.X(), nextPnt.Y(), plateZ));
				theSegs.push_back(theConnectionSeg);
			}
		}
		BRepBuilderAPI_MakeWire aWireMk = BRepBuilderAPI_MakeWire();
		for (Handle(Geom_TrimmedCurve) aSeg : theSegs)
		{
			TopoDS_Edge anEdge = BRepBuilderAPI_MakeEdge(aSeg);
			aWireMk.Add(anEdge);
		}
		TopoDS_Face myFaceProfile = BRepBuilderAPI_MakeFace(aWireMk.Wire());
		//沿着X负方向平移半个厚度
		gp_Trsf halfThick = gp_Trsf();
		halfThick.SetTranslationPart(gp_Vec(-VerticalPlateThickness / 2, 0, 0));
		myFaceProfile.Move(TopLoc_Location(halfThick));
		//根据曲线构建扫掠
		theVerticalPlate = BRepPrimAPI_MakePrism(myFaceProfile, gp_Vec(VerticalPlateThickness, 0, 0));
		break;
	}
	case Y:
	{
		Standard_Real plateFixed = theSectionPair.first;
		//按照 X 值从小到大排序，使用 lambda 表达式
		std::sort(ConstructionPoints.begin(), ConstructionPoints.end(), [](const auto& a, const auto& b) {
			return a.X() < b.X();
			});
		//todo没有检查GC_MakeSegment是否构造成功
		for (size_t i = 0; i < ConstructionPoints.size(); i++)
		{
			gp_Pnt currentPnt = ConstructionPoints[i];
			if (i == 0)
			{
				Handle(Geom_TrimmedCurve) firstVerticalLine = GC_MakeSegment(gp_Pnt(theX, plateFixed, theZ), gp_Pnt(theX, plateFixed, plateZ));//原点到支撑高
				theSegs.push_back(firstVerticalLine);
				Handle(Geom_TrimmedCurve) startSeg = GC_MakeSegment(gp_Pnt(theX, plateFixed, plateZ), gp_Pnt(currentPnt.X(), currentPnt.Y(), plateZ));//默认初始没有支撑，为连接段
				theSegs.push_back(startSeg);
			}
			else if (i == ConstructionPoints.size() - 1)
			{
				//重复一次偶数段
				//竖向向下连接
				Handle(Geom_TrimmedCurve) theVerticalConnectionSeg = GC_MakeSegment(currentPnt, gp_Pnt(currentPnt.X(), currentPnt.Y(), plateZ));
				theSegs.push_back(theVerticalConnectionSeg);
				//偶数点时构造连接段
				Handle(Geom_TrimmedCurve) theConnectionSeg = GC_MakeSegment(gp_Pnt(currentPnt.X(), currentPnt.Y(), plateZ), gp_Pnt(theX + BasePlateLength, plateFixed, plateZ));
				theSegs.push_back(theConnectionSeg);

				Handle(Geom_TrimmedCurve) lastVerticalLine = GC_MakeSegment(gp_Pnt(theX + BasePlateLength, plateFixed, plateZ), gp_Pnt(theX + BasePlateLength, plateFixed, theZ));//末段下降
				theSegs.push_back(lastVerticalLine);
				Handle(Geom_TrimmedCurve) baseLine = GC_MakeSegment(gp_Pnt(theX + BasePlateLength, plateFixed, theZ), gp_Pnt(theX, plateFixed, theZ));//基平面段
				theSegs.push_back(baseLine);
				break;//避免后续奇数段i+1超范围
			}
			if (i % 2 == 0)
			{
				gp_Pnt nextPnt = ConstructionPoints[i + 1];
				//竖向向上连接
				Handle(Geom_TrimmedCurve) theVerticalSupportSeg = GC_MakeSegment(gp_Pnt(currentPnt.X(), currentPnt.Y(), plateZ), currentPnt);
				theSegs.push_back(theVerticalSupportSeg);
				//偶数点时构造支撑段（从0开始，为第1+2n个点）
				Handle(Geom_TrimmedCurve) theSupportSeg = GC_MakeSegment(currentPnt, nextPnt);
				theSegs.push_back(theSupportSeg);
			}
			else
			{
				gp_Pnt nextPnt = ConstructionPoints[i + 1];
				//竖向向下连接
				Handle(Geom_TrimmedCurve) theVerticalConnectionSeg = GC_MakeSegment(currentPnt, gp_Pnt(currentPnt.X(), currentPnt.Y(), plateZ));
				theSegs.push_back(theVerticalConnectionSeg);
				//奇数点时构造连接段（从0开始，为第2n个点）
				Handle(Geom_TrimmedCurve) theConnectionSeg = GC_MakeSegment(gp_Pnt(currentPnt.X(), currentPnt.Y(), plateZ), gp_Pnt(nextPnt.X(), nextPnt.Y(), plateZ));
				theSegs.push_back(theConnectionSeg);
			}
		}
		BRepBuilderAPI_MakeWire aWireMk = BRepBuilderAPI_MakeWire();
		for (Handle(Geom_TrimmedCurve) aSeg : theSegs)
		{
			TopoDS_Edge anEdge = BRepBuilderAPI_MakeEdge(aSeg);
			aWireMk.Add(anEdge);
		}
		TopoDS_Face myFaceProfile = BRepBuilderAPI_MakeFace(aWireMk.Wire());
		//沿着Y负方向平移半个厚度
		gp_Trsf halfThick = gp_Trsf();
		halfThick.SetTranslationPart(gp_Vec(0, -VerticalPlateThickness / 2, 0));
		myFaceProfile.Move(TopLoc_Location(halfThick));
		//根据曲线构建扫掠
		theVerticalPlate = BRepPrimAPI_MakePrism(myFaceProfile, gp_Vec(0, VerticalPlateThickness, 0));
		break;
	}
	default:
	{
		throw std::invalid_argument("Invalid direction. Only X or Y are supported.");
		break;
	}
	}
#pragma region 构造连接槽
	TopoDS_Shape theVerticalPlateWithSlot = theVerticalPlate;
	switch (theDirection)
	{
	case X:
		//沿着
		break;
	case Y:
		break;
	default:
		break;
	}
#pragma endregion

	return theVerticalPlateWithSlot;

#pragma endregion
	////!test
	//// 创建一个空的复合形状
	//TopoDS_Compound compound;
	//BRep_Builder builder;
	//builder.MakeCompound(compound);
	//// 将所有的 TopoDS_Edge 添加到复合形状中
	//for (const auto& edge : originConstructionLines)
	//{
	//	builder.Add(compound, edge.Edge);
	//}
	//return compound;
}

///==============================
/// (需求变更，以下代码废弃)
///==============================
//static std::vector<TopoDS_Shape> MakeVerticalPieceWithSection(TopoDS_Shape theSection, Direction theDirection, Standard_Real theZ, Standard_Real VerticalPlateClearances, Standard_Real VerticalPlateMinSupportingLen, Standard_Real VerticalPlateCuttingDistance) {
//	std::vector<TopoDS_Shape>result;
//#pragma region 得到原始构造线
//	//!首先判断有几个环，对环进行处理
//	std::vector<localEdge>TempEdges;
//	std::vector<Ring>Rings;
//	TopExp_Explorer aEdgeExp = TopExp_Explorer(theSection, TopAbs_EDGE);
//	while (aEdgeExp.More())
//	{
//		localEdge anEdge(TopoDS::Edge(aEdgeExp.Current()));// 遍历得到每一个边
//		aEdgeExp.Next();
//		TempEdges.push_back(anEdge);
//	}
//	Rings = GetRings(TempEdges);
//	//!每个环中取最下边的线段作为原始构造线
//	std::vector<localEdge> originConstructionLines;
//	for (auto aRing : Rings)
//	{
//		//todo将Ring中的线都转化为直线，去掉圆弧
//		Ring newRing = ConvertRing(aRing);
//		//todo采用直接删去垂直线段（沿Z方向）的逻辑，如果有超过两段线是垂直的，将会出错。
//		std::vector<localEdge>theEdges = GetConstructionLinesFromRing(newRing);
//		if (!theEdges.empty())
//		{
//			originConstructionLines.insert(originConstructionLines.end(), theEdges.begin(), theEdges.end());
//		}
//	}
//#pragma endregion
//#pragma region 将原始构造线进行切分和缩短
//	std::vector<localEdge> cuttedConstructionLines;
//	for (auto& originEdge : originConstructionLines)
//	{
//		gp_Vec moveDir = gp_Vec(originEdge.startPoint, originEdge.endPoint).Normalized();
//#pragma region 缩短
//		gp_Trsf start_trsf = gp_Trsf();
//		start_trsf.SetTranslation(moveDir * VerticalPlateClearances);
//		originEdge.startPoint = originEdge.startPoint.Transformed(start_trsf);
//
//		gp_Trsf end_trsf = gp_Trsf();
//		end_trsf.SetTranslation(moveDir * -VerticalPlateClearances);
//		originEdge.endPoint = originEdge.endPoint.Transformed(end_trsf);
//#pragma endregion
//#pragma region 切割
//		//!优先保证两侧有支撑，然后在中间切割
//		Standard_Real originEdgeLen = originEdge.startPoint.Distance(originEdge.endPoint);
//		Standard_Real originEdge2DLen = gp_Pnt2d(originEdge.startPoint.X(), originEdge.startPoint.Y()).Distance(gp_Pnt2d(originEdge.endPoint.X(), originEdge.endPoint.Y()));
//		//如果线不平行于XoY平面，需要乘上cos系数
//		Standard_Real cosValue = 1.0;
//		if (originEdgeLen != originEdge2DLen)
//		{
//			cosValue = originEdge2DLen / originEdgeLen;
//		}
//		if (originEdge2DLen > 2 * VerticalPlateMinSupportingLen)
//		{
//			//todo因为段数少，先直接套if判断。后续优化
//			if (originEdge2DLen > 3 * VerticalPlateMinSupportingLen + 2 * VerticalPlateCuttingDistance)
//			{
//				if (originEdge2DLen > 4 * VerticalPlateMinSupportingLen + 3 * VerticalPlateCuttingDistance)
//				{
//					//生成4段
//					//todo先不做
//				}
//				else
//				{
//					//生成3段
//					localEdge firstEdge = originEdge;
//					localEdge secondEdge = originEdge;
//					localEdge thirdEdge = originEdge;
//					gp_Pnt middlePnt = gp_Pnt((originEdge.startPoint.XYZ() + originEdge.endPoint.XYZ()) / 2);
//
//					gp_Trsf firstTrsf = gp_Trsf();
//					firstTrsf.SetTranslation(moveDir * VerticalPlateMinSupportingLen);
//					firstEdge.endPoint = firstEdge.startPoint.Transformed(firstTrsf);
//
//					gp_Trsf secondTrsf_Start = gp_Trsf();
//					gp_Trsf secondTrsf_End = gp_Trsf();
//					secondTrsf_Start.SetTranslation(moveDir * -VerticalPlateMinSupportingLen);
//					secondTrsf_End.SetTranslation(moveDir * VerticalPlateMinSupportingLen);
//					secondEdge.startPoint = middlePnt.Transformed(secondTrsf_Start);
//					secondEdge.endPoint = middlePnt.Transformed(secondTrsf_End);
//
//					gp_Trsf thirdTrsf = gp_Trsf();
//					thirdTrsf.SetTranslation(moveDir * -VerticalPlateMinSupportingLen);
//					thirdEdge.startPoint = secondEdge.endPoint.Transformed(thirdTrsf);
//
//					cuttedConstructionLines.push_back(firstEdge);
//					cuttedConstructionLines.push_back(secondEdge);
//					cuttedConstructionLines.push_back(thirdEdge);
//				}
//			}
//			else
//			{
//				//生成对称2段支撑
//				localEdge firstEdge = originEdge;
//				localEdge secondEdge = originEdge;
//
//				gp_Trsf firstTrsf = gp_Trsf();
//				firstTrsf.SetTranslation(moveDir * VerticalPlateMinSupportingLen / cosValue);
//				firstEdge.endPoint = firstEdge.startPoint.Transformed(firstTrsf);
//
//				gp_Trsf secondTrsf = gp_Trsf();
//				secondTrsf.SetTranslation(moveDir * -VerticalPlateMinSupportingLen / cosValue);
//				secondEdge.startPoint = secondEdge.endPoint.Transformed(secondTrsf);
//
//				cuttedConstructionLines.push_back(firstEdge);
//				cuttedConstructionLines.push_back(secondEdge);
//			}
//		}
//		else
//		{
//			//不修改支撑线
//			cuttedConstructionLines.push_back(originEdge);
//		}
//#pragma endregion
//	}
//	//根据构造线，生成平板
//	for (auto theEdge : cuttedConstructionLines)
//	{
//		result.push_back(MakeFaceFromLocalEdge(theEdge, theZ));
//	}
//#pragma endregion
//	return result;
//}
//// 定义一个结构体
//struct myEdge {
//	TopoDS_Edge topo;
//	Standard_Real len;
//	gp_Pnt point1;
//	gp_Pnt point2;
//	gp_Dir dir;
//};
//static bool RmoveParallelX(myEdge anEdge) {
//	gp_Dir theDir = gp_Dir(1, 0, 0);
//	return anEdge.dir.IsParallel(theDir, 1e-2);
//}
//static bool RmoveParallelY(myEdge anEdge) {
//	gp_Dir theDir = gp_Dir(0, 1, 0);
//	return anEdge.dir.IsParallel(theDir, 1e-2);
//}
//static bool IsEdgePiar(myEdge edge1, myEdge edge2) {
//	if (std::abs(edge1.len - edge2.len) < 1e-4)
//	{
//		if (((fabs(edge1.point1.X() - edge2.point1.X()) < 1e-4 && fabs(edge1.point1.Y() - edge2.point1.Y()) < 1e-4) &&
//			(fabs(edge1.point2.X() - edge2.point2.X()) < 1e-4 && fabs(edge1.point2.Y() - edge2.point2.Y()) < 1e-4)) ||
//			((fabs(edge1.point1.X() - edge2.point2.X()) < 1e-4 && fabs(edge1.point1.Y() - edge2.point2.Y()) < 1e-4) &&
//				(fabs(edge1.point2.X() - edge2.point1.X()) < 1e-4 && fabs(edge1.point2.Y() - edge2.point1.Y()) < 1e-4)))
//		{
//
//			return true;
//		}
//		else
//		{
//			return false;
//		}
//	}
//	else
//	{
//		return false;
//	}
//}
//std::vector<std::pair<gp_Pnt, gp_Pnt>> GetPointsFromSection(TopoDS_Shape theSection, void (*func)(int))
///// <summary>
///// 遍历截面的线，知道用哪些点创建竖板X
///// </summary>
///// <param name="theSection"></param>
///// <returns></returns>
//static std::vector<std::pair<gp_Pnt, gp_Pnt>> GetPointsFromSectionX(TopoDS_Shape theSection) {
//	TopExp_Explorer aEdgeExp = TopExp_Explorer(theSection, TopAbs_EDGE);
//	std::vector<myEdge> myEdges;
//	while (aEdgeExp.More())
//	{
//		TopoDS_Shape curentShape = aEdgeExp.Current();  // 遍历得到每一个边
//		if (curentShape.ShapeType() == TopAbs_EDGE)
//		{ // 检查是否为边
//			TopoDS_Edge anEdge = TopoDS::anEdge(curentShape);
//			// 在这里可以对 anEdge 进行操作
//			#pragma region 得到边的信息
//			BRepAdaptor_Curve aBAC = BRepAdaptor_Curve(anEdge);
//			// 曲线类型
//			if (aBAC.GetType() == GeomAbs_Line)
//			{
//				// 直线参数(直线长度)
//				Standard_Real start_param = aBAC.FirstParameter();
//				Standard_Real end_param = aBAC.LastParameter();
//				Standard_Real curve_length = end_param - start_param;
//				//两端点
//				gp_Pnt Point1 = aBAC.Value(start_param);
//				gp_Pnt Point2 = aBAC.Value(end_param);
//				//边的朝向
//				gp_Dir aDir = gp_Dir(gp_Vec(Point1, Point2));
//				myEdge aMyEdge = myEdge(anEdge, curve_length, Point1, Point2, aDir);
//				myEdges.push_back(aMyEdge);
//			}
//			#pragma endregion
//
//		}
//		else
//		{
//			// 如果不是边，则进行相应的处理
//		}
//		aEdgeExp.Next();
//
//	}
//	#pragma region 对边进行判别，找出符合的平行边
//	//移除不平行的边
//	myEdges.erase(std::remove_if(myEdges.begin(), myEdges.end(), RmoveParallelX), myEdges.end());
//	std::vector<myEdge> tempmyEdges;
//	for (size_t i = 0; i < myEdges.size(); i++)
//	{
//		gp_Dir theDir = gp_Dir(1, 0, 0);
//		if (myEdges[i].dir.IsParallel(theDir, 1e-2))
//		{
//			tempmyEdges.push_back(myEdges[i]);
//		}
//	}
//	myEdges = tempmyEdges;
//	//平行的边分为两组
//	// 双重循环遍历所有可能的组合
//	std::vector<myEdge> resultEdges;
//	for (size_t i = 0; i < myEdges.size(); ++i)
//	{
//		for (size_t i = i + 1; i < myEdges.size(); ++i)
//		{
//			if (IsEdgePiar(myEdges[i], myEdges[i]))
//			{
//				// 如果满足条件，保留Z更小的点
//				if (myEdges[i].point1.Z() < myEdges[i].point1.Z())
//				{
//					resultEdges.push_back(myEdges[i]);
//				}
//				else
//				{
//					resultEdges.push_back(myEdges[i]);
//				}
//			}
//		}
//	}
//	#pragma endregion
//	//取出点进行输出
//	std::vector<std::pair<gp_Pnt, gp_Pnt>> results;
//	for (size_t i = 0; i < resultEdges.size(); i++)
//	{
//		if (resultEdges[i].point1.Y() < resultEdges[i].point2.Y())
//		{
//			results.push_back(std::make_pair(resultEdges[i].point1, resultEdges[i].point2));
//		}
//		else
//		{
//			results.push_back(std::make_pair(resultEdges[i].point2, resultEdges[i].point1));
//		}
//	}
//	return results;
//}
///// <summary>
///// 遍历截面的线，知道用哪些点创建竖板Y
///// </summary>
///// <param name="theSection"></param>
///// <returns></returns>
//static std::vector<std::pair<gp_Pnt, gp_Pnt>> GetPointsFromSectionY(TopoDS_Shape theSection) {
//	TopExp_Explorer aEdgeExp = TopExp_Explorer(theSection, TopAbs_EDGE);
//	std::vector<myEdge> myEdges;
//	while (aEdgeExp.More())
//	{
//		TopoDS_Shape curentShape = aEdgeExp.Current();  // 遍历得到每一个边
//		if (curentShape.ShapeType() == TopAbs_EDGE)
//		{ // 检查是否为边
//			TopoDS_Edge anEdge = TopoDS::anEdge(curentShape);
//			// 在这里可以对 anEdge 进行操作
//			#pragma region 得到边的信息
//			BRepAdaptor_Curve aBAC = BRepAdaptor_Curve(anEdge);
//			// 曲线类型
//			if (aBAC.GetType() == GeomAbs_Line)
//			{
//				// 直线参数(直线长度)
//				Standard_Real start_param = aBAC.FirstParameter();
//				Standard_Real end_param = aBAC.LastParameter();
//				Standard_Real curve_length = end_param - start_param;
//				//两端点
//				gp_Pnt Point1 = aBAC.Value(start_param);
//				gp_Pnt Point2 = aBAC.Value(end_param);
//				//边的朝向
//				gp_Dir aDir = gp_Dir(gp_Vec(Point1, Point2));
//				myEdge aMyEdge = myEdge(anEdge, curve_length, Point1, Point2, aDir);
//				myEdges.push_back(aMyEdge);
//			}
//			#pragma endregion
//
//		}
//		else
//		{
//			// 如果不是边，则进行相应的处理
//		}
//		aEdgeExp.Next();
//
//	}
//	#pragma region 对边进行判别，找出符合的平行边
//	//移除不平行的边
//	myEdges.erase(std::remove_if(myEdges.begin(), myEdges.end(), RmoveParallelY), myEdges.end());
//	//平行的边分为两组
//	// 双重循环遍历所有可能的组合
//	std::vector<myEdge> resultEdges;
//	for (size_t i = 0; i < myEdges.size(); ++i)
//	{
//		for (size_t i = i + 1; i < myEdges.size(); ++i)
//		{
//			if (IsEdgePiar(myEdges[i], myEdges[i]))
//			{
//				// 如果满足条件，保留Z更小的点
//				if (myEdges[i].point1.Z() < myEdges[i].point1.Z())
//				{
//					resultEdges.push_back(myEdges[i]);
//				}
//				else
//				{
//					resultEdges.push_back(myEdges[i]);
//				}
//			}
//		}
//	}
//	#pragma endregion
//	//取出点进行输出
//	std::vector<std::pair<gp_Pnt, gp_Pnt>> results;
//	for (size_t i = 0; i < resultEdges.size(); i++)
//	{
//		if (resultEdges[i].point1.X() < resultEdges[i].point2.X())
//		{
//			results.push_back(std::make_pair(resultEdges[i].point1, resultEdges[i].point2));
//		}
//		else
//		{
//			results.push_back(std::make_pair(resultEdges[i].point2, resultEdges[i].point1));
//		}
//	}
//	return results;
//}
///// <summary>
///// 构造X方向的竖板
///// </summary>
///// <param name="theSection"></param>
///// <param name="theX"></param>
///// <param name="theY"></param>
///// <param name="theZ"></param>
///// <param name="theYLength"></param>
///// <param name="theTv"></param>
///// <param name="theH"></param>
///// <returns></returns>
//static TopoDS_Shape MakeVerticalPlateInX(TopoDS_Shape theSection, Standard_Real theX, Standard_Real theY, Standard_Real theZ, Standard_Real theYLength, Standard_Real theTv, Standard_Real theH) {
//	std::vector<std::pair<gp_Pnt, gp_Pnt>> pointPairs = GetPointsFromSectionX(theSection);
//	// 按照 pair 的第一个元素的 Y 值进行排序，使用 lambda 表达式
//	std::sort(pointPairs.begin(), pointPairs.end(), [](const auto& a, const auto& b) {
//		return a.first.Y() < b.first.Y();
//		});
//	Standard_Real startY = theY;
//	std::vector<TopoDS_Shape> verticalPlatesX;
//	while (startY < theYLength)
//	{
//		if (!pointPairs.empty())
//		{
//			auto first_pair = pointPairs.begin();
//			if (startY < first_pair->first.Y())
//			{
//				//构造连接处
//				gp_Pnt box_p1 = gp_Pnt(theX - theTv / 2, startY, theZ);
//				gp_Pnt box_p2 = gp_Pnt(first_pair->first.X() + theTv / 2, first_pair->first.Y(), theZ + theH);
//				TopoDS_Shape theConnection = BRepPrimAPI_MakeBox(box_p1, box_p2).Shape();
//				verticalPlatesX.push_back(theConnection);
//				//构造板
//				gp_Pnt box_p3 = gp_Pnt(first_pair->first.X() - theTv / 2, first_pair->first.Y(), theZ);
//				gp_Pnt box_p4 = gp_Pnt(first_pair->second.X() + theTv / 2, first_pair->second.Y(), first_pair->second.Z());
//				TopoDS_Shape theYPlate = BRepPrimAPI_MakeBox(box_p3, box_p4).Shape();
//				verticalPlatesX.push_back(theYPlate);
//				startY = first_pair->second.Y();
//			}
//			else
//			{
//				//构造板
//				gp_Pnt box_p1 = gp_Pnt(first_pair->first.X() - theTv / 2, first_pair->first.Y(), theZ);
//				gp_Pnt box_p2 = gp_Pnt(first_pair->second.X() + theTv / 2, first_pair->second.Y(), first_pair->second.Z());
//				TopoDS_Shape theBox = BRepPrimAPI_MakeBox(box_p1, box_p2).Shape();
//				verticalPlatesX.push_back(theBox);
//				startY = first_pair->second.Y();
//			}
//			// 弹出第一个元素
//			pointPairs.erase(pointPairs.begin());
//		}
//		else
//		{
//			//只构造连接处
//			gp_Pnt box_p1 = gp_Pnt(theX - theTv / 2, startY, theZ);
//			gp_Pnt box_p2 = gp_Pnt(theX + theTv / 2, theY + theYLength, theZ + theH);
//			TopoDS_Shape theBox = BRepPrimAPI_MakeBox(box_p1, box_p2).Shape();
//			verticalPlatesX.push_back(theBox);
//		}
//	}
//	TopoDS_Shape resultShape = verticalPlatesX[0];
//	for (size_t i = 1; i < verticalPlatesX.size(); i++)
//	{
//		BRepAlgoAPI_Fuse aFuse = BRepAlgoAPI_Fuse(resultShape, verticalPlatesX[i]);
//		aFuse.Build();
//		if (aFuse.IsDone())
//		{
//			resultShape = aFuse.Shape();
//		}
//	}
//	return resultShape;
//}
///// <summary>
///// 构造Y方向的竖板
///// </summary>
///// <param name="theSection"></param>
///// <param name="theX"></param>
///// <param name="theY"></param>
///// <param name="theZ"></param>
///// <param name="theXLength"></param>
///// <param name="theTv"></param>
///// <param name="theH"></param>
///// <returns></returns>
//static TopoDS_Shape MakeVerticalPlateInY(TopoDS_Shape theSection, Standard_Real theX, Standard_Real theY, Standard_Real theZ, Standard_Real theXLength, Standard_Real theTv, Standard_Real theH) {
//	std::vector<std::pair<gp_Pnt, gp_Pnt>> pointPairs = GetPointsFromSectionY(theSection);
//	// 按照 pair 的第一个元素的 Y 值进行排序，使用 lambda 表达式
//	std::sort(pointPairs.begin(), pointPairs.end(), [](const auto& a, const auto& b) {
//		return a.first.Y() < b.first.Y();
//		});
//	Standard_Real startX = theX;
//	std::vector<TopoDS_Shape> verticalPlatesY;
//	while (startX < theXLength)
//	{
//		if (!pointPairs.empty())
//		{
//			auto first_pair = pointPairs.begin();
//			if (startX < first_pair->first.X())
//			{
//				//构造连接处
//				gp_Pnt box_p1 = gp_Pnt(startX, theY - theTv / 2, theZ);
//				gp_Pnt box_p2 = gp_Pnt(first_pair->first.X(), theY + theTv / 2, theZ + theH);
//				TopoDS_Shape theConnection = BRepPrimAPI_MakeBox(box_p1, box_p2).Shape();
//				verticalPlatesY.push_back(theConnection);
//				//构造板
//				gp_Pnt box_p3 = gp_Pnt(first_pair->first.X(), first_pair->first.Y() - theTv / 2, theZ);
//				gp_Pnt box_p4 = gp_Pnt(first_pair->second.X(), first_pair->second.Y() + theTv / 2, first_pair->second.Z());
//				TopoDS_Shape theXPlate = BRepPrimAPI_MakeBox(box_p3, box_p4).Shape();
//				verticalPlatesY.push_back(theXPlate);
//				startX = first_pair->second.Y();
//			}
//			else
//			{
//				//构造板
//				gp_Pnt box_p1 = gp_Pnt(first_pair->first.X(), first_pair->first.Y() - theTv / 2, theZ);
//				gp_Pnt box_p2 = gp_Pnt(first_pair->second.X(), first_pair->second.Y() + theTv / 2, first_pair->second.Z());
//				TopoDS_Shape theBox = BRepPrimAPI_MakeBox(box_p1, box_p2).Shape();
//				verticalPlatesY.push_back(theBox);
//				startX = first_pair->second.Y();
//			}
//			// 弹出第一个元素
//			pointPairs.erase(pointPairs.begin());
//		}
//		else
//		{
//			//只构造连接处
//			gp_Pnt box_p1 = gp_Pnt(startX, theY - theTv / 2, theZ);
//			gp_Pnt box_p2 = gp_Pnt(theX + theXLength, theY + theTv / 2, theZ + theH);
//			TopoDS_Shape theBox = BRepPrimAPI_MakeBox(box_p1, box_p2).Shape();
//			verticalPlatesY.push_back(theBox);
//		}
//	}
//	TopoDS_Shape resultShape = verticalPlatesY[0];
//	for (size_t i = 1; i < verticalPlatesY.size(); i++)
//	{
//		BRepAlgoAPI_Fuse aFuse = BRepAlgoAPI_Fuse(resultShape, verticalPlatesY[i]);
//		aFuse.Build();
//		if (aFuse.IsDone())
//		{
//			resultShape = aFuse.Shape();
//		}
//	}
//	return resultShape;
//}
