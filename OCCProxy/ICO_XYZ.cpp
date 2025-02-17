#include "ICO_XYZ.h"
#include <gp_XYZ.hxx>

namespace OCCTK {
namespace OCC {
namespace gp {

XYZ::XYZ(double x, double y, double z)
{
    X = x;
    Y = y;
    Z = z;
}

XYZ::XYZ(gp_XYZ xyz)
{
    X = xyz.X();
    Y = xyz.Y();
    Z = xyz.Z();
}

gp_XYZ XYZ::GetOCC()
{
    return gp_XYZ(X, Y, Z);
}

System::Object^ XYZ::Clone()
{
    return gcnew XYZ(X, Y, Z);
}

System::String^ XYZ::ToString()
{
    return System::String::Format("{0}, {1}, {2}", X, Y, Z);
}

}
}
}