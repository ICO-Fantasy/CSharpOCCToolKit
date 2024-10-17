using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Linq.Expressions;
using System.Security.Policy;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using log4net;
using OCCTK.Extension;
using OCCTK.OCC.AIS;
using OCCTK.OCC.BRep;
using OCCTK.OCC.BRepAdaptor;
using OCCTK.OCC.BRepExtrema;
using OCCTK.OCC.BRepGProp;
using OCCTK.OCC.BRepPrimAPI;
using OCCTK.OCC.GeomAbs;
using OCCTK.OCC.gp;
using OCCTK.OCC.GProp;
using OCCTK.OCC.TopExp;
using OCCTK.OCC.Topo;
using OCCTK.OCC.TopoAbs;

namespace TestWPF.Bending;

public class Face
{
    private static readonly ILog log = LogManager.GetLogger(typeof(App));

    public Face(TFace face, TShape parent, double TOL = 1e-2)
    {
        this.TOL = TOL;
        TopoFace = face;
        Index = face.GetHashCode();
        Shape = parent;
        //计算面积和面的中心
        GProps GP = new();
        BRepGlobalProperties.SurfaceProperties(face, ref GP);
        Area = GP.Mass();
        FaceCenter = GP.CentreOfMass();

        GetData();
    }

    #region override
    // 重写 Equals 方法，基于 Index 进行比较
    public override bool Equals(object? obj)
    {
        if (obj == null || obj is not Face)
            return false;

        // 比较 Index
        return Index == ((Face)obj).Index;
    }

    // 重写 GetHashCode 方法，返回 Index 的哈希码
    public override int GetHashCode()
    {
        return Index;
    }

    public override string ToString()
    {
        return Index.ToString();
    }
    #endregion

    public double TOL { get; private set; }
    public int Index { get; private set; }
    public TFace TopoFace { get; private set; }
    public TShape Shape { get; private set; }
    public HashSet<Edge> Edges { get; private set; } = [];
    public double Area { get; private set; }
    public Pnt FaceCenter { get; private set; }
    public SurfaceType Type { get; private set; }
    public Dictionary<Edge, Face> AdjacentFaces { get; set; } = [];

    #region 圆柱面属性
    public Ax1? CircleAixs { get; private set; }
    public double? CircleRadius { get; private set; }
    public Pnt? CircleCenter { get; private set; }
    public Dir? CircleNormal { get; private set; }
    #endregion

    private Ax1? normal;
    public Ax1 Normal
    {
        get
        {
            if (normal == null)
            {
                GetNormal();
            }
            return normal;
        }
    }
    public AAxis? AISNormal;

    public void ShowNormal(InteractiveContext context)
    {
        if (normal == null)
        {
            GetNormal();
        }
        AISNormal = new(normal);
        context.Display(AISNormal, false);
    }

    public void RemoveAIS()
    {
        AISNormal?.RemoveSelf(false);
    }

    private void GetData()
    {
        if (TopoFace == null)
            return;

        //! 构建并储存构成面的边
        Explorer edgeExp = new(TopoFace, ShapeEnum.EDGE);
        for (; edgeExp.More(); edgeExp.Next())
        {
            TEdge te = edgeExp.Current().AsEdge();
            Edge e = new(te, this);
            Edges.Add(e);
        }
        Surface BA = new(TopoFace);
        Type = BA.GetType();
        //! 获取特定的信息
        GetCylinderData(BA);
    }

    public AShape AISp1;
    public AShape AISp2;
    public AShape AISp3;
    public AShape AIScenter;

    public void DebugShow(InteractiveContext context, Color color)
    {
        context.Display(AISp1, false);
        context.Display(AISp2, false);
        context.Display(AISp3, false);
        context.Display(AIScenter, false);
        context.SetColor(AISp1, color, false);
        context.SetColor(AISp2, color, false);
        context.SetColor(AISp3, color, false);
        context.SetColor(AIScenter, color, false);
    }

    private void GetCylinderData(Surface brep_face)
    {
        if (Type != SurfaceType.Cylinder || brep_face == null)
            return;
        var brep_cylinder = brep_face.Cylinder();
        CircleAixs = brep_cylinder.Axis();
        CircleRadius = brep_cylinder.Radius();
        //内半径的面法线
        GProps props = new();
        BRepGlobalProperties.SurfaceProperties(TopoFace, ref props);
        //找三个点，定圆
        //! V为圆柱长度方向，U为圆柱弧形方向
        double bv = (
            brep_face.FirstVParameter()
            + (brep_face.LastVParameter() - brep_face.FirstVParameter()) / 2
        );
        //+ 圆弧起点
        double u1 = brep_face.FirstUParameter();
        Pnt point1 = brep_face.Value(u1, bv);
        //+ 圆弧中点
        double u2 = (
            brep_face.FirstUParameter()
            + (brep_face.LastUParameter() - brep_face.FirstUParameter()) / 2
        );
        Pnt point2 = brep_face.Value(u2, bv);
        //+ 圆弧终点
        double u3 = brep_face.LastUParameter();
        Pnt point3 = brep_face.Value(u3, bv);
        //三点定圆
        (Pnt CircleCenter, double Radius, double Angle) C =
            Geometry.Tools.BasicGeometryTools.ThreePointFixedCircle(point1, point2, point3);
        if (Math.Abs((double)(C.Radius - CircleRadius)) > TOL)
        {
            log.Debug($"半径计算错误 计算值{C.Radius}和{CircleRadius}不一致");
        }
        #region Debug
        MakeSphere p1 = new(point1, 0.2);
        MakeSphere p2 = new(point2, 0.2);
        MakeSphere p3 = new(point3, 0.2);
        MakeSphere center = new(C.CircleCenter, 1);
        AISp1 = new(p1);
        AISp2 = new(p2);
        AISp3 = new(p3);
        AIScenter = new(center);

        #endregion
        CircleCenter = C.CircleCenter;
        CircleNormal = new Dir(new Vec(point2, CircleCenter));
    }

