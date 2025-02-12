#include "ICO_WPR.h"

namespace OCCTK {
namespace OCC {
namespace gp {

WPR::WPR(double w, double p, double r)
{
    w = W;
    p = P;
    r = R;
}

System::Object^ WPR::Clone()
{
    return gcnew WPR(W, P, R);
}

System::String^ WPR::ToString()
{
    return System::String::Format("({0}, {1}, {2})", W, P, R);
}

}
}
}