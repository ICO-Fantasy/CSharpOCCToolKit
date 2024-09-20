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
#include <BRepFeat_SplitShape.hxx>
#include <BRepAlgoAPI_Splitter.hxx>

static const double LINEAR_TOLERANCE = 0.2;

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
	double tx1 = 0.0;
	double ty1 = 0.0;
	double tx2 = 0.0;
	double ty2 = 0.0;
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
static bool AddEdge(Ring& theWire, std::vector<myEdge>& Edges, gp_Pnt& start, gp_Pnt& end, bool originDir) {
	for (size_t i = 0; i < Edges.size(); ++i) {
		myEdge oneEdge = Edges[i];
		if (end.Distance(oneEdge.start) < LINEAR_TOLERANCE) {
			if (oneEdge.dir == originDir) {
				end = oneEdge.end;
				theWire.push_back(oneEdge);
				Edges.erase(Edges.begin() + i);
				return true;
			}
		}
		if (start.Distance(oneEdge.end) < LINEAR_TOLERANCE) {
			if (oneEdge.dir == originDir) {
				start = oneEdge.start;
				theWire.insert(theWire.begin(), oneEdge);
				Edges.erase(Edges.begin() + i);
				return true;
			}
		}
	}
	return false;
}

//对Pieces排序
static std::vector<VerticalPiece> SortingPieces(const PlatePose& thePose, std::vector<VerticalPiece>& thePieces) {
	// 设置start为左边点
	gp_Trsf tempswapT;
	tempswapT.SetRotation(gp_Ax1(thePose.point, gp_Dir(0, 0, 1)), thePose.dir.AngleWithRef(gp_Vec(0.0, 1.0, 0.0), gp_Vec(0.0, 0.0, 1.0)));
	for (size_t i = 0; i < thePieces.size(); i++) {
		auto thePiece = &thePieces[i];
		gp_Pnt tempStart = thePiece->myEdge.start.Transformed(tempswapT);
		gp_Pnt tempEnd = thePiece->myEdge.end.Transformed(tempswapT);
		//对Edge的两端点排序
		if (tempStart.X() > tempEnd.X()) {
			std::swap(thePiece->myEdge.start, thePiece->myEdge.end);
			std::swap(tempStart, tempEnd);
		}
		thePiece->order = tempStart.X();
	}
	//// 对Edges重新排序(start更小)
	//std::sort(orderedPieces.begin(), orderedPieces.end(),
	//	[&](const myEdge& edge1, const myEdge& edge2) {
	//		return edge1.start.Transformed(tempswapT).X() < edge2.start.Transformed(tempswapT).X();
	//	});

	// 对Edges重新排序(start更小)
	std::sort(thePieces.begin(), thePieces.end(),
		[&](const VerticalPiece& piece1, const VerticalPiece& piece2) {
			return piece1.order < piece2.order;
		});
	return thePieces;
}