    private void GetNormal()
    {
        Surface BF = new(TopoFace);
        double U = BF.FirstUParameter() + (BF.LastUParameter() - BF.FirstUParameter()) / 2.0;
        double V = BF.FirstVParameter() + (BF.LastVParameter() - BF.FirstVParameter()) / 2.0;
        BRepGlobalProperties_Face BGPF = new(TopoFace);
        normal = new(FaceCenter, new(BGPF.Normal(U, V)));
    }
}

public class Edge
{
    private static readonly ILog log = LogManager.GetLogger(typeof(App));

    public Edge(TEdge edge, Face parent)
    {
        TopoEdge = edge;
        Parent = parent;
        Curve BE = new(edge);
        Index = edge.GetHashCode();
        Type = BE.GetType();
        GetEdgeData(BE);
    }

    #region override
    // 重写 Equals 方法，基于 Index 进行比较
    public override bool Equals(object? obj)
    {
        if (obj == null || obj is not Edge)
            return false;

        // 比较 Index
        return Index == ((Edge)obj).Index;
    }

    // 重写 GetHashCode 方法，返回 Index 的哈希码
    public override int GetHashCode()
    {
        return Index;
    }

    public override string ToString()
    {
        return Index.ToString();
    }
    #endregion

    public Face Parent { get; private set; }
    public HashSet<Face> Faces { get; private set; } = [];

    public void SetFaces(List<Face> faces) => Faces = [.. faces];

    public int Index { get; private set; }

    public void SetIndex(int value)
    {
        Index = value;
    }

    public TEdge TopoEdge { get; private set; }
    public CurveType Type { get; private set; }

    /// <summary>
    /// 长度，用于计算板厚
    /// </summary>
    public double? Length { get; private set; }

    #region 圆弧属性
    public double? CircleRadius { get; private set; }
    public Pnt? CircleCenter { get; private set; }
    #endregion

    private void GetEdgeData(Curve BE)
    {
        if (TopoEdge == null)
            return;
        if (Type == CurveType.Line)
        {
            Length = BE.Value(BE.FirstParameter()).Distance(BE.Value(BE.LastParameter()));
        }
        if (Type == CurveType.Circle)
        {
            var c = BE.Circle();
            CircleCenter = c.Location();
            CircleRadius = c.Radius;
            Length = null;
        }
    }
}

public class BendingDS
{
    public BendingDS(HashSet<Face> faces)
    {
        List<Face> cylin = faces.Where(x => x.Type == SurfaceType.Cylinder).ToList();
        if (cylin.Count != 2)
        {
            throw new Exception("圆柱面数不为2");
        }
        if (cylin[0].CircleRadius < cylin[1].CircleRadius)
        {
            InnerFace = cylin[0];
            OutFace = cylin[1];
        }
        else
        {
            InnerFace = cylin[1];
            OutFace = cylin[0];
        }
        List<Face> plane = faces
            .Where(x => x.Type == SurfaceType.Plane || x.Type == SurfaceType.BSplineSurface)
            .ToList();
        if (plane.Count != 2)
        {
            throw new Exception("扇形面不为2");
        }
        //todo 暂时取扇形面的直边作为板厚
        foreach (var e in plane[0].Edges)
        {
            if (e.Length == null)
                continue;
            Thickness = Math.Round((double)e.Length, 1);
            break;
        }

        PlaneFaces = (plane[0], plane[1]);
        Axis = new(InnerFace.CircleCenter, InnerFace.CircleNormal);
    }

    public (Face, Face) CylinderFaces => (InnerFace, OutFace);
    public (Face, Face) PlaneFaces { get; private set; }
    public Face InnerFace { get; private set; }
    public Face OutFace { get; private set; }
    public List<Face> BendingFaces =>
        [CylinderFaces.Item1, CylinderFaces.Item2, PlaneFaces.Item1, PlaneFaces.Item2];
    public Ax1 Axis { get; private set; }
    public double Thickness { get; private set; } = 0.0;

    #region override
    // 重写 Equals 方法，基于 Index 进行比较
    //public override bool Equals(object? obj) {
    //if (obj == null || obj is not Edge)
    //    return false;

    //// 比较 Index
    //return Index == ((Edge)obj).Index;
    //}

    // 重写 GetHashCode 方法，返回 Index 的哈希码
    public override int GetHashCode()
    {
        return HashCode.Combine(
            CylinderFaces.Item1,
            CylinderFaces.Item2,
            PlaneFaces.Item1,
            PlaneFaces.Item2
        );
        ;
    }

    //public override string ToString() {
    //return Index.ToString();
    //}
    #endregion
}
