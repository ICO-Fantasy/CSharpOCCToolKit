using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using MathNet.Spatial.Euclidean;
using OCCTK.OCC.gp;

namespace OCCTK.IO;

public static class PointExtensions
{
    /// <summary>
    /// 扩展方法，将 Point3D 转换为 Pnt
    /// </summary>
    /// <param name="point"></param>
    /// <returns></returns>
    public static Pnt ToPnt(this Point3D point)
    {
        return new Pnt(point.X, point.Y, point.Z);
    }

    /// <summary>
    /// 扩展方法，将 Pnt 转换为 Point3D
    /// </summary>
    /// <param name="point"></param>
    /// <returns></returns>
    public static Point3D ToPoint3D(this Pnt point)
    {
        return new Point3D(point.X, point.Y, point.Z);
    }
}

public static class VectorExtensions
{
    /// <summary>
    /// 扩展方法，将 Vector3D 转换为 Vec
    /// </summary>
    /// <param name="point"></param>
    /// <returns></returns>
    public static Vec ToVec(this Vector3D point)
    {
        return new Vec(point.X, point.Y, point.Z);
    }

    /// <summary>
    /// 扩展方法，将 Pnt 转换为 Point3D
    /// </summary>
    /// <param name="vector"></param>
    /// <returns></returns>
    public static Vector3D ToVector3D(this Vec vector)
    {
        return new Vector3D(vector.X, vector.Y, vector.Z);
    }
}