// 找到并返回一个环
static Ring FindARing(std::vector<myEdge>& Edges) {

	Ring theRing;
	theRing.push_back(Edges.front());
	gp_Pnt start = Edges.front().start;
	gp_Pnt end = Edges.front().end;
	Edges.erase(Edges.begin());

	//! debug
	std::vector<double> debug_dis;

	// 找到匹配的边，并删除该边
	while (!Edges.empty()) {
		bool endFlag = true;
		for (size_t i = 0; i < Edges.size(); ++i) {
			myEdge oneEdge = Edges[i];
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

	////! debug
	//std::vector<std::pair<gp_Pnt2d, gp_Pnt2d>>debug1;
	//for (auto a : theRing) {
	//	debug1.push_back({ a.start2D(),a.end2D() });
	//}
	//std::vector<std::pair<gp_Pnt2d, gp_Pnt2d>>debug2;
	//for (auto a : Edges) {
	//	debug2.push_back({ a.start2D(),a.end2D() });
	//}


	return theRing;
}

// 从截面中划分出环
static std::vector<Ring> GetRings(std::vector<myEdge>& edges) {
	//std::vector<Ring>Rings;
	//Ring tempEdges = Edges;
	//while (!tempEdges.empty()) {
	//	Rings.push_back(FindARing(tempEdges));
	//}
	//return Rings;
	std::vector<std::vector<myEdge>> rings;
	bool continue_flag = true;
	gp_Pnt start, end;  // 将start和end的声明移到这里

	while (!edges.empty()) {
		if (continue_flag) {
			myEdge an_edge = edges.front();
			edges.erase(edges.begin());
			start = an_edge.start;  // 这里可以访问start和end
			end = an_edge.end;
			std::vector<myEdge> a_ring = { an_edge };
			rings.push_back(a_ring);
			continue_flag = false;
		}
		bool end_for = true;
		for (auto it = edges.begin(); it != edges.end(); ++it) {
			if (start.Distance(it->start) < LINEAR_TOLERANCE) {
				rings.back().push_back(*it);
				end = it->end;
				edges.erase(it);
				end_for = false;
				break;
			}
			else if (start.Distance(it->end) < LINEAR_TOLERANCE) {
				rings.back().push_back(*it);
				start = it->start;
				edges.erase(it);
				end_for = false;
				break;
			}
			else if (end.Distance(it->end) < LINEAR_TOLERANCE) {
				rings.back().push_back(*it);
				end = it->start;
				edges.erase(it);
				end_for = false;
				break;
			}
			else if (end.Distance(it->start) < LINEAR_TOLERANCE) {
				rings.back().push_back(*it);
				end = it->end;
				edges.erase(it);
				end_for = false;
				break;
			}
		}
		if (end_for) {
			continue_flag = true;
		}

	}

	return rings;
}

// 把一个环划分为上下两边，取下端的边
static std::vector<myEdge> SplitRing(Ring theRing) {
	// 错误处理，理论上 Ring 不应该少于4条边
	if (theRing.size() < 4) {
		return theRing;
	}

	std::vector<myEdge> originRing = theRing;

	//对线排序
	std::vector<myEdge> orderedEdges;
	TopTools_ListOfShape edgeList;
	BRepBuilderAPI_MakeWire wireMaker = BRepBuilderAPI_MakeWire();
	for (size_t i = 0; i < originRing.size(); i++) { edgeList.Append(originRing[i].edge); }

	gp_Vec2d baseVec2d;
	for (size_t i = 0; i < originRing.size(); i++) {
		baseVec2d = gp_Vec2d(originRing[i].start2D(), originRing[i].end2D());
		if (baseVec2d.Magnitude() != 0.0) {
			break;
		}
		if (i == originRing.size() - 1) {
			return originRing;//没有错误处理
		}
	}

	////! debug
	//std::vector<gp_Vec2d> debug_vec;
	//std::vector<double> debug_vec_mag;
	//std::vector <std::pair< double, double >> debug_vec_start;
	//std::vector <std::pair< gp_Pnt2d, gp_Pnt2d >> debug_vec_start2;

	wireMaker.Add(edgeList);
	if (!wireMaker.IsDone()) {
		return theRing;
	}
	BRepTools_WireExplorer wireExplorer(wireMaker);
	while (wireExplorer.More()) {
		myEdge anEdge(wireExplorer.Current());
		// 对Edge两端点重新排序
		gp_Pnt theStart = BRep_Tool::Pnt(wireExplorer.CurrentVertex());
		if (anEdge.start.Distance(theStart) > anEdge.end.Distance(theStart)) { std::swap(anEdge.start, anEdge.end); }
		//debug_vec_start2.push_back({ anEdge.start2D() , anEdge.end2D() });
		// 获取线的相对方向
		gp_Vec2d edgeVec2d = gp_Vec2d(anEdge.start2D(), anEdge.end2D());
		if (edgeVec2d.Magnitude() != 0.0) {
			if (edgeVec2d.IsOpposite(baseVec2d, 1e-2)) { anEdge.dir = false; }
			else { anEdge.dir = true; }
		}
		orderedEdges.push_back(anEdge);
		wireExplorer.Next();
		//debug_vec.push_back(edgeVec2d);
		//debug_vec_mag.push_back(edgeVec2d.Magnitude());
	}
	int debug_trueCount = 0;
	int debug_falseCount = 0;

	for (auto d : orderedEdges) {
		if (d.dir) {
			debug_trueCount += 1;
		}
		else {
			debug_falseCount += 1;
		}
	}


	// 构建第一条线
	std::vector<myEdge> firstWire, secondWire;
	gp_Pnt startP = orderedEdges.front().start;
	gp_Pnt endP = orderedEdges.front().end;
	bool originDir = orderedEdges.front().dir;
	firstWire.push_back(orderedEdges.front());
	orderedEdges.erase(orderedEdges.begin());
	while (AddEdge(firstWire, orderedEdges, startP, endP, originDir)) {}

	// 直接取剩余线作为第二条
	secondWire = orderedEdges;

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

	if ((!firstWire.empty()) && (!secondWire.empty())) {
		//! 使用两端的 Z 值比较比较高度(可能不准确,由于斜着的边缘）
		for (size_t i = 0; i < firstWire.size(); ++i) {
			const myEdge& firstEdge = firstWire[i];
			bool isSame = false;
			for (size_t j = 0; j < secondWire.size(); ++j) {
				const myEdge& secondEdge = secondWire[j];
				gp_Pnt2d a(firstEdge.middle.X(), firstEdge.middle.Y());
				gp_Pnt2d b(secondEdge.middle.X(), secondEdge.middle.Y());

				if (a.Distance(b) < 3.0) {
					isSame = true;
				}

				if (isSame) {
					if (firstEdge.middle.Z() < secondEdge.middle.Z()) {
						return firstWire;
					}
					else {
						return secondWire;
					}
				}
			}
		}

		if (firstWire.front().middle.Z() < secondWire.front().middle.Z()) {
			return firstWire;
		}
		else {
			return secondWire;
		}
	}
	else {
		if (!firstWire.empty()) {
			return firstWire;
		}
		if (!secondWire.empty()) {
			return secondWire;
		}
	}
	//不应该出现执行到此处的情况
	return theRing;
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
		else if (text == 'A') {
			digits.push_back(44);
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
	//theBasePlate.lowestZ = zMin;
	theBasePlate.hight = zMax - zMin;
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
	return theBaseplate;
}

#pragma endregion

#pragma region 生成竖板

// 得到切割后的工件平面
static TopoDS_Shape MakeSection(PlatePose thePose, const TopoDS_Shape& theWorkpiece) {
	BRepAlgoAPI_Splitter aSplitter;
	TopTools_ListOfShape aListOfArguments, aListOfTools;
	aListOfArguments.Append(theWorkpiece);
	aListOfTools.Append(BRepBuilderAPI_MakeFace(thePose.Plane()));
	aSplitter.SetArguments(aListOfArguments);
	aSplitter.SetTools(aListOfTools);
	aSplitter.SetFuzzyValue(0.1);
	aSplitter.Build();

	TopoDS_Compound test;
	BRep_Builder bb;
	bb.MakeCompound(test);
	//bb.Add(test, BRepBuilderAPI_MakeFace(thePose.Plane(), -5, 5, -5, 5));
	TopTools_ListOfShape edges = aSplitter.SectionEdges();
	TopTools_ListIteratorOfListOfShape it(edges);
	for (; it.More(); it.Next()) {
		bb.Add(test, it.Value());
	}
	//TopoDS_Shape aSplitShape = aSplitter.Shape();
	//return aSplitShape;
	return test;

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
		myEdge anEdge(TopoDS::Edge(aEdgeExp.Current()));// 遍历得到每一个边
		aEdgeExp.Next();
		TempEdges.push_back(anEdge);
	}
	Rings = GetRings(TempEdges);
	//! 每个环中取最下边的线段作为原始构造线
	std::vector<myEdge>allEdges;
	std::vector<VerticalPiece>finalPieces;
	for (Ring aRing : Rings) {
		std::vector<myEdge> bottomEdges = SplitRing(aRing);
		for (myEdge anEdge : bottomEdges) {
			//VerticalPiece anPiece(thePose, anEdge, theZ);
			allEdges.push_back(anEdge);
		}
	}
	// 检查是否存在分离失败的情况
	gp_Trsf theT;
	theT.SetRotation(gp_Ax1(thePose.point, gp_Dir(0, 0, 1)), thePose.dir.AngleWithRef(gp_Vec(0.0, 1.0, 0.0), gp_Vec(0.0, 0.0, 1.0)));

	for (size_t i = 0; i < allEdges.size(); i++) {
		myEdge Edge_i = allEdges[i];
		bool collision = false;
		gp_Pnt Start_i = Edge_i.start.Transformed(theT);
		gp_Pnt Middle_i = Edge_i.middle.Transformed(theT);
		gp_Pnt End_i = Edge_i.end.Transformed(theT);
		for (size_t j = 0; j < allEdges.size(); j++) {
			if (i == j) { continue; }
			myEdge Edge_j = allEdges[j];
			gp_Pnt Start_j = Edge_j.start.Transformed(theT);
			gp_Pnt Middle_j = Edge_j.middle.Transformed(theT);
			gp_Pnt End_j = Edge_j.end.Transformed(theT);
			double localTol = 3.0;
			if (Start_i.X() - localTol <= Middle_j.X() && Middle_j.X() <= End_i.X() + localTol) {
				if (Middle_i.Z() > Middle_j.Z()) {
					collision = true;
				}
			}
			if (Start_j.X() - localTol <= Middle_i.X() && Middle_i.X() <= End_j.X() + localTol) {
				if (Middle_i.Z() > Middle_j.Z()) {
					collision = true;
				}
			}
		}
		if (!collision) {
			VerticalPiece aPiece(thePose, Edge_i, theZ);
			finalPieces.push_back(aPiece);
		}
	}
	thePalte.pieces = finalPieces;

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
		if (std::abs(ratio) > 1e-2) {
			p1.SetX(p1.X() + (p2.X() - p1.X()) * ratio);
			p1.SetY(p1.Y() + (p2.Y() - p1.Y()) * ratio);

			p2.SetX(p2.X() + (p1.X() - p2.X()) * ratio);
			p2.SetY(p2.Y() + (p1.Y() - p2.Y()) * ratio);

			TopoDS_Edge outEdge = TrimEdge(aPiece.myEdge.edge, p1, p2);
			if (!outEdge.IsNull()) {
				tempPieces.push_back(VerticalPiece(aPiece.pose, myEdge(outEdge), aPiece.Z));
			}
		}
		else {
			//不做改变
			tempPieces.push_back(aPiece);
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

//切开过长边
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
		myEdge newmyEdge(newEdge);
		//newEdge.Orientation(thePiece.myEdge.edge.Orientation());//设置线的方向为同向

		VerticalPiece newPiece(thePiece.pose, newmyEdge, thePiece.Z);
		result.push_back(newPiece);
		currentNum += 1.0;
	}

	return result;
}

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
	//result.shape = aSection;
	//return result;
	result = MakeVerticalPieceWithSection(result, aSection);
	result = TrimEdgeEnds(result);
	result = RemoveShortEdge(result);
	result = CutLongEdge(result);
	// 把最后得到的片体排序
	result.pieces = SortingPieces(thePose, result.pieces);

	return result;
}

// 合并片体为一块板
VerticalPlate SuturePiece(VerticalPlate& thePlate, const BasePlate theBase, double theAvoidanceHeight, double theConnectionThickness) {
	if (thePlate.pieces.empty()) { return thePlate; }
	//! 板宽度不包含 Offset 的部分
	thePlate.avoidanceHeight = theAvoidanceHeight;
	double AH = theAvoidanceHeight;
	thePlate.connectionThickness = theConnectionThickness;
	const double Z = thePlate.Z;
	Ring tempEdges;

	for (auto anPiece : SortingPieces(thePlate.pose, thePlate.pieces)) {
		tempEdges.push_back(anPiece.myEdge);
	}

#pragma region 线段生成

	gp_Pnt start_p0 = tempEdges.front().start;
	start_p0.SetZ(Z);
	gp_Pnt end_p0 = tempEdges.back().end;
	end_p0.SetZ(Z);

	//! 线段生成逻辑见飞书
	BRepBuilderAPI_MakeWire wire;
	gp_Pnt lastP = start_p0;
	for (size_t i = 0; i < tempEdges.size(); i++) {
		gp_Pnt p0 = tempEdges[i].start;
		p0.SetZ(tempEdges[i].middle.Z() - AH);
		gp_Pnt p1 = tempEdges[i].start;
		gp_Pnt p2 = tempEdges[i].end;
		gp_Pnt p3 = tempEdges[i].end;
		p3.SetZ(tempEdges[i].middle.Z() - AH);
		// 首段线
		if (i == 0) {
			wire.Add(BRepBuilderAPI_MakeEdge(lastP, p1));
		}
		else {
			if (tempEdges[i].start.Distance(tempEdges[i - 1].end) > 0.5) {
				wire.Add(BRepBuilderAPI_MakeEdge(lastP, p0));
				wire.Add(BRepBuilderAPI_MakeEdge(p0, p1));
			}
		}
		wire.Add(tempEdges[i].edge);
		//末段线
		if (i == tempEdges.size() - 1) {
			wire.Add(BRepBuilderAPI_MakeEdge(p2, end_p0));
		}
		else {
			if (tempEdges[i].end.Distance(tempEdges[i + 1].start) > 0.5) {
				wire.Add(BRepBuilderAPI_MakeEdge(p2, p3));
			}
		}
		lastP = p3;
	}
#pragma endregion
	//合并后更新Plate的首末端
	thePlate.start = start_p0;
	thePlate.end = end_p0;
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
		if (x3 > x4) { std::swap(x3, x4); }
		if (y3 > y4) { std::swap(y3, y4); }
		//避开边缘，防止切出去
		double cx, cy;
		if (Get2DLineIntersection(x1, y1, x2, y2, x3, y3, x4, y4, cx, cy)) {
			if (x1 - TOL <= cx && cx <= x2 + TOL && y1 - TOL <= cy && cy <= y2 + TOL) {
				if (x3 - TOL <= cx && cx <= x4 + TOL && y3 - TOL <= cy && cy <= y4 + TOL) {
					gp_Pnt cutPoint = gp_Pnt(cx, cy, Z);
					cutPoints.push_back(cutPoint);
				}
			}
		}
	}
	//按距离end从小到大排序，从end开始连接回start
	std::sort(cutPoints.begin(), cutPoints.end(), [&](const gp_Pnt& p1, const gp_Pnt& p2) {
		return p1.Distance(thePlate.end) < p2.Distance(thePlate.end);
		});
	std::sort(thePlate.cutPoints.begin(), thePlate.cutPoints.end(), [&](const gp_Pnt& p1, const gp_Pnt& p2) {
		return p1.Distance(thePlate.start) < p2.Distance(thePlate.start);
		});
	// 如果没用相交点，则添加辅助板
	if (cutPoints.empty()) {
		thePlate.singlePlate = true;
		cutPoints.push_back(gp_Pnt((thePlate.start.XYZ() + thePlate.end.XYZ()) / 2));
	}
	//用于后续烙印数字
	thePlate.cutPoints = cutPoints;

	double slotL = thePlate.slotLength / 2;
	double slotH = thePlate.slotHight;
	double ct = thePlate.connectionThickness / 2;
	gp_Pnt lastP = thePlate.end;
	// 在每个位置切槽
	gp_Vec plateDir(thePlate.start, thePlate.end);
	plateDir.Normalize();
	for (gp_Pnt aPnt : cutPoints) {
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
	}
	// 如果没有其它竖板相交

	thePlate._unclosedWire.Add(BRepBuilderAPI_MakeEdge(lastP, thePlate.start));
	////! debug
	//thePlate.shape = thePlate._unclosedWire.Shape();
	//return thePlate;
	// 
	////! debug
	//TopoDS_Compound testShape;
	//BRep_Builder b;
	//b.MakeCompound(testShape);
	//thePlate.shape = testShape;
	if (thePlate._unclosedWire.IsDone()) {
		TopoDS_Shape finalShape = BRepBuilderAPI_MakeFace(thePlate._unclosedWire).Shape();
		//b.Add(testShape, finalShape);
		// 还需要额外处理切除
		if (!middleToDown) {
			double ZC = thePlate.Z + thePlate.slotHight;
			// 创建一个圆柱 (从中间往上）
			for (gp_Pnt aPnt : cutPoints) {
				gp_Pnt cutPnt = aPnt;
				cutPnt.SetZ(ZC);
				TopoDS_Shape theSlot = BRepPrimAPI_MakeCylinder(gp_Ax2(cutPnt, gp_Dir(0.0, 0.0, 1.0)), ct, 999.0).Shape();
				finalShape = BRepAlgoAPI_Cut(finalShape, theSlot).Shape();
				//b.Add(testShape, theSlot);
			}
		}
		thePlate.shape = finalShape;
		////! debug
		//thePlate.shape = testShape;
	}

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

	// 执行倒圆角
	//if (middleToDown) {
	//	BRepFilletAPI_MakeFillet2d filleter(TopoDS::Face(thePlate.shape));
	//	BRepTools_WireExplorer wireExplorer(thePlate._unclosedWire.Wire());
	//	while (wireExplorer.More()) {
	//		TopoDS_Vertex vertex = wireExplorer.CurrentVertex();
	//		filleter.AddFillet(vertex, theFilletRadius);
	//		wireExplorer.Next();
	//	}
	//	if (filleter.IsDone()) {
	//		thePlate.shape = filleter.Shape();
	//	}
	//}
	return thePlate;
}

