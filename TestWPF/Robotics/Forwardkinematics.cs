using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using MathNet.Numerics.LinearAlgebra;
using MathNet.Numerics.LinearAlgebra.Double;

namespace TestWPF.Robotics;

public class Forwardkinematics
{
    public static (
        Matrix<double>,
        Matrix<double>,
        Matrix<double>,
        Matrix<double>,
        Matrix<double>,
        Matrix<double>
    ) ForwardTransform6R(List<DHParameter> DHParameters, List<double> angles)
    {
        if (DHParameters.Count != 6 || angles.Count != 6)
        {
            throw new Exception($"机器人轴数{DHParameters.Count}，需要6；输入角度数{angles.Count}，需要6");
        }
        return FK6R(DHParameters, angles);
    }

    private static (
        Matrix<double>,
        Matrix<double>,
        Matrix<double>,
        Matrix<double>,
        Matrix<double>,
        Matrix<double>
    ) FK6R(List<DHParameter> DHParameters, List<double> angles)
    {
        Matrix<double> T01 = Kinematics.DHParaToTransfrom(DHParameters[0], angles[0]);
        Matrix<double> T12 = Kinematics.DHParaToTransfrom(DHParameters[1], angles[1]);
        Matrix<double> T23 = Kinematics.DHParaToTransfrom(DHParameters[2], angles[2]);
        Matrix<double> T34 = Kinematics.DHParaToTransfrom(DHParameters[3], angles[3]);
        Matrix<double> T45 = Kinematics.DHParaToTransfrom(DHParameters[4], angles[4]);
        Matrix<double> T56 = Kinematics.DHParaToTransfrom(DHParameters[5], angles[5]);
        return (T01, T12, T23, T34, T45, T56);
    }
}
