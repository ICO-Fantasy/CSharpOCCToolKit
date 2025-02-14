#pragma once
#include <Precision.hxx>

static double LINEAR_TOL = Precision::Confusion();
static double ANGULAR_TOL = Precision::Angular();

namespace OCCTK {
namespace OCC {

ref class Precision
{
private:
    // 私有构造函数，防止外部实例化
    Precision();
private:
    static property double LINEAR {double get() { return LINEAR_TOL; }}
    static property double ANGULAR {double get() { return ANGULAR_TOL; }}
};

}
}