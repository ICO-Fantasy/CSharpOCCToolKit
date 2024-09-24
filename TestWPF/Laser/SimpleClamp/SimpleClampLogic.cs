using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms.VisualStyles;
using System.Windows.Shapes;
using System.Windows.Xps.Packaging;
using log4net;
using OCCTK.Extension;
using OCCTK.OCC.AIS;
using OCCTK.OCC.Bnd;
using OCCTK.OCC.BRepAlgoAPI;
using OCCTK.OCC.BRepBuilderAPI;
using OCCTK.OCC.BRepPrimAPI;
using OCCTK.OCC.gp;
using OCCTK.OCC.Topo;
using Windows.System.Profile;
using GT = TestWPF.Geometry.Tools.BasicGeometryTools;

namespace TestWPF.Laser.SimpleClamp;

public partial class Workpiece : IDisposable
{
    private static TShape MoveToOrigin(TShape shape)
    {
        BoundingBox Bnd = new(shape);
        return new Transform(shape, new Trsf(Bnd.GetAABB().CornerMin(), new Pnt()));
    }
}

public partial class BasePlate { }

public partial class PlatePose
{
    public void Show(InteractiveContext context, Workpiece workpiece)
    {
        _AIS ??= new(
            new MakeFace(
                new Pln(Location, Direction),
                workpiece.BndBox.XMin,
                workpiece.BndBox.XMax,
                workpiece.BndBox.YMin,
                workpiece.BndBox.YMax
            )
        );
        context.Display(_AIS, false);
        context.SetColor(_AIS, new(0, 255, 97), false);
        context.SetTransparency(_AIS, 0.8, false);
        context.SetSelectionMode(_AIS, SelectionMode.None);
        context.UpdateCurrentViewer();
    }

    public void Remove()
    {
        _AIS?.RemoveSelf();
    }

    //// 定义隐式转换运算符，将 PlatePose 隐式转换为 TShape
    //public static implicit operator AShape(PlatePose platePose)
    //{
    //    return platePose.AIS;
    //}
}

public partial class MyEdge { }

public partial class VerticalPiece
{
    private static readonly ILog log = LogManager.GetLogger(typeof(App));

    public void Show(InteractiveContext context, bool update)
    {
        context.Display(AISPiece, false);
        context.SetColor(AISPiece, new(0, 255, 0), false);
        context.Display(_AISEdge, false);
        context.SetColor(_AISEdge, new(255, 0, 0), false);
        if (update)
        {
            context.UpdateCurrentViewer();
        }
    }

    public void Remove()
    {
        AISPiece.RemoveSelf();
        _AISEdge.RemoveSelf();
    }
}

public partial class VerticalPlate
{
    private static readonly ILog log = LogManager.GetLogger(typeof(App));
    private List<List<MyEdge>> rings = [];
    private List<MyEdge> buttomEdges = [];

    /// <summary>
    /// 从横截面分离边并得到闭合的环
    /// </summary>
    public void GetRings()
    {
        #region 得到截面上所有边
        Splitter splitter = new();
        splitter.SetArguments(Workpiece);
        splitter.SetTools(Pose);
        splitter.Build();
        List<MyEdge> allEdges = [];
        foreach (TEdge? edge in splitter.SectionEdges())
        {
            allEdges.Add(new MyEdge(edge, Pose));
        }
        rings = SplitRing(allEdges);
        #endregion
    }

    /// <summary>
    /// 静态方法，返回分离后的环
    /// </summary>
    /// <returns></returns>
    private static List<List<MyEdge>> SplitRing(in List<MyEdge> edges)
    {
        List<List<MyEdge>> rings = [];
        while (edges.Count > 0) { }
        return rings;
    }

    /// <summary>
    /// 从环中获取下端线
    /// </summary>
    public void GetBottomEdges() { }
    #region 切割工件
    #endregion

    #region Display
    private void ShowPlate(InteractiveContext context, bool update) { }

    private void ShowPieces(InteractiveContext context, bool update) { }

    private void Remove() { }
    #endregion
}
