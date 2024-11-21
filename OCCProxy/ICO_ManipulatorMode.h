#pragma once
#include<AIS_ManipulatorMode.hxx>

namespace OCCTK {
namespace OCC {
namespace AIS {

public enum class ManipulatorMode {
	None = AIS_ManipulatorMode::AIS_MM_None,
	Translation = AIS_ManipulatorMode::AIS_MM_Translation,
	Rotation = AIS_ManipulatorMode::AIS_MM_Rotation,
	Scaling = AIS_ManipulatorMode::AIS_MM_Scaling,
	TranslationPlane = AIS_ManipulatorMode::AIS_MM_TranslationPlane,
};

}
}
}