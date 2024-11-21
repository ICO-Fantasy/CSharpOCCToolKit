using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using MathNet.Numerics.LinearAlgebra;
using MathNet.Numerics.LinearAlgebra.Double;
using MathNet.Spatial.Units;

namespace TestWPF.Robotics;

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
