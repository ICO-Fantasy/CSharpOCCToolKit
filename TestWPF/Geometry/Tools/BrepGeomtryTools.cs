using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using OCCTK.OCC.BRep;
using OCCTK.OCC.BRepBuilderAPI;
using OCCTK.OCC.Geom;
using OCCTK.OCC.GeomAPI;
using OCCTK.OCC.gp;
using OCCTK.OCC.Topo;

namespace TestWPF.Geometry.Tools;

public class BrepGeomtryTools
{
    private BrepGeomtryTools() { }

    public static double GetFaceArea(TFace face)
    {
        return 0.0;
    }

    /// <summary>
    /// 根据投影点修剪曲线
    /// </summary>
    /// <param name="curve"></param>
    /// <param name="p1"></param>
    /// <param name="p2"></param>
    /// <returns></returns>
    public static TEdge TrimCurve(TEdge curve, Pnt p1, Pnt p2)
    {
        //获取底层曲线
        double first = 0;
        double last = 0;
        Curve ParametricCurves = Tool.Curve(curve, ref first, ref last);
        // 有时底层曲线没有创建，要手动创建
        if (ParametricCurves.IsNull())
        {
            //BRepLib::BuildCurves3d(theOriginEdge, 1.0e-5, GeomAbs_C1);//创建曲线 (一阶导数的连续性)
            //aCurve = BRep_Tool::Curve(theOriginEdge, l, first, last);
        }

        //投影点到曲线上，并获取投影点处的参数
        ProjectPointOnCurve ppc1 = new(p1, ParametricCurves);
        double param1 = ppc1.LowerDistanceParameter();
        ProjectPointOnCurve ppc2 = new(p2, ParametricCurves);
        double param2 = ppc2.LowerDistanceParameter();
        if (param1 > param2)
        {
            (param1, param2) = (param2, param1);
        }
        //处于起始和终止参数中间，则构建两个新边
        if (first <= param1 && param1 <= last && first <= param2 && param2 <= last)
        {
            return new MakeEdge(ParametricCurves, param1, param2);
        }
        else
        {
            //如果投影点参数小于起始参数或大于终止参数，则分割失败
            throw new Exception("修剪失败");
        }
    }
}
