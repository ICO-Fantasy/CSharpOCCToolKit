﻿#pragma once
#include <TopAbs_ShapeEnum.hxx>
//包装C++类到托管类
#include <NCollection_Haft.h> 

namespace OCCTK {
namespace OCC {
namespace TopoAbs {

public enum class ShapeEnum {
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

}
}
}

