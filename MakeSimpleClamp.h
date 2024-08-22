#pragma once
#include "BRepAdaptor_Curve.hxx"
#include "BRepBuilderAPI_MakeEdge.hxx"
#include "BRepBuilderAPI_MakeFace.hxx"
#include "BRepBuilderAPI_MakeWire.hxx"
#include "TopoDS_Edge.hxx"
#include "TopoDS_Shape.hxx"
#include <BRep_Builder.hxx>
#include <BRepAlgoAPI_Cut.hxx>
#include <gp_Dir.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <TopoDS_Face.hxx>
#include <vector>

//! 所有板均无厚度

namespace OCCTK {
namespace SimpleClamp {

#pragma region Utility

public struct BasePlate {
	TopoDS_Shape TShape() {
		// 根据边界框尺寸创建立方体
		gp_Pnt theLowLeft(X - offsetX, Y - offsetY, Z);
		gp_Pnt theTopRight(X + dX + offsetX, Y + dY + offsetY, Z);

		gp_Pnt p1, p2, p3, p4;
		p1 = theLowLeft;
		p2 = gp_Pnt(theLowLeft.X(), theTopRight.Y(), Z);
		p3 = theTopRight;
		p4 = gp_Pnt(theTopRight.X(), theLowLeft.Y(), Z);
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
		if (mySlotShape.IsNull()) {
			TopoDS_Compound test;
			mySlotBuilder.MakeCompound(test);
			mySlotShape = test;
			return face.Shape();
		}
		else {
			return BRepAlgoAPI_Cut(face.Shape(), mySlotShape).Shape();
		}
	}
	double X;
	double Y;
	double Z;
	double dX;
	double dY;
	double offsetX;
	double offsetY;
	double offsetZ;
	double lowestZ;// 零件最低点
	double hight;// 零件高度
	TopoDS_Shape mySlotShape = TopoDS_Shape();
	BRep_Builder mySlotBuilder = BRep_Builder();
};

// 横截方向必须垂直于XY方向，Z值由底板决定
public class PlatePose {
public:
	PlatePose() {};
	PlatePose(const gp_Pnt thePnt, const gp_Dir theDir) { point = thePnt, dir = theDir; }
	gp_Pnt point;
	gp_Dir dir; // dir 与 pln 在同一平面
	gp_Pln Plane() {
		if (myPlant != nullptr) return *myPlant;
		myPlant = new gp_Pln(point, dir);
		return *myPlant;
	}
	// 拷贝构造函数
	PlatePose(const PlatePose& other) :point(other.point), dir(other.dir) {
		if (other.myPlant) {
			myPlant = other.myPlant;
		}
	}
	// 拷贝赋值运算符
	PlatePose& operator=(const PlatePose& other) {
		if (this != &other) {
			point = other.point;
			dir = other.dir;
		}
		return *this;
	}
private:
	gp_Pln* myPlant = nullptr;
};

public class myEdge {
public:
	myEdge() {};
	// 构造函数使用初始化列表
	myEdge(TopoDS_Edge theEdge)
		: edge(theEdge) // 初始化列表初始化 edge 和 dir
	{
		gp_Pnt a, b;
		BRepAdaptor_Curve aBAC(theEdge);
		start = aBAC.Value(aBAC.FirstParameter());
		end = aBAC.Value(aBAC.LastParameter());
		middle = aBAC.Value(aBAC.FirstParameter() + (aBAC.LastParameter() - aBAC.FirstParameter()) / 2);
	};
	gp_Pnt2d start2D() {
		return gp_Pnt2d(start.X(), start.Y());
	}
	gp_Pnt2d end2D() {
		return gp_Pnt2d(end.X(), end.Y());
	}

	TopoDS_Edge edge;
	gp_Pnt start;
	gp_Pnt end;
	gp_Pnt middle;
	bool dir = true;

	// 拷贝赋值操作符
	myEdge& operator=(const myEdge& other) {
		if (this != &other) {
			edge = other.edge;
			start = other.start;
			end = other.end;
			middle = other.middle;
			dir = other.dir;
		}
		return *this;
	}

	// 重载 operator== 操作符，用于比较两个 myEdge 对象是否相等
	bool operator==(const myEdge& other) const {
		return start.IsEqual(other.start, 1e-4) && end.IsEqual(other.end, 1e-4);
	}
};

using Ring = std::vector<myEdge>;

public struct VerticalPiece {
	VerticalPiece() {};
	VerticalPiece(PlatePose thePose, myEdge theMyEdge, double theZ) :pose(thePose), myEdge(theMyEdge), Z(theZ) {}

	//// 拷贝构造函数
	//VerticalPiece(const VerticalPiece& other)
	//	: pose(other.pose), myEdge(other.myEdge), Z(other.Z), myLength(other.myLength) {
	//	if (other.myShape) {
	//		myShape = new TopoDS_Shape(*other.myShape);
	//	}
	//	else {
	//		myShape = nullptr;
	//	}
	//}
	//// 拷贝赋值运算符
	//VerticalPiece& operator=(const VerticalPiece& other) {
	//	if (this != &other) {
	//		pose = other.pose;
	//		myEdge = other.myEdge;
	//		Z = other.Z;
	//		myLength = other.myLength;
	//		if (myShape) {
	//			delete myShape;
	//		}
	//		if (other.myShape) {
	//			myShape = new TopoDS_Shape(*other.myShape);
	//		}
	//		else {
	//			myShape = nullptr;
	//		}
	//	}
	//	return *this;
	//}