//添加辅助板
VerticalPlate AddSupportPlate(VerticalPlate thePlate, bool middleToDown) {
	if (thePlate.cutPoints.empty()) {
		throw std::exception("切割点为空");
	}
	VerticalPlate auxiliaryPlate;
	auxiliaryPlate.cutPoints = thePlate.cutPoints;
	auxiliaryPlate.Z = thePlate.Z;
	BRepBuilderAPI_MakeWire wireMaker;
	gp_Vec plateDir = gp_Vec(thePlate.pose.dir);
	auxiliaryPlate.pose = PlatePose(thePlate.cutPoints[0], thePlate.pose.dir.Crossed(gp_Dir(0, 0, -1)));
	if (middleToDown) {
		auto s = thePlate.numberString;
		s.InsertBefore(1, "Y");
		auxiliaryPlate.numberString = s;
	}
	else {
		auto s = thePlate.numberString;
		s.InsertBefore(1, "X");
		auxiliaryPlate.numberString = s;
	}
	auxiliaryPlate.connectionThickness = thePlate.connectionThickness;
	gp_Pnt aPnt = thePlate.cutPoints[0];
	double slotL = thePlate.slotLength / 2;
	double slotH = thePlate.slotHight;
	double ct = thePlate.connectionThickness / 2;
	gp_Trsf Th;
	Th.SetTranslation(gp_Vec(0, 0, thePlate.auxiliaryHight));

	gp_Pnt r1 = aPnt;
	gp_Trsf Tr1;
	Tr1.SetTranslation(plateDir.Multiplied(thePlate.auxiliaryWidth));
	r1.Transform(Tr1);
	auxiliaryPlate.end = r1;

	gp_Pnt rh = r1;
	rh.Transform(Th);

	gp_Pnt r2 = aPnt;
	gp_Trsf Tr2;
	Tr2.SetTranslation(plateDir.Multiplied(slotL));
	r2.Transform(Tr2);

	gp_Pnt r3 = aPnt;
	gp_Trsf Tr3;
	Tr3.SetTranslation(plateDir.Multiplied(slotL).Added(gp_Vec(0, 0, -ct * 2)));
	r3.Transform(Tr3);

	gp_Pnt l1 = aPnt;
	gp_Trsf Tl1;
	Tl1.SetTranslation(plateDir.Multiplied(-slotL).Added(gp_Vec(0, 0, -ct * 2)));
	l1.Transform(Tl1);

	gp_Pnt l2 = aPnt;
	gp_Trsf Tl2;
	Tl2.SetTranslation(plateDir.Multiplied(-slotL));
	l2.Transform(Tl2);

	gp_Pnt l3 = aPnt;
	gp_Trsf Tl3;
	Tl3.SetTranslation(plateDir.Multiplied(-thePlate.auxiliaryWidth));
	l3.Transform(Tl3);
	auxiliaryPlate.start = l3;

	gp_Pnt lh = l3;
	lh.Transform(Th);

	//为了能顺利烙印数字
	gp_Pnt tempStart = auxiliaryPlate.start;
	tempStart.SetZ(thePlate.Z + thePlate.auxiliaryHight);
	gp_Pnt tempEnd = auxiliaryPlate.end;
	tempEnd.SetZ(thePlate.Z + thePlate.auxiliaryHight);
	VerticalPiece auxPiece(auxiliaryPlate.pose, myEdge(BRepBuilderAPI_MakeEdge(tempStart, tempEnd)), auxiliaryPlate.Z);
	auxiliaryPlate.pieces.push_back(auxPiece);
	if (middleToDown) {
		// 从中间往上
		gp_Pnt topP = aPnt.Transformed(Th);

		gp_Pnt p1 = topP;
		gp_Trsf Tp1;
		Tp1.SetTranslation(plateDir.Multiplied(-ct));
		p1.Transform(Tp1);

		gp_Pnt p2 = aPnt;
		gp_Trsf Tp2;
		Tp2.SetTranslation(plateDir.Multiplied(-ct).Added(gp_Vec(0, 0, slotH)));
		p2.Transform(Tp2);

		gp_Pnt p3 = aPnt;
		gp_Trsf Tp3;
		Tp3.SetTranslation(plateDir.Multiplied(ct).Added(gp_Vec(0, 0, slotH)));
		p3.Transform(Tp3);

		gp_Pnt p4 = topP;
		gp_Trsf Tp4;
		Tp4.SetTranslation(plateDir.Multiplied(ct));
		p4.Transform(Tp4);

		wireMaker.Add(BRepBuilderAPI_MakeEdge(lh, p1));
		wireMaker.Add(BRepBuilderAPI_MakeEdge(p1, p2));
		wireMaker.Add(BRepBuilderAPI_MakeEdge(p2, p3));
		wireMaker.Add(BRepBuilderAPI_MakeEdge(p3, p4));
		wireMaker.Add(BRepBuilderAPI_MakeEdge(p4, rh));

		if (BRepBuilderAPI_MakeEdge(r2, r3).IsDone()) {
			wireMaker.Add(BRepBuilderAPI_MakeEdge(rh, r1));
			wireMaker.Add(BRepBuilderAPI_MakeEdge(r1, r2));
			wireMaker.Add(BRepBuilderAPI_MakeEdge(r2, r3));
		}
		else {
			wireMaker.Add(BRepBuilderAPI_MakeEdge(rh, r3));
		}
		wireMaker.Add(BRepBuilderAPI_MakeEdge(r3, l1));
		if (BRepBuilderAPI_MakeEdge(l2, l3).IsDone()) {
			wireMaker.Add(BRepBuilderAPI_MakeEdge(l1, l2));
			wireMaker.Add(BRepBuilderAPI_MakeEdge(l2, l3));
			wireMaker.Add(BRepBuilderAPI_MakeEdge(l3, lh));
		}
		else {
			wireMaker.Add(BRepBuilderAPI_MakeEdge(l1, lh));
		}
	}
	else {
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

		wireMaker.Add(BRepBuilderAPI_MakeEdge(lh, rh));
		if (BRepBuilderAPI_MakeEdge(r2, r3).IsDone()) {
			wireMaker.Add(BRepBuilderAPI_MakeEdge(rh, r1));
			wireMaker.Add(BRepBuilderAPI_MakeEdge(r1, r2));
			wireMaker.Add(BRepBuilderAPI_MakeEdge(r2, r3));
		}
		else {
			wireMaker.Add(BRepBuilderAPI_MakeEdge(rh, r3));
		}
		wireMaker.Add(BRepBuilderAPI_MakeEdge(r3, p3));
		wireMaker.Add(BRepBuilderAPI_MakeEdge(p3, p4));
		wireMaker.Add(BRepBuilderAPI_MakeEdge(p4, p5));
		wireMaker.Add(BRepBuilderAPI_MakeEdge(p5, p6));
		wireMaker.Add(BRepBuilderAPI_MakeEdge(p6, l1));
		if (BRepBuilderAPI_MakeEdge(l2, l3).IsDone()) {
			wireMaker.Add(BRepBuilderAPI_MakeEdge(l1, l2));
			wireMaker.Add(BRepBuilderAPI_MakeEdge(l2, l3));
			wireMaker.Add(BRepBuilderAPI_MakeEdge(l3, lh));
		}
		else {
			wireMaker.Add(BRepBuilderAPI_MakeEdge(l1, lh));
		}
	}
	auxiliaryPlate.auxiliary = true;
	auxiliaryPlate.shape = BRepBuilderAPI_MakeFace(wireMaker).Shape();
	return auxiliaryPlate;
}

