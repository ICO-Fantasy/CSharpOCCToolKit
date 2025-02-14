#pragma once

/// <summary>
/// 默认精度1e-4
/// </summary>
static double LINEAR_TOL = 1e-4;
/// <summary>
/// 默认精度0.001度
/// </summary>
static double ANGULAR_TOL = 0.0000174533;

namespace OCCTK {
namespace OCC {

public ref class Precision
{
private:
    // 私有构造函数，防止外部实例化
    Precision() {}
private:
    static property double LINEAR {double get() { return LINEAR_TOL; }}
    static property double ANGULAR {double get() { return ANGULAR_TOL; }}
};

}
}