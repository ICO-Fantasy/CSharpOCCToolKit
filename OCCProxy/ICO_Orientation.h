#pragma once
#include<TopAbs_Orientation.hxx>

namespace OCCTK {
namespace OCC {
namespace TopoAbs {

public enum class Orientation {
	FORWARD = TopAbs_Orientation::TopAbs_FORWARD,
	REVERSED = TopAbs_Orientation::TopAbs_REVERSED,
	INTERNAL = TopAbs_Orientation::TopAbs_INTERNAL,
	EXTERNAL = TopAbs_Orientation::TopAbs_EXTERNAL
};

}
}
}