	PlatePose pose;
	myEdge myEdge;
	double Z;
	double order;// 用于记录排序的依据
	// 由 Edge 和 Z 值生成形状
	TopoDS_Shape* Shape() {
		if (myShape != nullptr) return myShape;
		gp_Pnt p2 = myEdge.start;
		gp_Pnt p3 = myEdge.end;
		gp_Pnt p1(p2.X(), p2.Y(), Z);
		gp_Pnt p4(p3.X(), p3.Y(), Z);
		BRepBuilderAPI_MakeWire aWireMk = BRepBuilderAPI_MakeWire();
		aWireMk.Add(BRepBuilderAPI_MakeEdge(p1, p2));
		aWireMk.Add(myEdge.edge);
		aWireMk.Add(BRepBuilderAPI_MakeEdge(p3, p4));
		aWireMk.Add(BRepBuilderAPI_MakeEdge(p4, p1));
		if (!aWireMk.IsDone()) {
			myShape = new TopoDS_Shape(myEdge.edge);
			return myShape;
		}
		BRepBuilderAPI_MakeFace PieceProfile = BRepBuilderAPI_MakeFace(aWireMk.Wire());
		if (!PieceProfile.IsDone()) {
			myShape = new TopoDS_Shape(myEdge.edge);
			return myShape; myShape = new TopoDS_Shape(myEdge.edge);
			return myShape;
		}
		myShape = new TopoDS_Shape(PieceProfile);
		return myShape;
	};
	double Length() const {
		if (myLength != -1.0) return myLength;
		gp_Pnt2d a(myEdge.start.X(), myEdge.start.Y());
		gp_Pnt2d b(myEdge.end.X(), myEdge.end.Y());
		myLength = a.Distance(b);
		return myLength;
	}
private:
	TopoDS_Shape* myShape = nullptr;
	mutable double myLength = -1.0;// 使用一个无效的默认值来表示长度尚未计算
};

public struct VerticalPlate {
public:
	TopoDS_Shape shape;
	TopoDS_Shape numberedShape;
	std::vector<VerticalPiece> pieces = std::vector<VerticalPiece>();
	PlatePose pose;
	gp_Pnt start;
	gp_Pnt end;
	double Z;
	double clearances;
	double minSupportLen;
	double cuttingDistance;
	double avoidanceHeight;
	double connectionThickness;// 板厚
	double slotLength = 6.0;//todo 暂不开放 连接槽长度
	double slotHight = 10.0;//todo 暂不开放 连接槽高度
	double filletRadius;
	std::vector<gp_Pnt> cutPoints;
	TCollection_AsciiString numberString = "unset";

	BRepBuilderAPI_MakeWire _unclosedWire;
};

///==============================Function==============================

// 根据theEdge和Z创建Piece
inline TopoDS_Face MakePiece(myEdge theEdge, double theZ) {
	gp_Pnt p1, p2, p3, p4;
	p1 = theEdge.start;
	p2 = theEdge.end;
	p3 = gp_Pnt(p2.X(), p2.Y(), theZ);
	p4 = gp_Pnt(p1.X(), p1.Y(), theZ);
	// 创建边
	TopoDS_Edge l1 = theEdge.edge;
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
	return face.Face();
}

#pragma endregion

///==============================
///生成底板
///==============================

BasePlate MakeBasePlate(TopoDS_Shape theWorkpiece, double theOffsetZ, double theOffsetX, double theOffsetY);

///==============================
///生成竖板
///==============================

VerticalPlate MakeVerticalPlate(TopoDS_Shape theWorkpiece, BasePlate theBasePlate, PlatePose theDir, double theClearances, double theMinSupportLen, double theCutDistance);

VerticalPlate SuturePiece(VerticalPlate& thePlate, const BasePlate theBase, double theConnectionHight, double theConnectionThickness);

VerticalPlate SlotVerticalPlate(VerticalPlate& thePlate, std::vector<VerticalPlate> otherPlates, double theFilletRadius, bool middleToDown);

BasePlate SlotBasePlate(BasePlate& theBasePlate, std::vector<VerticalPlate> middleDownPlates, std::vector<VerticalPlate> middleUpPlates);

VerticalPlate BrandNumberVerticalPlate(VerticalPlate thePlate, double hight);

VerticalPlate BrandNumberVerticalPlate(VerticalPlate thePlate, gp_Pnt aimPoint, double hight);

BasePlate BrandNumberBasePlate(BasePlate thePlate, double hight);

///==============================
///得到结果
///==============================

TopoDS_Shape DeployPlates(BasePlate theBasePlate, std::vector<VerticalPlate> middleToDownPlates, std::vector<VerticalPlate> middleToUpPlates);

}
}
