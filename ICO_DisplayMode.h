#pragma once
#include<AIS_DisplayMode.hxx>
namespace OCCTK {
namespace OCC {
namespace AIS {

public enum class DisplayMode {
	WireFrame = AIS_DisplayMode::AIS_WireFrame,
	Shaded = AIS_DisplayMode::AIS_Shaded,
};

}
}
}