using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using OCCTK.Extension;
using OCCTK.OCC.gp;

namespace TestWPF.Laser.Positioner;

public class PositionerFunctions
{
    /// <summary>
    /// 将方向按角度分度
    /// </summary>
    /// <param name="originDir">原始方向</param>
    /// <param name="internalAngle">分度角度(Degree)</param>
    /// <param name="boundary">最大偏离范围（正/负）</param>
    /// <returns></returns>
    public static AngleMatrix<Knot> AngularDivision(
        Ax2 originAx2,
        double internalAngle,
        double boundary
    )
    {
        AngleMatrix<Knot> result = new();

        List<(double angleX, double angleY)> diffAngles = [];

        // 计算正向角度组合
        double angleXPlus = 0;
        while (angleXPlus <= boundary)
        {
            double angleYPlus = 0;
            while (angleYPlus <= boundary)
            {
                diffAngles.Add((angleXPlus, angleYPlus));
                angleYPlus += internalAngle;
            }
            angleXPlus += internalAngle;
        }
        // 计算负向角度组合
        double angleXMinus = 0;
        while (angleXMinus >= -boundary)
        {
            double angleYMinus = 0;
            while (angleYMinus >= -boundary)
            {
                diffAngles.Add((angleXMinus, angleYMinus));
                angleYMinus -= internalAngle;
            }
            angleXMinus -= internalAngle;
        }

        // 并行处理角度组合
        object lockObj = new object();
        Parallel.ForEach(
            diffAngles,
            angles =>
            {
                double angleX = angles.angleX;
                double angleY = angles.angleY;

                Trsf rx = new Trsf();
                Trsf ry = new Trsf();
                rx.SetRotation(originAx2.XAxis, angleX);
                ry.SetRotation(originAx2.YAxis, angleY);

                Dir newDir = originAx2.ZDir.Transformed(rx).Transformed(ry);
                double angleWithOrigin = newDir.Angle(originAx2.ZDir);

                lock (lockObj) // 确保线程安全地更新结果
                {
                    result[angleX, angleY] = new(
                        originAx2,
                        angleX,
                        angleY,
                        angleWithOrigin,
                        newDir
                    );
                }
            }
        );

        return result;
    }

    /// <summary>
    /// testFunction
    /// </summary>
    public static Dictionary<double, double> TestFun(Pnt location, List<(Dir, double)> values)
    {
        Dictionary<double, double> result = [];
        //新的点位

        //新的姿态
        //二轴解
        //6R解
        //偏离量
        return result;
    }

    /// <summary>
    /// 根据两个姿态之间的角度差，构建层间距离矩阵
    /// </summary>
    /// <param name="fromLayer"></param>
    /// <param name="toLayer"></param>
    public static void GetAdjacencyMatrixWithDir(
        AngleMatrix<Knot> fromLayer,
        AngleMatrix<Knot> toLayer
    )
    {
        foreach (var fromKnot in fromLayer)
        {
            foreach (var toKnot in toLayer)
            {
                double angleDistance = fromKnot.Direction.Angle(toKnot.Direction);
                fromKnot.AdjacencyMatrix[toKnot] = angleDistance;
            }
        }
    }

    public static List<Knot> TestFun2(List<Ax2> trajectory, double internalAngle, double boundary)
    {
        List<Knot> result = [];
        List<AngleMatrix<Knot>> layers = [];
        foreach (var ax2 in trajectory)
        {
            layers.Add(AngularDivision(ax2, internalAngle, boundary));
        }

        //todo 先用粗的分度进行尝试
        //List<AngleMatrix<(double, Dir)>> coarseLayers = [];
        //foreach (var layer in layers)
        //{
        //    AngleMatrix<(double, Dir)> coarseAngleMatrix = new();
        //    if (layer.Dx.Count <= 10 || layer.Dy.Count <= 10)
        //    {
        //        coarseLayers.Add(layer);
        //        continue;
        //    }
        //    for (int i = 0; i < layer.Dx.Count; i += 4)
        //    {
        //        for (int j = 0; j < layer.Dy.Count; j += 4)
        //        {
        //            coarseAngleMatrix[layer.Dx[i], layer.Dy[j]] = layer[layer.Dx[i], layer.Dy[j]];
        //        }
        //    }
        //    coarseLayers.Add(coarseAngleMatrix);
        //}

        return result;
    }

    public static void TestFun3()
    {
        double minAngle = double.PositiveInfinity;
    }
}
