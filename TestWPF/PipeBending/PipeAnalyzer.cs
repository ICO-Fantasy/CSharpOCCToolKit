using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using OCCTK.Extension;
using OCCTK.IO;
using OCCTK.OCC;
using OCCTK.OCC.AIS;
using OCCTK.OCC.BRepAdaptor;
using OCCTK.OCC.GeomAbs;
using OCCTK.OCC.TopExp;
using OCCTK.OCC.Topo;
using OCCTK.OCC.TopoAbs;

namespace TestWPF.PipeBending;

public class PFace
{
    public PFace(TFace face)
    {
        Surface ada = new(face);
        Type = ada.GetType();
        foreach (var edge in new Explorer(face, ShapeEnum.EDGE))
        {
            Edges.Add(new(edge.AsEdge()));
        }
    }

    public SurfaceType Type { get; protected set; }
    public List<PEdge> Edges { get; protected set; } = [];
}

public class TorusPFace : PFace
{
    public TorusPFace(TFace face)
        : base(face) { }
}

public class PEdge
{
    public PEdge(TEdge edge)
    {
        Curve ada = new(edge);
        Type = ada.GetType();
    }

    public CurveType Type { get; protected set; }
}

public class PipeAnalyzer
{
    public PipeAnalyzer()
    {
        Pipe = new();
    }

    public PipeAnalyzer(string stepFile)
    {
        FromStep(stepFile);
        Pipe = new();
    }

    public void FromStep(string stepFile)
    {
        Pipe.STEPFilePath = stepFile;
        Pipe.originSTEPShape = new STEPExchange(stepFile).Shape().TopoShape;
        Pipe.topoShape = Pipe.originSTEPShape;
    }

    public Pipe Pipe { get; private set; }

    // 定义隐式转换运算符
    public static implicit operator Pipe(PipeAnalyzer ana)
    {
        return ana.Pipe;
    }
}