// 构建数字 Face (默认在原点沿着XOZ平面正方向构建)
static TopoDS_Compound MakeNumber(TCollection_AsciiString numberString, double scaleRatio, double spacing, bool XOY = false) {
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
	// Axis
	numberMap[44] = {
		{10.0, 10.0},
		{68.0, 10.0},
		{68.0, 0.0},
		{78.0, 15.0},
		{68.0, 30.0},
		{68.0, 20.0},
		{57.0, 20.0},
		{47.0, 50.0},
		{57.0, 80.0},
		{47.0, 80.0},
		{42.0, 65.0},
		{37.0, 80.0},
		{27.0, 80.0},
		{37.0, 50.0},
		{27.0, 20.0},
		{20.0, 20.0},
		{20.0, 91.0},
		{50.0, 91.0},
		{80.0, 81.0},
		{80.0, 91.0},
		{65.0, 96.0},
		{80.0, 101.0},
		{80.0, 111.0},
		{50.0, 101.0},
		{20.0, 101.0},
		{20.0, 110.0},
		{30.0, 110.0},
		{15.0, 120.0},
		{0.0, 110.0},
		{10.0, 110.0},
	};
	TopoDS_Compound result;
	BRep_Builder builder;
	builder.MakeCompound(result);
	double offset = (spacing + 30.0);

	auto numbers = GetNumberString(numberString);
	if (XOY) {
		for (size_t i = 0; i < numbers.size(); ++i) {
			int currentIndex = numbers[i];
			// 开始构建
			BRepBuilderAPI_MakeWire wireMaker;
			std::array<double, 3> lastValue{ 999.0,999.0,999.0 };
			for (std::array<double, 2> p : numberMap[currentIndex]) {
				if (std::all_of(lastValue.begin(), lastValue.end(), [](double v) { return v == 999.0; })) {
					lastValue[0] = p[0] + i * offset;
					lastValue[1] = p[1];
					lastValue[2] = 0.0;
					continue;
				}
				std::array<double, 3> value{ p[0] + i * offset , p[1], 0.0 };

				wireMaker.Add(BRepBuilderAPI_MakeEdge(gp_Pnt(lastValue[0] * scaleRatio, lastValue[1] * scaleRatio, 0.0), gp_Pnt(value[0] * scaleRatio, value[1] * scaleRatio, 0.0)));
				lastValue = value;
			}
			// 最后连接
			std::array<double, 3>end_value{ numberMap[currentIndex][0][0] + i * offset ,numberMap[currentIndex][0][1],0.0 };
			wireMaker.Add(BRepBuilderAPI_MakeEdge(gp_Pnt(lastValue[0] * scaleRatio, lastValue[1] * scaleRatio, 0.0), gp_Pnt(end_value[0] * scaleRatio, end_value[1] * scaleRatio, 0.0)));
			// 构造面
			BRepBuilderAPI_MakeFace faceMaker(wireMaker.Wire());
			builder.Add(result, faceMaker.Face());
		}
		return result;
	}
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

// 在底板上开槽并烙印编号
BasePlate SlotBasePlate(BasePlate& theBasePlate, std::vector<VerticalPlate> middleDownPlates, std::vector<VerticalPlate> middleUpPlates) {
	if (theBasePlate.Shape().IsNull()) {
		return theBasePlate;
	}
	double TOL = 1e-2;
	//std::vector<gp_Pnt> cutPoints;

	////! test
	//TopoDS_Compound test;
	//BRep_Builder builder;
	//builder.MakeCompound(test);

	for (size_t i = 0; i < middleDownPlates.size(); ++i) {
		int lasti = 999;
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
					if (x3 - TOL <= cx && cx <= x4 + TOL && y3 - TOL <= cy && cy <= y4 + TOL) {
						double slotLen = downPlate.slotLength / 2 + 0.5;
						gp_Pnt currentPnt = gp_Pnt(cx, cy, theBasePlate.Z - 1);

#pragma region 开连接槽

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

						theBasePlate.mySlotBuilder.Add(theBasePlate.mySlotShape, prismer.Shape());

#pragma endregion

#pragma region 烙印编号
						double scaleFactor = 0.1;

						//X方向
						TCollection_AsciiString numberStringX = downPlate.numberString;
						TopoDS_Compound originFaceX = MakeNumber(numberStringX, scaleFactor, 10.0, true);
						// 平移
						gp_Trsf transTX;
						gp_Pnt aimPntX = gp_Pnt(currentPnt.X() + downPlate.slotLength / 2, currentPnt.Y() + downPlate.slotLength / 2, currentPnt.Z());
						transTX.SetTranslation(gp_Pnt(), aimPntX);
						// 往后(Z)走2，相当于前后各1
						BRepPrimAPI_MakePrism prismerNumX(originFaceX, gp_Vec(0, 0, 2));
						BRepBuilderAPI_Transform transformedFaceX = BRepBuilderAPI_Transform(prismerNumX.Shape(), transTX);
						theBasePlate.mySlotBuilder.Add(theBasePlate.mySlotShape, transformedFaceX.Shape());

						//Y方向
						TCollection_AsciiString numberStringY = upPlate.numberString;
						TopoDS_Compound originFaceY = MakeNumber(numberStringY, scaleFactor, 10.0, true);
						// 平移
						gp_Trsf transTY;
						gp_Pnt aimPntY = gp_Pnt(currentPnt.X() + downPlate.slotLength / 2, currentPnt.Y() - downPlate.slotLength / 2 - 60.0 * scaleFactor, currentPnt.Z());
						transTY.SetTranslation(gp_Pnt(), aimPntY);
						// 往后(Z)走2，相当于前后各1
						BRepPrimAPI_MakePrism prismerNumY(originFaceY, gp_Vec(0, 0, 2));
						BRepBuilderAPI_Transform transformedFaceY = BRepBuilderAPI_Transform(prismerNumY.Shape(), transTY);
						theBasePlate.mySlotBuilder.Add(theBasePlate.mySlotShape, transformedFaceY.Shape());

#pragma endregion
					}
				}
			}
		}
	}
	return theBasePlate;
}

