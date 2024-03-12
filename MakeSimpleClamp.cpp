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
#include <map>
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
					result = static_cast<TopoDS_Shape>(aFace);
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
						result = static_cast<TopoDS_Shape>(aFace);
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
enum Direction {
	X,
	Y
};
/// <summary>
/// 创建拓扑平面
/// </summary>
/// <returns></returns>
static TopoDS_Face MakeVerticalPlane(Standard_Real thePointStart, Direction theDirection) {
	gp_Dir PlaneNnormal;
	gp_Pnt startPnt;
	switch (theDirection)
	{
	case X:
		PlaneNnormal = gp_Dir(1.0, 0.0, 0.0);
		startPnt = gp_Pnt(thePointStart, 0.0, 0);
		break;
	case Y:
		PlaneNnormal = gp_Dir(0.0, 1.0, 0.0);
		startPnt = gp_Pnt(0.0, thePointStart, 0);
		break;
	default:
		throw std::invalid_argument("Invalid direction. Only X or Y are supported.");
		break;
	}
	gp_Pln thePln = gp_Pln(startPnt, PlaneNnormal);
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
static TopoDS_Shape MakeVerticalPlateWithSection(std::pair<double, TopoDS_Shape> theSectionPair, Direction theDirection, std::map<double, TopoDS_Shape> otherSections, Standard_Real theX, Standard_Real theY, Standard_Real theZ, Standard_Real VerticalPlateConnectionHeight, Standard_Real VerticalPlateClearances, Standard_Real VerticalPlateCuttingDistance) {
	//todo首先生成板，再考虑连接
	struct localEdge {
		localEdge(TopoDS_Edge theEdge) {
			Edge = theEdge;
			BRepAdaptor_Curve aBAC(theEdge);
			startPoint = aBAC.Value(aBAC.FirstParameter());
			endPoint = aBAC.Value(aBAC.LastParameter());
		};
		TopoDS_Edge Edge;
		gp_Pnt startPoint;
		gp_Pnt endPoint;
	};
	std::vector<localEdge>TempEdges;
	std::vector<std::vector<gp_Pnt>>Rings;
	//首先判断有几个环，对环进行处理
	TopExp_Explorer aEdgeExp = TopExp_Explorer(theSectionPair.second, TopAbs_EDGE);
	while (aEdgeExp.More())
	{
		localEdge anEdge(TopoDS::Edge(aEdgeExp.Current()));// 遍历得到每一个边
		aEdgeExp.Next();
		TempEdges.push_back(anEdge);
	}
	while (not TempEdges.empty())
	{
		localEdge anEdge = TempEdges.back();
		TempEdges.pop_back();
		for (size_t i = 0; i < TempEdges.size(); i++)
		{

		}

	}

	//todo长度相等的线段配为一对(存在问题)
	//VerticalPlateConnectionHeight, VerticalPlateClearances, VerticalPlateCuttingDistance
	switch (theDirection)
	{
	case X:
		break;
	case Y:
		break;
	default:
		throw std::invalid_argument("Invalid direction. Only X or Y are supported.");
		break;
	}
}

///==============================
///生成竖板 (需求变更，以下代码废弃)
///==============================
//// 定义一个结构体
//struct myEdge {
//	TopoDS_Edge topo;
//	Standard_Real len;
//	gp_Pnt point1;
//	gp_Pnt point2;
//	gp_Dir dir;
//};
//static bool RmoveParallelX(myEdge aEdge) {
//	gp_Dir theDir = gp_Dir(1, 0, 0);
//	return aEdge.dir.IsParallel(theDir, 1e-2);
//}
//static bool RmoveParallelY(myEdge aEdge) {
//	gp_Dir theDir = gp_Dir(0, 1, 0);
//	return aEdge.dir.IsParallel(theDir, 1e-2);
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
////std::vector<std::pair<gp_Pnt, gp_Pnt>> GetPointsFromSection(TopoDS_Shape theSection, void (*func)(int))
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
//			TopoDS_Edge aEdge = TopoDS::Edge(curentShape);
//			// 在这里可以对 anEdge 进行操作
//			#pragma region 得到边的信息
//			BRepAdaptor_Curve aBAC = BRepAdaptor_Curve(aEdge);
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
//				myEdge aMyEdge = myEdge(aEdge, curve_length, Point1, Point2, aDir);
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
//		for (size_t j = i + 1; j < myEdges.size(); ++j)
//		{
//			if (IsEdgePiar(myEdges[i], myEdges[j]))
//			{
//				// 如果满足条件，保留Z更小的点
//				if (myEdges[i].point1.Z() < myEdges[j].point1.Z())
//				{
//					resultEdges.push_back(myEdges[i]);
//				}
//				else
//				{
//					resultEdges.push_back(myEdges[j]);
//				}
//			}
//		}
//	}
//	#pragma endregion
//	//取出点进行输出
//	std::vector<std::pair<gp_Pnt, gp_Pnt>> result;
//	for (size_t i = 0; i < resultEdges.size(); i++)
//	{
//		if (resultEdges[i].point1.Y() < resultEdges[i].point2.Y())
//		{
//			result.push_back(std::make_pair(resultEdges[i].point1, resultEdges[i].point2));
//		}
//		else
//		{
//			result.push_back(std::make_pair(resultEdges[i].point2, resultEdges[i].point1));
//		}
//	}
//	return result;
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
//			TopoDS_Edge aEdge = TopoDS::Edge(curentShape);
//			// 在这里可以对 anEdge 进行操作
//			#pragma region 得到边的信息
//			BRepAdaptor_Curve aBAC = BRepAdaptor_Curve(aEdge);
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
//				myEdge aMyEdge = myEdge(aEdge, curve_length, Point1, Point2, aDir);
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
//		for (size_t j = i + 1; j < myEdges.size(); ++j)
//		{
//			if (IsEdgePiar(myEdges[i], myEdges[j]))
//			{
//				// 如果满足条件，保留Z更小的点
//				if (myEdges[i].point1.Z() < myEdges[j].point1.Z())
//				{
//					resultEdges.push_back(myEdges[i]);
//				}
//				else
//				{
//					resultEdges.push_back(myEdges[j]);
//				}
//			}
//		}
//	}
//	#pragma endregion
//	//取出点进行输出
//	std::vector<std::pair<gp_Pnt, gp_Pnt>> result;
//	for (size_t i = 0; i < resultEdges.size(); i++)
//	{
//		if (resultEdges[i].point1.X() < resultEdges[i].point2.X())
//		{
//			result.push_back(std::make_pair(resultEdges[i].point1, resultEdges[i].point2));
//		}
//		else
//		{
//			result.push_back(std::make_pair(resultEdges[i].point2, resultEdges[i].point1));
//		}
//	}
//	return result;
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
