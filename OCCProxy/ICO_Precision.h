#pragma once

/// <summary>
/// Ĭ�Ͼ���1e-4
/// </summary>
static double LINEAR_TOL = 1e-4;
/// <summary>
/// Ĭ�Ͼ���0.001��
/// </summary>
static double ANGULAR_TOL = 0.0000174533;

namespace OCCTK {
namespace OCC {

public ref class Precision
{
private:
    // ˽�й��캯������ֹ�ⲿʵ����
    Precision() {}
private:
    static property double LINEAR {double get() { return LINEAR_TOL; }}
    static property double ANGULAR {double get() { return ANGULAR_TOL; }}
};

}
}