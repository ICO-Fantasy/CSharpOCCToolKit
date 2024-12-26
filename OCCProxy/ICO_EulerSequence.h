#pragma once
#include <gp_EulerSequence.hxx>

namespace OCCTK {
namespace OCC {
namespace gp {

public enum class EulerSequence {
	//! Classic Euler angles, alias to Intrinsic_ZXZ
	EulerAngles = gp_EulerSequence::gp_EulerAngles,

	//! Yaw Pitch Roll (or nautical) angles, alias to Intrinsic_ZYX
	YawPitchRoll = gp_EulerSequence::gp_YawPitchRoll,

	// Tait-Bryan angles (using three different axes)
	Extrinsic_XYZ = gp_EulerSequence::gp_Extrinsic_XYZ,
	Extrinsic_XZY = gp_EulerSequence::gp_Extrinsic_XZY,
	Extrinsic_YZX = gp_EulerSequence::gp_Extrinsic_YZX,
	Extrinsic_YXZ = gp_EulerSequence::gp_Extrinsic_YXZ,
	Extrinsic_ZXY = gp_EulerSequence::gp_Extrinsic_ZXY,
	Extrinsic_ZYX = gp_EulerSequence::gp_Extrinsic_ZYX,

	Intrinsic_XYZ = gp_EulerSequence::gp_Intrinsic_XYZ,
	Intrinsic_XZY = gp_EulerSequence::gp_Intrinsic_XZY,
	Intrinsic_YZX = gp_EulerSequence::gp_Intrinsic_YZX,
	Intrinsic_YXZ = gp_EulerSequence::gp_Intrinsic_YXZ,
	Intrinsic_ZXY = gp_EulerSequence::gp_Intrinsic_ZXY,
	Intrinsic_ZYX = gp_EulerSequence::gp_Intrinsic_ZYX,
	WPR = Intrinsic_XYZ,

	// Proper Euler angles (using two different axes, first and third the same)
	Extrinsic_XYX = gp_EulerSequence::gp_Extrinsic_XYX,
	Extrinsic_XZX = gp_EulerSequence::gp_Extrinsic_XZX,
	Extrinsic_YZY = gp_EulerSequence::gp_Extrinsic_YZY,
	Extrinsic_YXY = gp_EulerSequence::gp_Extrinsic_YXY,
	Extrinsic_ZYZ = gp_EulerSequence::gp_Extrinsic_ZYZ,
	Extrinsic_ZXZ = gp_EulerSequence::gp_Extrinsic_ZXZ,

	Intrinsic_XYX = gp_EulerSequence::gp_Intrinsic_XYX,
	Intrinsic_XZX = gp_EulerSequence::gp_Intrinsic_XZX,
	Intrinsic_YZY = gp_EulerSequence::gp_Intrinsic_YZY,
	Intrinsic_YXY = gp_EulerSequence::gp_Intrinsic_YXY,
	Intrinsic_ZXZ = gp_EulerSequence::gp_Intrinsic_ZXZ,
	Intrinsic_ZYZ = gp_EulerSequence::gp_Intrinsic_ZYZ
};

}
}
}