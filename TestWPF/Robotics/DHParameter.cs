using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TestWPF.Robotics;

public class DHParameter
{
    public DHParameter(double a, double d, double alpha, double theta, double offset = 0.0)
    {
        Alpha = alpha;
        Theta = theta;
        A = a;
        D = d;
        Offset = offset;
    }

    public double Alpha { get; set; }
    public double A { get; set; }

    /// <summary>
    /// 初始角度
    /// </summary>
    public double Theta { get; set; }
    public double Offset { get; set; }
    public double D { get; set; }
}
