﻿#pragma once
#include <TopAbs_ShapeEnum.hxx>
//包装C++类到托管类
#include <NCollection_Haft.h> 
namespace OCCTK {
namespace OCC {
namespace TopoAbs {



public ref class WTopAbs_ShapeEnum {
public:
	enum class Value {
		COMPOUND = TopAbs_ShapeEnum::TopAbs_COMPOUND,
		COMPSOLID = TopAbs_ShapeEnum::TopAbs_COMPSOLID,
		SOLID = TopAbs_ShapeEnum::TopAbs_SOLID,
		SHELL = TopAbs_ShapeEnum::TopAbs_SHELL,
		FACE = TopAbs_ShapeEnum::TopAbs_FACE,
		WIRE = TopAbs_ShapeEnum::TopAbs_WIRE,
		EDGE = TopAbs_ShapeEnum::TopAbs_EDGE,
		VERTEX = TopAbs_ShapeEnum::TopAbs_VERTEX,
		SHAPE = TopAbs_ShapeEnum::TopAbs_SHAPE
	};

	WTopAbs_ShapeEnum(Value aValue) : value(aValue) {}

	// Allow switch and comparisons.
	operator Value() { return value; }

	bool operator==(WTopAbs_ShapeEnum^ other) {
		if (other == nullptr) return false;
		return value == other->value;
	}

	bool operator!=(WTopAbs_ShapeEnum^ other) {
		return !(*this == other);
	}

	TopAbs_ShapeEnum GetOCC() {
		return static_cast<TopAbs_ShapeEnum>(value);
	}

private:
	Value value;
};


}
}
}

