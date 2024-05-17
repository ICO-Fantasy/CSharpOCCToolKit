#pragma once
#include "BRepAdaptor_Curve.hxx"
#include "BRepBuilderAPI_MakeEdge.hxx"
#include "BRepBuilderAPI_MakeFace.hxx"
#include "BRepBuilderAPI_MakeWire.hxx"
#include "GC_MakeSegment.hxx"
#include "TopoDS_Edge.hxx"
#include "TopoDS_Shape.hxx"
#include <TopoDS_Face.hxx>
#include <vector>

//! 所有板均无厚度

namespace OCCTK {
namespace SimpleClamp {
#pragma region Utility

enum Direction {
	X,
	Y
};
struct BasePlate {
	TopoDS_Shape shape;
	double X;
	double Y;
	double Z;
	double dX;
	double dY;
	double OffsetX;
	double OffsetY;
};
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
struct VerticalPiece {
	Direction dir;
	TopoDS_Shape Shape;
	TopoDS_Edge Edge;
	gp_Pnt StartPoint() const {
		BRepAdaptor_Curve aBAC(Edge);
		gp_Pnt startP = aBAC.Value(aBAC.FirstParameter());
		gp_Pnt endP = aBAC.Value(aBAC.LastParameter());
		switch (dir) {
		case X:
			if (startP.Y() < endP.Y()) {
				return startP;
			}
			else {
				return endP;
			}
			break;
		case Y:
			if (startP.X() < endP.X()) {
				return startP;
			}
			else {
				return endP;
			}
			break;
		default:
			throw std::runtime_error("Unexpected Direction in switch");
		}
	}
	gp_Pnt EndPoint() const {
		BRepAdaptor_Curve aBAC(Edge);
		gp_Pnt startP = aBAC.Value(aBAC.FirstParameter());
		gp_Pnt endP = aBAC.Value(aBAC.LastParameter());
		switch (dir) {
		case X:
			if (startP.Y() > endP.Y()) {
				return startP;
			}
			else {
				return endP;
			}
			break;
		case Y:
			if (startP.X() > endP.X()) {
				return startP;
			}
			else {
				return endP;
			}
			break;
		default:
			throw std::runtime_error("Unexpected Direction in switch");
		}
	}
	Standard_Real Length() {
		double len;
		switch (dir) {
		case X:
			len = this->EndPoint().Y() - this->StartPoint().Y();
			return len;
			break;
		case Y:
			len = this->EndPoint().X() - this->StartPoint().X();
			return len;
			break;
		default:
			throw std::runtime_error("Unexpected Direction in switch");
		}
	}
};
struct VerticalPlate
{
	TopoDS_Shape shape;
	std::vector<VerticalPiece> pieces;
	Direction dir;
	double location;
	double clearances;
	double minSupportLen;
	double cuttiDistance;
	double connectionHeight;
};
///==============================Function==============================
//todo 函数不正常工作，拆分成其它函数(2024-05-17)
///// <summary>
///// 以逆时针方向，根据三个角点构建片体
///// </summary>
///// <param name="p1">左下角点</param>
///// <param name="p2"></param>
///// <param name="p3">右上角点</param>
///// /// <returns></returns>
//static TopoDS_Face MakePiece(gp_Pnt p1, gp_Pnt p2, gp_Pnt p3) {
//	/*p2 p3
//	p1  p4*/
//	//先取两个相邻点，得到其中心点
//	gp_Pnt middlePnt((p1.X() + p2.X()) / 2, (p1.Y() + p2.Y()) / 2, (p1.Z() + p2.Z()) / 2);
//	//得到矩形中心
//	gp_Pnt centerPnt((p1.X() + p3.X()) / 2, (p1.Y() + p3.Y()) / 2, (p1.Z() + p3.Z()) / 2);
//	//得到对称平面
//	gp_Ax2 mirrorFace = gp_Ax2(middlePnt, gp_Dir(gp_Vec(p1, p2)));
//	gp_Pnt p4 = p3.Mirrored(mirrorFace);
//	//! p4一定是右下角点，如果不是，交换p3、p4
//	if (gp_Vec(centerPnt, p3).Dot(gp_Vec(centerPnt, p4)) < 0) {
//		std::swap(p3, p4);
//	}
//	// 创建边
//	BRepBuilderAPI_MakeEdge l1(GC_MakeSegment(p1, p2).Value());
//	BRepBuilderAPI_MakeEdge l2(GC_MakeSegment(p2, p3).Value());
//	BRepBuilderAPI_MakeEdge l3(GC_MakeSegment(p3, p4).Value());
//	BRepBuilderAPI_MakeEdge l4(GC_MakeSegment(p4, p1).Value());
//	// 创建线段
//	BRepBuilderAPI_MakeWire wire;
//	wire.Add(l1.Edge());
//	wire.Add(l2.Edge());
//	wire.Add(l3.Edge());
//	wire.Add(l4.Edge());
//	// 创建面
//	BRepBuilderAPI_MakeFace face(wire.Wire());
//	return face.Face();
//}
#pragma endregion

///==============================
///生成底板
///==============================

BasePlate MakeBasePlate(TopoDS_Shape theWorkpiece, double theOffsetZ, double theOffsetX, double theOffsetY);

///==============================
///生成竖板
///==============================

VerticalPlate MakeVerticalPlate(TopoDS_Shape theWorkpiece, BasePlate theBasePlate, Direction theDir, double theLoc, double theClearances, double theMinSupportLen, double theCuttiDistance);
void SuturePiece(VerticalPlate& thePlate, BasePlate theBasePlate, double theConnectionHight);
void Slotting(VerticalPlate& thePlate, BasePlate theBasePlate, std::vector<double> theLocations, double theConnectionHight, double theConnectWidth);
}
}