// 在板上烙印数字（在指定点）
VerticalPlate BrandNumberVerticalPlate(VerticalPlate thePlate, gp_Pnt aimPoint, double hight = 60.0) {
	if (thePlate.pieces.empty()) {
		return thePlate;
	}
	double scaleFactor = hight / 60.0;
	// todo 字符间的间距暂不开放
	double spacing = 10.0;

	//左下角在原点的数字，沿着XOZ平面正方向构建
	TopoDS_Compound originFace = MakeNumber(thePlate.numberString, 1.0, spacing);

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

	BRepBuilderAPI_Transform transformedFace = BRepBuilderAPI_Transform(prismer.Shape(), transT.Multiplied(rotT.Multiplied(scaleT)));

	BRepAlgoAPI_Cut cutter(thePlate.shape, transformedFace.Shape());

	thePlate.numberedShape = cutter.Shape();

	////! debug
	//TopoDS_Compound testShape;
	//BRep_Builder b;
	//b.MakeCompound(testShape);
	//b.Add(testShape, thePlate.shape);
	//b.Add(testShape, transformedFace);
	//thePlate.numberedShape = testShape;

	return thePlate;
}

// 在板上烙印数字（默认位置）
VerticalPlate BrandNumberVerticalPlate(VerticalPlate thePlate, double hight = 60.0) {
	double width = 30.0;//默认宽度为30.0
	double neededLen = ((double)GetNumberString(thePlate.numberString).size() * (width + 10.0) + thePlate.connectionThickness) * (hight / 60.0) + 10.0;
	if (!thePlate.cutPoints.empty()) {
		gp_Pnt lastP = thePlate.cutPoints[0];
		for (size_t i = 1; i < thePlate.cutPoints.size(); i++) {
			double dis = lastP.Distance(thePlate.cutPoints[i]);
			std::cout << "Distance: " << dis << std::endl;
			if (lastP.Distance(thePlate.cutPoints[i]) > neededLen) {
				gp_Trsf tempT;
				tempT.SetTranslation(gp_Vec(thePlate.start, thePlate.end).Normalized().Multiplied(5.0));
				gp_Pnt aimPnt = lastP.Transformed(tempT);
				aimPnt.SetZ(thePlate.Z + thePlate.slotHight + 5.0);
				return BrandNumberVerticalPlate(thePlate, aimPnt, hight);
			}
			lastP = thePlate.cutPoints[i];
		}
	}
	//都失败的话在起点烙印
	gp_Trsf tempT;
	tempT.SetTranslation(gp_Vec(thePlate.start, thePlate.end).Normalized().Multiplied(5.0));
	gp_Pnt aimPnt = thePlate.start.Transformed(tempT);
	aimPnt.SetZ(thePlate.Z + thePlate.slotHight + 5.0);
	return BrandNumberVerticalPlate(thePlate, aimPnt, hight);
}

