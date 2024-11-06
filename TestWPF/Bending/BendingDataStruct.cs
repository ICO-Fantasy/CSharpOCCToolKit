using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Linq.Expressions;
using System.Security.Cryptography.Pkcs;
using System.Security.Policy;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Forms.VisualStyles;
using log4net;
using MathNet.Spatial.Units;
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
    public List<TVertex> TopoVertices
    {
        get
        {
            List<TVertex> facePoints = [];
            foreach (var v in new Explorer(TopoFace, ShapeEnum.VERTEX))
            {
                facePoints.Add(v.AsVertex());
            }
            return facePoints;
        }
    }
    public double Area { get; private set; }
    public Pnt FaceCenter { get; private set; }
    public SurfaceType Type { get; private set; }
    public Dictionary<Edge, Face> AdjacentFaces { get; set; } = [];

    #region 圆柱面属性
    public Ax1? CircleAixs { get; private set; }
    public double? CircleRadius { get; private set; }
    public Pnt? CircleCenter { get; private set; }
    public double? CircleAngle { get; private set; }
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

    public void DebugErase()
    {
        AISp1.RemoveSelf(false);
        AISp2.RemoveSelf(false);
        AISp3.RemoveSelf(false);
        AIScenter.RemoveSelf(false);
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
        if (point1.Distance(point3) < 1e-3)
        {
            //todo 圆柱为完整圆柱，非折弯面
            Trsf t = new();
            Vec v = new(point1, point2);
            v.Multiply(0.5);
            t.SetTranslation(v);
            CircleCenter = point1.Transformed(t);
            CircleAixs.Location = CircleCenter;
            CircleAngle = Math.PI * 2;
            CircleNormal = new Dir(new Vec(point2, CircleCenter));
            return;
        }
        //三点定圆
        (Pnt CircleCenter, double Radius, double Angle) C =
            Geometry.Tools.BasicGeometryTools.ThreePointFixedCircle(point1, point2, point3);
        if (Math.Abs((double)(C.Radius - CircleRadius)) > TOL)
        {
            log.Debug($"半径计算错误 计算值{C.Radius}和{CircleRadius}不一致");
        }
        #region Debug
        MakeSphere p1 = new(point1, C.Angle * (double)CircleRadius / 50.0);
        MakeSphere p2 = new(point2, C.Angle * (double)CircleRadius / 50.0);
        MakeSphere p3 = new(point3, C.Angle * (double)CircleRadius / 50.0);
        MakeSphere center = new(C.CircleCenter, 1);
        AISp1 = new(p1);
        AISp2 = new(p2);
        AISp3 = new(p3);
        AIScenter = new(center);

        #endregion
        CircleCenter = C.CircleCenter;
        CircleAixs.Location = CircleCenter;
        CircleAngle = C.Angle;
        CircleNormal = new Dir(new Vec(point2, CircleCenter));
    }

    private void GetNormal()
    {
        Surface BF = new(TopoFace);
        double U = BF.FirstUParameter() + (BF.LastUParameter() - BF.FirstUParameter()) / 2.0;
        double V = BF.FirstVParameter() + (BF.LastVParameter() - BF.FirstVParameter()) / 2.0;
        BRepGlobalProperties_Face BGPF = new(TopoFace);
        normal = new(FaceCenter, new Dir(BGPF.Normal(U, V)));
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

    // 重载 == 运算符
    public static bool operator ==(Edge? a, Edge? b)
    {
        if (a is null && b is null)
            return true;
        if (a is null || b is null)
            return false;
        return a.Equals(b);
    }

    // 重载 != 运算符
    public static bool operator !=(Edge? a, Edge? b)
    {
        return !(a == b);
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
    public List<Pnt> Points { get; private set; } = [];

    /// <summary>
    /// 长度，用于计算板厚
    /// </summary>
    public double Length { get; private set; } = 0.0;

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
        }
        var twoPnts = Geometry.Tools.BrepGeomtryTools.GetEdgeEndPoints(TopoEdge);
        Points = [twoPnts.Item1, twoPnts.Item2];
    }
}

