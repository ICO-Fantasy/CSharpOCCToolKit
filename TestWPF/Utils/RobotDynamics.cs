using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using MathNet.Numerics.LinearAlgebra;
using MathNet.Numerics.LinearAlgebra.Double;

namespace TestWPF.Utils;

class RobotDynamics
{
    public static Matrix<double> ForwardKinematics(
        List<double> thetas,
        Matrix<double> baseCoordinate,
        RobotData robot,
        Matrix<double> toolCoordinate
    )
    {
        if (thetas.Count != 6)
        {
            throw new ArgumentException("需要六个角度值");
        }
        Matrix<double> robotT = Matrix<double>.Build.DenseIdentity(4);
        for (int i = 0; i < 6; i++)
        {
            Matrix<double> T = RobotTransform.DHtoSO3(thetas[i], robot.DHParameters[i]);
            robotT = robotT.Multiply(T);
        }
        return baseCoordinate.Multiply(robotT).Multiply(toolCoordinate);
    }
}
