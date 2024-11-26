#pragma once
#include<gp_TrsfForm.hxx>

namespace OCCTK {
namespace OCC {
namespace gp {

public enum class TrsfForm {
	Identity = gp_Identity,     // 无变换 (矩阵作为ID)
	Rotation = gp_Rotation,     // 旋转变换
	Translation = gp_Translation,  // 平移变换
	PntMirror = gp_PntMirror,    // 点对称
	Ax1Mirror = gp_Ax1Mirror,    // 轴对称
	Ax2Mirror = gp_Ax2Mirror,    // 平面对称
	Scale = gp_Scale,        // 缩放
	CompoundTrsf = gp_CompoundTrsf, // 组合变换
	Other = gp_Other         // 非正交矩阵变换
};

}
}
}