// 在底板上烙印XY指示
BasePlate BrandNumberBasePlate(BasePlate thePlate, double hight) {
	if (thePlate.Shape().IsNull()) {
		return thePlate;
	}

	TCollection_AsciiString numberString = "A";
	double scaleFactor = hight / 60.0;

	TopoDS_Compound originFace = MakeNumber(numberString, 1.0, 10.0, true);
	//往前(Z)挪1(为了烙印板)
	gp_Trsf temp;
	temp.SetTranslation(gp_Pnt(), gp_Pnt(0, 0, -1));
	BRepBuilderAPI_Transform theFace(originFace, temp);
	// 缩放
	gp_Trsf scaleT;
	scaleT.SetScaleFactor(scaleFactor);
	// 平移
	gp_Trsf transT;
	gp_Pnt aimPnt = gp_Pnt(thePlate.X - thePlate.offsetX + 1.0, thePlate.Y - thePlate.offsetY + 1.0, thePlate.Z);
	transT.SetTranslation(gp_Pnt(), aimPnt);
	// 往后(Z)走2，相当于前后各1
	BRepPrimAPI_MakePrism prismer(theFace, gp_Vec(0, 0, 2));

	BRepBuilderAPI_Transform transformedFace = BRepBuilderAPI_Transform(prismer.Shape(), transT.Multiplied(scaleT));

	thePlate.mySlotBuilder.Add(thePlate.mySlotShape, transformedFace.Shape());

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

	TopoDS_Shape newBP = theBasePlate.Shape().Moved(TopLoc_Location(baseR.Multiplied(base_trsf)), true);
	builder.Add(result, newBP);

#pragma endregion

#pragma region 竖板

	double rightX = 0.0;
	double rightY = 0.0;
	// X方向
	for (size_t i = 0; i < middleToDownPlates.size(); ++i) {
		VerticalPlate thePlate = middleToDownPlates[i];
		// 首先全部旋转到XOZ平面
		gp_Trsf toOriginR;
		toOriginR.SetRotation(gp_Ax1(thePlate.pose.point, gp_Dir(0, 0, 1)), thePlate.pose.dir.AngleWithRef(gp_Vec(0.0, 1.0, 0.0), gp_Vec(0.0, 0.0, 1.0)));
		BRepBuilderAPI_Transform toOriginRshape(thePlate.numberedShape, toOriginR);
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
	// Y方向
	for (size_t i = 0; i < middleToUpPlates.size(); ++i) {
		VerticalPlate thePlate = middleToUpPlates[i];
		// 首先全部旋转到XOZ平面
		gp_Trsf toOriginR;
		toOriginR.SetRotation(gp_Ax1(thePlate.pose.point, gp_Dir(0, 0, 1)), thePlate.pose.dir.AngleWithRef(gp_Vec(0.0, 1.0, 0.0), gp_Vec(0.0, 0.0, 1.0)));
		BRepBuilderAPI_Transform toOriginRshape(thePlate.numberedShape, toOriginR);
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