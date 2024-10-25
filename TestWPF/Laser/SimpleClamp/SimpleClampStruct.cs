using System;
using System.Collections.Generic;
using System.Linq;
using OCCTK.Extension;
using OCCTK.OCC.AIS;
using OCCTK.OCC.Bnd;
using OCCTK.OCC.BRepBuilderAPI;
using OCCTK.OCC.gp;
using OCCTK.OCC.Topo;
using TestWPF.Geometry.Tools;

namespace TestWPF.Laser.SimpleClamp;

public partial class Workpiece
{
    /// <summary>
    /// 会默认把输入的Shape的左下角挪动到原点
    /// </summary>
    /// <param name="shape"></param>
    public Workpiece(TShape shape)
    {
        _shape = MoveToOrigin(shape);
        _AIS = new(_shape);
        BndBox = new BoundingBox(Shape).GetAABB();
    }

    private TShape _shape;
    public TShape Shape
    {
        get { return _shape; }
        set
        {
            _shape = MoveToOrigin(value);
            AIS = new(_shape);
            BndBox = new BoundingBox(Shape).GetAABB();
        }
    }

    // 定义隐式转换运算符，将 Workpiece 隐式转换为 TShape
    public static implicit operator TShape(Workpiece workpiece)
    {
        return workpiece.Shape;
    }

    private AShape _AIS;
    public AShape AIS
    {
        get { return _AIS; }
        private set
        {
            _AIS.RemoveSelf(false);
            _AIS = value;
        }
    }

    // 定义隐式转换运算符，将 Workpiece 隐式转换为 AShape
    public static implicit operator AShape(Workpiece workpiece)
    {
        return workpiece.AIS;
    }

    public AABB BndBox;
    public Pnt CornerLeft
    {
        get { return BndBox.CornerMin(); }
    }
    public double DX
    {
        get { return BndBox.XMax - BndBox.XMin; }
    }
    public double DY
    {
        get { return BndBox.YMax - BndBox.YMin; }
    }
    public double DZ
    {
        get { return BndBox.ZMax - BndBox.ZMin; }
    }
}

public partial class BasePlate
{
    /// <summary>
    /// 使用默认值构建底板
    /// </summary>
    /// <param name="inputWorkpiece"></param>
    public BasePlate(Workpiece inputWorkpiece)
    {
        Workpiece = inputWorkpiece;
        offsetX = 5;
        offsetY = 5;
        offsetZ = 100;
        _shape = new TShape();
        _changed = true;
    }

    /// <summary>
    /// 使用输入的值构建底板
    /// </summary>
    /// <param name="inputWorkpiece"></param>
    /// <param name="theDX"></param>
    /// <param name="theDY"></param>
    /// <param name="theOffsetZ"></param>
    public BasePlate(Workpiece inputWorkpiece, double theDX, double theDY, double theOffsetZ)
    {
        Workpiece = inputWorkpiece;
        DX = theDX;
        DY = theDY;
        offsetZ = theOffsetZ;
        _shape = new TShape();
        _changed = true;
    }

    private bool _changed;
    public Workpiece Workpiece { get; private set; }
    private double offsetX;
    private double offsetY;

    /// <summary>
    /// X方向长度
    /// </summary>
    public double DX
    {
        get { return Workpiece.DX + offsetX * 2; }
        set
        {
            offsetX = (value - Workpiece.DX) / 2;
            _changed = true;
        }
    }

    /// <summary>
    /// Y方向长度
    /// </summary>
    public double DY
    {
        get { return Workpiece.DY + offsetY * 2; }
        set
        {
            offsetX = (value - Workpiece.DY) / 2;
            _changed = true;
        }
    }

    private double offsetZ;

    /// <summary>
    /// 向下偏移量
    /// </summary>
    public double OffsetZ
    {
        get { return offsetZ; }
        set
        {
            offsetZ = value;
            _changed = true;
        }
    }

