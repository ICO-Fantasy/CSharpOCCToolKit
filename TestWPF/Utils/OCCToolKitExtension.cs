using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using MathNet.Numerics.LinearAlgebra;
using OCCTK.OCC.gp;

namespace OCCTK.Extension;

/// <summary>
/// OCC命名空间中对象的扩展方法
/// </summary>
public static class OCCToolKitExtension
{
    /// <summary>
    /// 从轴角设置旋转部分
    /// </summary>
    /// <param name="trsf"></param>
    /// <param name="axis"></param>
    /// <param name="angle"></param>
    public static void SetRotationPart(this Trsf trsf, Ax1 axis, double angle)
    {
        Quat quat = new Quat(axis, angle);
        trsf.SetRotationPart(quat);
    }
}
