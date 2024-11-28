using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using MathNet.Numerics.LinearAlgebra;
using MathNet.Numerics.LinearAlgebra.Double;
using MathNet.Spatial.Units;
using OCCTK.OCC.gp;

namespace TestWPF.Robotics;

public static class SO3Extensions
{
    public static Trsf ToTrsf(this Matrix<double> so3)
    {
        return new Trsf(so3.ToArray());
    }
}

[Flags]
public enum J5Cfg
{
    FLIP = 0,
    NOFLIP = 1,
}

[Flags]
public enum J4Cfg
{
    LEFT = 0,
    RIGHT = 1,
}

[Flags]
public enum J3Cfg
{
    UP = 0,
    DOWN = 1,
}

[Flags]
public enum J6Cfg
{
    FRONT = 0,
    BACK = 1,
}

[Flags]
public enum AxisCfg
{
    Zero = 0,
    PLUS = 1,
    MINUS = 3,
}

/// <summary>
/// 逆解姿态
/// </summary>
public class FanucRobotConfig(
    J5Cfg j5,
    J3Cfg j3,
    J6Cfg j6,
    J4Cfg j4 = 0,
    AxisCfg a1 = 0,
    AxisCfg a2 = 0,
    AxisCfg a3 = 0
)
{
    public J5Cfg J5 = j5;
    public J3Cfg J3 = j3;
    public J6Cfg J6 = j6;
    public J4Cfg J4 = j4;
    public AxisCfg A1 = a1;
    public AxisCfg A2 = a2;
    public AxisCfg A3 = a3;
}

public class Kinematics
{
    public static Matrix<double> DHParaToSO3(DHParameter DH)
    {
        // 使用 DH 参数计算变换矩阵
        double _theta = DH.Theta + DH.Offset; // 计算 θ
        double alpha = DH.Alpha; // α
        double d = DH.D; // d
        double a = DH.A; // a

        // 计算正弦和余弦值
        double s_t = Math.Sin(_theta);
        double c_t = Math.Cos(_theta);
        double s_a = Math.Sin(alpha);
        double c_a = Math.Cos(alpha);

        // 创建 4x4 变换矩阵
        Matrix<double> SO3 = DenseMatrix.OfArray(
            new double[,]
            {
                { c_t, (-s_t * c_a), (s_t * s_a), (a * c_t), },
                { s_t, (c_t * c_a), (-c_t * s_a), (a * s_t), },
                { 0, s_a, c_a, d, },
                { 0, 0, 0, 1 }
            }
        );

        return SO3;
    }

    public static Matrix<double> DHParaToTransfrom(DHParameter DH, double angle)
    {
        // 使用 DH 参数计算变换矩阵
        double _theta = angle + DH.Theta + DH.Offset; // 计算 θ
        double alpha = DH.Alpha; // α
        double d = DH.D; // d
        double a = DH.A; // a

        // 计算正弦和余弦值
        double s_t = Math.Sin(_theta);
        double c_t = Math.Cos(_theta);
        double s_a = Math.Sin(alpha);
        double c_a = Math.Cos(alpha);

        // 创建 4x4 变换矩阵
        Matrix<double> SO3 = DenseMatrix.OfArray(
            new double[,]
            {
                { c_t, (-s_t * c_a), (s_t * s_a), (a * c_t), },
                { s_t, (c_t * c_a), (-c_t * s_a), (a * s_t), },
                { 0, s_a, c_a, d, },
                { 0, 0, 0, 1 }
            }
        );

        return SO3;
    }
}