    public double Z
    {
        get { return Workpiece.BndBox.ZMin - offsetZ; }
    }

    private TShape _shape;
    public TShape Shape
    {
        get
        {
            if (_changed)
            {
                Pnt leftBottom = new(Workpiece.BndBox.XMin, Workpiece.BndBox.YMin, Z);
                Pnt rightTop = new(Workpiece.BndBox.XMax, Workpiece.BndBox.YMax, Z);
                Pnt p1 = new(leftBottom.X, rightTop.Y, Z);
                Pnt p3 = new(rightTop.X, leftBottom.Y, Z);
                TEdge edge1 = new MakeEdge(leftBottom, p1);
                TEdge edge2 = new MakeEdge(p1, rightTop);
                TEdge edge3 = new MakeEdge(rightTop, p3);
                TEdge edge4 = new MakeEdge(p3, leftBottom);
                TWire baseWire = new MakeWire(new List<TEdge>() { edge1, edge2, edge3, edge4 });
                //todo 还需要增加底板烙印
                _shape = new MakeFace(baseWire);
                AIS = new(_shape);
                _changed = false;
            }
            return _shape;
        }
    }

    // 定义隐式转换运算符，将 BasePlate 隐式转换为 TShape
    public static implicit operator TShape(BasePlate baseplate)
    {
        return baseplate.Shape;
    }

    private AShape? _AIS;
    public AShape AIS
    {
        get
        {
            _AIS ??= new(Shape);
            return _AIS;
        }
        private set
        {
            _AIS?.RemoveSelf(false);
            _AIS = value;
        }
    }

    // 定义隐式转换运算符，将 BasePlate 隐式转换为 AShape
    public static implicit operator AShape(BasePlate baseplate)
    {
        return baseplate.AIS;
    }

    //todo 开槽和烙印
    //TopoDS_Shape mySlotShape = TopoDS_Shape();
    //BRep_Builder mySlotBuilder = BRep_Builder();
}

public partial class PlatePose
{
    public PlatePose(Pnt location, Dir direction)
    {
        Location = location;
        Direction = direction;
        resetT = new();
        resetT.SetRotation(
            new Ax1(location, new(0, 0, 1)),
            direction.AngleWithRef(new(0, 1, 0), new(0, 0, 1))
        );
    }

    public Pnt Location { get; set; }
    public Dir Direction { get; set; }

    public TShape Plane
    {
        get { return new MakeFace(new Pln(Location, Direction)); }
    }

    // 定义隐式转换运算符，将 PlatePose 隐式转换为 TShape
    public static implicit operator TShape(PlatePose platePose)
    {
        return platePose.Plane;
    }

    private AShape? _AIS;

    private readonly Trsf resetT;

    public Trsf Trsf
    {
        get => resetT;
    }

    //// 定义隐式转换运算符，将 PlatePose 隐式转换为 AShape
    //public static implicit operator AShape(PlatePose platePose)
    //{
    //    return platePose.AIS;
    //}
}

public partial class MyEdge
{
    public MyEdge(TEdge edge, PlatePose pose)
    {
        Edge = edge;
        Pose = pose;
        (Start, End) = BrepGeomtryTools.GetEdgeEndPoints(edge);
        Middle = BrepGeomtryTools.GetEdgeMidlePoint(edge);
        GetOrder();
    }

    public PlatePose Pose { get; private set; }

    public Pnt Start { get; private set; }
    public Pnt End { get; private set; }
    public Pnt Middle { get; private set; }

    public TEdge Edge { get; private set; }

    public double OrderStart { get; private set; }
    public double OrderEnd { get; private set; }

    /// <summary>
    /// 根据pose旋转后的X的中间值；
    /// </summary>
    public double Order { get; private set; }

