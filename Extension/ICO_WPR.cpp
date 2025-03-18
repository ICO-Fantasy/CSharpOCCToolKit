#include "..\OCCProxy\ICO_Quaternion.h"
#include "ICO_WPR.h"
#include <cmath>
#include <gp_EulerSequence.hxx>
#include <TopLoc_Location.hxx>

namespace OCCTK {
namespace OCC {
namespace gp {

/// <summary>
/// 以角度值创建旋转
/// </summary>
/// <param name="w"></param>
/// <param name="p"></param>
/// <param name="r"></param>
WPR::WPR(double w, double p, double r)
{
    myW = w;
    myP = p;
    myR = r;
}

System::Object^ WPR::Clone()
{
    return gcnew WPR(W, P, R);
}

System::String^ WPR::ToString()
{
    return System::String::Format("({0}, {1}, {2})", W, P, R);
}

WPR::operator Quat(WPR wpr)
{
    gp_Quaternion q;
    q.SetEulerAngles(gp_EulerSequence::gp_Extrinsic_XYZ, wpr.W / 180 * M_PI, wpr.P / 180 * M_PI, wpr.R / 180 * M_PI);
    return Quat(q.X(), q.Y(), q.Z(), q.W());
}

WPR::operator gp_Quaternion(WPR wpr)
{
    gp_Quaternion q;
    q.SetEulerAngles(gp_EulerSequence::gp_Extrinsic_XYZ, wpr.W / 180 * M_PI, wpr.P / 180 * M_PI, wpr.R / 180 * M_PI);
    return q;
}

}
}
}