//参考资料 https://www.approvedsheetmetal.com/blog/what-type-of-hem-does-your-custom-sheet-metal-part-need
//todo 目前只支持V弯和压死边
public enum BendingType
{
    VBend,
    OpenHem,
    ClosedHem,
    TeardropHem,
    Curl,
    SimpleBendFlange,
    Normal = VBend,
    Hem = ClosedHem
}

public class BendingDS
{
    public BendingDS(Face cylin1, Face cylin2, HashSet<Face> otherFaces)
    {
        if (cylin1.CircleRadius < cylin2.CircleRadius)
        {
            InnerFace = cylin1;
            OutterFace = cylin2;
        }
        else
        {
            InnerFace = cylin2;
            OutterFace = cylin1;
        }
        Thickness = Math.Round((double)OutterFace.CircleRadius - (double)InnerFace.CircleRadius, 1);
        if (otherFaces.Count < 2)
        {
            throw new Exception("扇形面不为2");
        }

        void GetAdjFaces(Face face, ref List<Face> adjFaces)
        {
            adjFaces.Add(face);
            foreach (var f in face.AdjacentFaces.Values)
            {
                if (f == cylin1 || f == cylin2 || adjFaces.Contains(f))
                {
                    continue;
                }
                if (f.AdjacentFaces.ContainsValue(cylin1) || f.AdjacentFaces.ContainsValue(cylin2))
                {
                    adjFaces.Add(f);
                    GetAdjFaces(f, ref adjFaces);
                }
            }
        }
        List<Face> leftTempt = [];
        GetAdjFaces(otherFaces.First(), ref leftTempt);
        LeftSectorFaces = leftTempt;
        RightSectorFaces = otherFaces.Where(f => !leftTempt.Contains(f)).ToList();

        Normal = new(InnerFace.CircleCenter, InnerFace.CircleNormal);
        Angle = InnerFace.CircleAngle ?? throw new Exception("(内)圆柱面角度为空");
        //todo 暂时只处理V弯和闭合压边两类
        if (Math.Abs(Math.PI - Angle) < 1e-4)
        {
            Type = BendingType.ClosedHem;
        }
        else
        {
            Type = BendingType.VBend;
        }
        Axis = InnerFace.CircleAixs ?? throw new Exception("(内)圆柱面轴线为空");
        List<Edge> cylinLines = InnerFace
            .Edges.Where(edge => edge.Type == CurveType.Line)
            .Where(line => new Dir(line.Points[0], line.Points[1]).IsParallel(Axis.Direction, 1e-4))
            .ToList();
        if (cylinLines.Count < 2)
        {
            throw new Exception("圆柱面没有两条直边");
        }
        Length = cylinLines
            .Select(line => line.Length) // 提取每个 line 的长度
            .OrderByDescending(length => length) // 从大到小排序
            .Take(2) // 取前两个最大的值
            .Average(); // 计算这两个值的平均值
    }

    public List<Face> CylinderFaces => [InnerFace, OutterFace];

    //左右只是为了做区分，不具备实际的意义
    public List<Face> LeftSectorFaces { get; private set; }
    public List<Face> RightSectorFaces { get; private set; }
    public List<Face> SectorFaces
    {
        get => [.. LeftSectorFaces, .. RightSectorFaces];
    }
    public Face InnerFace { get; private set; }
    public Face OutterFace { get; private set; }
    public double Angle { get; private set; }
    public BendingType Type { get; private set; }
    public List<Face> BendingFaces => [.. CylinderFaces, .. SectorFaces];
    public Ax1 Normal { get; private set; }
    public Ax1 Axis { get; private set; }
    public double Length { get; private set; }
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
        return HashCode.Combine(CylinderFaces[0], CylinderFaces[1]);
        ;
    }

    public int Index => GetHashCode();

    public override string ToString()
    {
        return Index.ToString();
    }
    #endregion
}