    private void GetOrder()
    {
        Pnt resetStart = Start.Transformed(Pose.Trsf);
        Pnt resetEnd = End.Transformed(Pose.Trsf);
        if (resetStart.X > resetEnd.X)
        {
            (Start, End) = (End, Start);
        }
        OrderStart = new[] { resetStart.X, resetEnd.X }.Min();
        OrderEnd = new[] { resetStart.X, resetEnd.X }.Max();
        Order = (OrderStart + OrderEnd) / 2;
    }

    // 定义隐式转换运算符，将 Workpiece 隐式转换为 TShape
    public static implicit operator TEdge(MyEdge myEdge)
    {
        return myEdge.Edge;
    }
}

public partial class VerticalPiece : IComparable<VerticalPiece>
{
    public VerticalPiece(MyEdge edge, BasePlate baseplate)
    {
        BasePlate = baseplate;
        _edge = edge;
        AISPiece = new(Shape);
        _AISEdge = new(edge);
    }

    public BasePlate BasePlate { get; private set; }
    public PlatePose Pose
    {
        get { return _edge.Pose; }
    }
    private TFace Shape
    {
        get
        {
            Pnt p1 = _edge.Start;
            Pnt p2 = _edge.End;
            //StackFrame stackFrame = new(0);
            //log.Debug(
            //    $"Edge_Start: {p1} | Edge_End: {p2} | Method: {stackFrame.GetMethod().Name} | Line: {stackFrame.GetFileLineNumber()}"
            //);
            Pnt p0 = new(p1.X, p1.Y, BasePlate.Z);
            Pnt p3 = new(p2.X, p2.Y, BasePlate.Z);
            TEdge edge0 = new MakeEdge(p0, p1);
            TEdge edge1 = _edge.Edge;
            TEdge edge2 = new MakeEdge(p2, p3);
            TEdge edge3 = new MakeEdge(p3, p0);
            TWire wire = new MakeWire([edge0, edge1, edge2, edge3]);
            return new MakeFace(wire);
        }
    }

    // 定义隐式转换运算符，将 VerticalPiece 隐式转换为 TEdge
    public static implicit operator TEdge(VerticalPiece piece)
    {
        return piece.Edge.Edge;
    }

    private readonly MyEdge _edge;
    public MyEdge Edge => _edge;
    public AShape AISPiece { get; private set; }
    private readonly AShape _AISEdge;
    public double Order => _edge.Order;
    public List<AShape> AISs
    {
        get { return [AISPiece, _AISEdge]; }
    }

    public int CompareTo(VerticalPiece? other)
    {
        if (other == null)
            return 1;
        return Order.CompareTo(other.Order);
    }
}

public partial class VerticalPlate
{
    public VerticalPlate(BasePlate basePlate, PlatePose pose)
    {
        BasePlate = basePlate;
        Workpiece = basePlate.Workpiece;
        this.pose = pose;
    }

    private PlatePose pose;
    public PlatePose Pose
    {
        get => pose;
        set => pose = value;
    }

    private double clearances;

    /// <summary>
    /// 两端避让长度
    /// </summary>
    public double Clearances
    {
        get => clearances;
        set => clearances = value;
    }

    private double minSupportLen;

    /// <summary>
    /// 最小支撑长度
    /// </summary>
    public double MinSupportLen
    {
        get => minSupportLen;
        set => minSupportLen = value;
    }

    private double cutDistance;

    /// <summary>
    /// 切断距离
    /// </summary>
    public double CutDistance
    {
        get => cutDistance;
        set => cutDistance = value;
    }

    /// <summary>
    /// 连接避让高度
    /// </summary>
    public double AvoidanceHeight { get; set; } = 20.0;

    /// <summary>
    /// 连接槽宽度（板厚）
    /// </summary>
    public double ConnectionThickness { get; set; } = 2.0;

    public readonly BasePlate BasePlate;
    public readonly Workpiece Workpiece;

    public readonly SortedSet<VerticalPiece> Pieces = [];
}
