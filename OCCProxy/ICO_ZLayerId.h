#pragma once
#include <Graphic3d_ZLayerId.hxx>
namespace OCCTK {
namespace OCC {
namespace Graphic3d {

public enum class ZLayerId {
	UNKNOWN = Graphic3d_ZLayerId_UNKNOWN, //!< 无效值
	Default = Graphic3d_ZLayerId_Default,  //!< 默认值
	Top = Graphic3d_ZLayerId_Top, //!< 用于三维演示的覆盖层，继承了之前 ZLayer 的深度
	Topmost = Graphic3d_ZLayerId_Topmost, //!< 用于 3D 演示的独立深度覆盖层
	TopOSD = Graphic3d_ZLayerId_TopOSD, //!< 视图显示的顶层
	BotOSD = Graphic3d_ZLayerId_BotOSD   //!< 视图显示的底层
};

}
}
}