#include "ICO_SLERP.h"
#include <gp_Quaternion.hxx>
#include <gp_QuaternionSLerp.hxx>
//local
#include "ICO_Quaternion.h"

namespace OCCTK {
namespace OCC {
namespace gp {

SlerpCalculator::SlerpCalculator(Quat start, Quat end)
{
    mySlerpCalculator = new gp_QuaternionSLerp(start, end);
}

/// <summary>
/// 计算插值后的四元数
/// </summary>
/// <param name="percentage">插值的百分比</param>
/// <returns></returns>
Quat SlerpCalculator::Interpolate(double percentage)
{
    gp_Quaternion result;
    mySlerpCalculator->Interpolate(percentage, result);
    return Quat(result);
}

}
}
}