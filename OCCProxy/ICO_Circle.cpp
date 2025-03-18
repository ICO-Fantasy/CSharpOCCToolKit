#include "ICO_Circle.h"
#include <gp_Circ.hxx>
//local
#include "ICO_Ax2.h"

namespace OCCTK {
namespace OCC {
namespace gp {

Circle::Circle(Ax2 pose, double radius) {
    pose = pose;
    radius = radius;
}

Circle::Circle(gp_Circ Circle) {
    pose = Ax2(Circle.Position());
    radius = Circle.Radius();
}

gp_Circ Circle::GetOCC() {
    return gp_Circ(Pose, Radius);
}

System::Object^ Circle::Clone() {
    return gcnew Circle(Pose, Radius);
}

}
}
}