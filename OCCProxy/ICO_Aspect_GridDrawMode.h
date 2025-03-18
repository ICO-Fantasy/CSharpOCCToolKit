#pragma once
#include<Aspect_GridDrawMode.hxx>
namespace OCCTK {
namespace OCC {
namespace Aspect {

/// <summary>
/// 网格的绘制模式
/// </summary>
public enum class GridDrawMode {
	Lines = Aspect_GDM_Lines,
	Points = Aspect_GDM_Points,
	None = Aspect_GDM_None
};

}
}
}