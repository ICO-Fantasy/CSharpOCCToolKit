using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;
using MathNet.Numerics.LinearAlgebra.Factorization;
using OCCTK.OCC.BRep;
using OCCTK.OCC.gp;
using OCCTK.OCC.TopExp;
using OCCTK.OCC.Topo;
using OCCTK.OCC.TopoAbs;

namespace TestWPF.Geometry.Tools;

public class BasicGeometryTools
{
    private BasicGeometryTools() { }

    public static Tuple<Pnt, Pnt> GetEdgeEndPoints(TEdge edge)
    {
        List<Pnt> endpoints = new();
        Explorer PntExp = new(edge, ShapeEnum.VERTEX);
        //while (PntExp.More())
        //{
        //    TVertex endPoint = PntExp.Current().AsVertex();
        //    Pnt p = Tool.Pnt(endPoint);
        //    endpoints.Add(p);
        //    PntExp.Next();
        //}
        for (; PntExp.More(); PntExp.Next())
        {
            TVertex endPoint = PntExp.Current().AsVertex();
            Pnt p = Tool.Pnt(endPoint);
            endpoints.Add(p);
        }
        if (endpoints.Count != 2)
        {
            throw new Exception($"获取到的端点数不为2，有{endpoints.Count}个点");
        }
        return Tuple.Create(endpoints[0], endpoints[1]);
    }

    public static Pnt GetEdgeMidlePoint(TEdge edge)
    {
        OCCTK.OCC.BRepAdaptor.Curve aBAC = new(edge);
        return aBAC.Value(
            aBAC.FirstParameter() + (aBAC.LastParameter() - aBAC.FirstParameter()) / 2
        );
    }
}
