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
/// �����ֵ�����Ԫ��
/// </summary>
/// <param name="percentage">��ֵ�İٷֱ�</param>
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