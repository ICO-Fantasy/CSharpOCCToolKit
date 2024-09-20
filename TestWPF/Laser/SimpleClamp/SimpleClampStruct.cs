using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Shapes;
using System.Windows.Xps.Packaging;
using OCCTK.OCC.AIS;
using OCCTK.OCC.BRepBuilderAPI;
using OCCTK.OCC.BRepPrimAPI;
using OCCTK.OCC.gp;
using OCCTK.OCC.Topo;
using Windows.Devices.Bluetooth;
using static System.Windows.Forms.VisualStyles.VisualStyleElement.Rebar;
using GT = TestWPF.Geometry.Tools.BasicGeometryTools;

namespace TestWPF.Laser.SimpleClamp;

public class Workpiece : IDisposable
{
    /// <summary>
    /// 会默认把输入的Shape的左下角挪动到原点
    /// </summary>
    /// <param name="shape"></param>
    public Workpiece(TShape shape)
    {
        _shape = _MoveToOrigin(shape);
        _AIS = new(_shape);
        _bndBox = new OCCTK.Extension.BoundingBox(Shape).GetBndBox();
    }

    private TShape _shape;
    public TShape Shape
    {
        get { return _shape; }
        set
        {
            _shape = _MoveToOrigin(value);
            AIS = new(_shape);
            _bndBox = new OCCTK.Extension.BoundingBox(Shape).GetBndBox();
        }
    }
    private AShape _AIS;
    public AShape AIS
    {
        get { return _AIS; }
        private set
        {
            _AIS?.RemoveSelf();
            _AIS = value;
        }
    }

    // 定义隐式转换运算符，将 Workpiece 隐式转换为 AShape
    public static implicit operator AShape(Workpiece workpiece)
    {
        return workpiece.AIS;
    }

    // 定义隐式转换运算符，将 Workpiece 隐式转换为 TShape
    public static implicit operator TShape(Workpiece workpiece)
    {
        return workpiece.Shape;
    }

    #region 析构
    // 手动释放非托管资源
    public void Dispose()
    {
        Dispose(true);
        //不需要再自动GC
        GC.SuppressFinalize(this);
    }

    protected virtual void Dispose(bool disposing)
    {
        if (disposing)
        {
            // Dispose managed resources
            _AIS?.RemoveSelf();
        }
        // Dispose unmanaged resources if any
    }

    // 析构
    ~Workpiece()
    {
        Dispose(false);
    }
    #endregion

    private TShape _MoveToOrigin(TShape shape)
    {
        OCCTK.Extension.BoundingBox Bnd = new(shape);
        double[] bndarray = Bnd.GetBndBox();
        return new OCCTK.OCC.BRepBuilderAPI.Transform(
            shape,
            new Trsf(new Pnt(bndarray[0], bndarray[1], bndarray[2]), new Pnt())
        );
    }

    private double[] _bndBox;
    public double[] BndBox
    {
        get { return _bndBox; }
    }
}

public class BasePlate
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
        get { return Math.Abs(Workpiece.BndBox[3] - Workpiece.BndBox[0]) + 2 * offsetX; }
        set
        {
            offsetX = (value - Math.Abs(Workpiece.BndBox[3] - Workpiece.BndBox[0])) / 2;
            _changed = true;
        }
    }

    /// <summary>
    /// Y方向长度
    /// </summary>
    public double DY
    {
        get { return Math.Abs(Workpiece.BndBox[4] - Workpiece.BndBox[1]) + 2 * offsetY; }
        set
        {
            offsetX = (value - Math.Abs(Workpiece.BndBox[4] - Workpiece.BndBox[1])) / 2;
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
        get { return Workpiece.BndBox[2] - offsetZ; }
    }

    private TShape _shape;
    public TShape Shape
    {
        get
        {
            if (_changed)
            {
                Pnt leftBottom = new(Workpiece.BndBox[0], Workpiece.BndBox[1], Z);
                Pnt rightTop = new(Workpiece.BndBox[3], Workpiece.BndBox[4], Z);
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

    private AShape? _AIS;
    public AShape AIS
    {
        get
        {
            if (_AIS == null)
            {
                _AIS = new(Shape);
            }
            return _AIS;
        }
        private set
        {
            _AIS?.RemoveSelf();
            _AIS = value;
        }
    }

    // 定义隐式转换运算符，将 BasePlate 隐式转换为 AShape
    public static implicit operator AShape(BasePlate baseplate)
    {
        return baseplate.AIS;
    }

    // 定义隐式转换运算符，将 BasePlate 隐式转换为 TShape
    public static implicit operator TShape(BasePlate baseplate)
    {
        return baseplate.Shape;
    }

    //TopoDS_Shape mySlotShape = TopoDS_Shape();
    //BRep_Builder mySlotBuilder = BRep_Builder();
}

public class PlatePose(Pnt location, Dir direction)
{
    public Pnt Location { get; set; } = location;
    public Dir Direction { get; set; } = direction;
    public TShape Plane
    {
        get { return new MakeFace(new Pln(Location, Direction)); }
    }
    private AShape? _AIS;
    public AShape AIS
    {
        get
        {
            _AIS?.RemoveSelf();
            _AIS = new(new MakeFace(new Pln(Location, Direction), -100, 100, -100, 100));
            return _AIS;
        }
    }

    // 定义隐式转换运算符，将 PlatePose 隐式转换为 TShape
    public static implicit operator TShape(PlatePose platePose)
    {
        return platePose.Plane;
    }

    // 定义隐式转换运算符，将 PlatePose 隐式转换为 TShape
    public static implicit operator AShape(PlatePose platePose)
    {
        return platePose.AIS;
    }
}

public class MyEdge
{
    public MyEdge(TEdge edge, PlatePose pose)
    {
        Edge = edge;
        Pose = pose;
        (Start, End) = GT.GetEdgeEndPoints(edge);
        Middle = GT.GetEdgeMidlePoint(edge);
    }

    public PlatePose Pose { get; private set; }
    public TEdge Edge { get; private set; }

    // 定义隐式转换运算符，将 Workpiece 隐式转换为 TShape
    public static implicit operator TEdge(MyEdge myEdge)
    {
        return myEdge.Edge;
    }

    public Pnt Start { get; private set; }
    public Pnt End { get; private set; }
    public Pnt Middle { get; private set; }
}

public class VerticalPiece(MyEdge edge, BasePlate baseplate)
{
    public BasePlate BasePlate { get; private set; } = baseplate;
    public PlatePose Pose
    {
        get { return Edge.Pose; }
    }
    public MyEdge Edge { get; private set; } = edge;
    public TFace Shape
    {
        get
        {
            Pnt p1 = Edge.Start;
            Pnt p2 = Edge.End;
            Pnt p0 = new(p1.X, p1.Y, BasePlate.Z);
            Pnt p3 = new(p2.X, p2.Y, BasePlate.Z);
            TEdge edge0 = new MakeEdge(p0, p1);
            TEdge edge1 = Edge;
            TEdge edge2 = new MakeEdge(p2, p3);
            TEdge edge3 = new MakeEdge(p3, p0);
            TWire wire = new MakeWire(new List<TEdge> { edge0, edge1, edge2, edge3 });
            return new MakeFace(wire);
        }
    }
    public AShape AIS
    {
        get { return new AShape(Shape); }
    }
}
