using System;
using System.Collections.Generic;
using log4net;
using OCCTK.Extension;
using OCCTK.OCC.AIS;
using OCCTK.OCC.BRepBuilderAPI;
using OCCTK.OCC.BRepGProp;
using OCCTK.OCC.BRepOffsetAPI;
using OCCTK.OCC.gp;
using OCCTK.OCC.GProp;
using OCCTK.OCC.Topo;
using OCCTK.Utils;

namespace TestWPF.PipeBending;

public class XYZR
{
    public double X { get; set; }
    public double Y { get; set; }
    public double Z { get; set; }
    public double R { get; set; } = 0.0;

    // 定义隐式转换运算符
    public static implicit operator Pnt(XYZR xyz)
    {
        return new(xyz.X, xyz.Y, xyz.Z);
    }
}

public class YBCR
{
    public double Y { get; set; }
    public double B { get; set; }
    public double C { get; set; }
    public double R { get; set; } = 0.0;
}

public class Pipe
{
    private static readonly ILog log = LogManager.GetLogger(typeof(App));

    public Pipe() { }

    public Pipe(InteractiveContext context)
    {
        Context = context;
    }

    public InteractiveContext Context;

    public Pipe(TShape shape)
    {
        originSTEPShape = shape;
        topoShape = shape;
    }

    #region PipeInfo
    public string Material { get; set; } = "";
    public string Name { get; set; } = "";
    public string? STEPFilePath { get; set; }

    /// <summary>
    /// 管截面有多种类型：圆形、(方形、圆环、方框)
    /// circle, (square, ring, square_ring)
    /// </summary>
    public string Type { get; } = "Circle";

    protected TShape? crossSectionShape;

    /// <summary>
    /// 截面形状
    /// </summary>
    public TShape CrossSectionShape
    {
        get
        {
            if (crossSectionShape == null)
            {
                makeCrossSectionShape();
                return crossSectionShape;
            }
            else
            {
                return crossSectionShape;
            }
        }
    }

    /// <summary>
    /// 中心线由直线和圆弧构成
    /// </summary>
    public TWire? wire;
    public TWire Wire
    {
        get
        {
            if (wire == null)
                makeWireFromYBC();
            return wire;
        }
    }
    protected GProps? GlobalProperties;
    public Pnt Center
    {
        get
        {
            if (GlobalProperties == null)
                GlobalProperties = new();
            BRepGlobalProperties.VolumeProperties(TopoShape, ref GlobalProperties);
            return GlobalProperties.CentreOfMass();
        }
    }
    public Ax1 Moment
    {
        get
        {
            if (GlobalProperties == null)
                GlobalProperties = new();
            BRepGlobalProperties.VolumeProperties(TopoShape, ref GlobalProperties);
            var m = GlobalProperties.MatrixOfInertia();
            return new Ax1(Center, new Dir(m[2, 0], m[2, 1], m[2, 2]));
        }
    }
    private double pipeLength = 0.0;

    /// <summary>
    /// 管长度
    /// </summary>
    public double Length
    {
        get
        {
            if (pipeLength == 0.0)
            {
                CalculatePipeLengthFromYBC();
            }
            return pipeLength;
        }
    }

    /// <summary>
    /// 管接头的样式(首段)
    /// </summary>
    public string FirstFitting { get; set; } = "";

    /// <summary>
    /// 管接头的样式(末段)
    /// </summary>
    public string LastFitting { get; set; } = "";

    /// <summary>
    /// 内半径
    /// </summary>
    public double InnerRadius
    {
        get => OuterRadius - Thickness;
    }

    /// <summary>
    /// 外半径
    /// </summary>
    public double OuterRadius { get; set; }

    /// <summary>
    /// 管径
    /// </summary>
    public double Diameter
    {
        get => OuterRadius * 2;
        set => OuterRadius = value / 2.0;
    }

    /// <summary>
    /// 管壁厚度
    /// </summary>
    public double Thickness { get; set; }

    /// <summary>
    /// 夹持长度
    /// </summary>
    public double ClampLength { get; set; }

    /// <summary>
    /// 首直线段长度
    /// </summary>
    public double FirstSegLength { get; }

    /// <summary>
    /// 最短中间直线段长
    /// </summary>
    public double MinMidSegLength { get; }

    /// <summary>
    /// 末直线段长度
    /// </summary>
    public double LastSegLength { get; }

    /// <summary>
    /// 折弯次数
    /// </summary>
    public double BendingNum { get; }

    private List<XYZR> xyzr = [];

    /// <summary>
    /// 笛卡尔坐标
    /// </summary>
    public List<XYZR> XYZR
    {
        get => xyzr;
        set
        {
            xyzr = value;
            XYZR2StandardXYZR();
            XYZR2YBCR();
        }
    }

    public List<YBCR> ybcr = [];

    /// <summary>
    /// 增量坐标
    /// </summary>
    public List<YBCR> YBCR
    {
        get => ybcr;
        set
        {
            ybcr = value;
            YBCR2XYZR();
        }
    }

    #endregion

    #region Function
    protected void CalculatePipeLengthFromYBC()
    {
        if (ybcr.Count == 0)
            pipeLength = 0.0;
    }

    protected void XYZR2YBCR()
    {
        ybcr.Clear();
        if (xyzr.Count < 3)
        {
            log.Debug($"XYZ数量 {xyzr.Count}。少于3，不合法");
            return;
        }
        double minusY = 0.0;
        for (int i = 0; i < xyzr.Count; ++i)
        {
            double paraY = 0.0;
            double paraB = 0.0;
            double paraC = 0.0;
            double paraR = xyzr[i].R;
            //第一个XYZ，没有值
            if (i == 0)
            {
                continue;
            }
            //最后一个XYZ，只有Y值
            else if (i == xyzr.Count - 1)
            {
                double angle = new Dir(xyzr[i - 2], xyzr[i - 1]).Angle(
                    new Dir(xyzr[i - 1], xyzr[i])
                );
                minusY = xyzr[i - 1].R * Math.Tan(angle / 2);
                paraY = ((Pnt)xyzr[i - 1]).Distance(xyzr[i]) - minusY;
            }
            //中间点才有 B 值
            else
            {
                if (i == 1)
                {
                    double angle = new Dir(xyzr[i - 1], xyzr[i]).Angle(
                        new Dir(xyzr[i], xyzr[i + 1])
                    );
                    paraY = ((Pnt)xyzr[i - 1]).Distance(xyzr[i]) - xyzr[i].R * Math.Tan(angle / 2);
                    paraC = angle;
                }
                else
                {
                    double lastAngle = new Dir(xyzr[i - 2], xyzr[i - 1]).Angle(
                        new Dir(xyzr[i - 1], xyzr[i])
                    );
                    double angle = new Dir(xyzr[i - 1], xyzr[i]).Angle(
                        new Dir(xyzr[i], xyzr[i + 1])
                    );
                    paraC = angle;
                    //求解Y
                    paraY =
                        ((Pnt)xyzr[i - 1]).Distance(xyzr[i])
                        - xyzr[i].R * Math.Tan(angle / 2)
                        - xyzr[i - 1].R * Math.Tan(lastAngle / 2);
                    //求解B
                    Dir lastDir1 = new(xyzr[i - 2], xyzr[i - 1]);
                    Dir lastDir2 = new(xyzr[i - 1], xyzr[i]);
                    Dir lastrotAxis = lastDir1.Crossed(lastDir2);
                    Dir Dir1 = new(xyzr[i - 1], xyzr[i]);
                    Dir Dir2 = new(xyzr[i], xyzr[i + 1]);
                    Dir rotAxis = Dir1.Crossed(Dir2);
                    paraB = rotAxis.AngleWithRef(lastrotAxis, new(xyzr[i], xyzr[i - 1]));
                }
            }
            ybcr.Add(
                new()
                {
                    Y = paraY,
                    B = paraB,
                    C = paraC,
                    R = paraR,
                }
            );
        }
    }

    protected void YBCR2XYZR()
    {
        //int i = 0;//debug
        xyzr.Clear();
        xyzr.Add(
            new()
            {
                X = 0,
                Y = 0,
                Z = 0,
                R = 0
            }
        );
        //x为平移方向，z为旋转轴方向
        Ax2 currentMoveAxis = new(new Pnt(), new(0, 0, 1), new(1, 0, 0));
        foreach (var ybc in ybcr)
        {
            //! 平移Y->旋转B->计算旋转轴（->移动C/2得到点并记录）->旋转C
            //移动Y距离
            Trsf tY = new();
            tY.SetTranslation(currentMoveAxis.XDir.ToVec(ybc.Y));
            currentMoveAxis.Transform(tY);
            if (ybc.R == 0)
            {
                xyzr.Add(
                    new()
                    {
                        X = currentMoveAxis.Location.X,
                        Y = currentMoveAxis.Location.Y,
                        Z = currentMoveAxis.Location.Z,
                        R = ybc.R
                    }
                );
                continue;
            }
            currentMoveAxis.Transformed(tY);
            #region debug
            //APoint debugP = new(currentMoveAxis.Location);
            //Context.Display(debugP, false);
            //if (i == ColorMap.Colors.Count)
            //    i = 0;
            //Context.SetColor(debugP, ColorMap.Colors[i], false);
            #endregion
            //旋转B
            Trsf tB = new();
            tB.SetRotation(new(currentMoveAxis.Location, currentMoveAxis.XDir), ybc.B);
            currentMoveAxis.Transform(tB);
            //旋转轴是临时轴
            Trsf axisT = new();
            axisT.SetTranslation(
                currentMoveAxis.XDir.Crossed(currentMoveAxis.ZDir).Reversed().ToVec(ybc.R)
            );
            Ax1 bendingAxis = new Ax1(currentMoveAxis.Location, currentMoveAxis.ZDir).Transformed(
                axisT
            );
            #region debug
            //AAxis aAxis = new(bendingAxis);
            //Context.Display(aAxis, false);
            //if (i == ColorMap.Colors.Count)
            //    i = 0;
            //Context.SetColor(aAxis, ColorMap.Colors[i], false);
            //i++;
            #endregion
            //移动C/2对应直段的距离
            Trsf tC = new();
            double addedY = ybc.R * Math.Abs(Math.Tan(ybc.C / 2));
            tC.SetTranslation(currentMoveAxis.XDir.ToVec(addedY));
            Pnt xyzPnt = currentMoveAxis.Location.Transformed(tC);
            xyzr.Add(
                new()
                {
                    X = xyzPnt.X,
                    Y = xyzPnt.Y,
                    Z = xyzPnt.Z,
                    R = ybc.R
                }
            );
            Trsf tR = new();
            tR.SetRotation(bendingAxis, ybc.C);
            currentMoveAxis.Transform(tR);
        }
        //Context.UpdateCurrentViewer();
    }

    /// <summary>
    /// 转换为标准化的XYZR坐标
    /// 1. 起点在原点
    /// 2. 第二点在X轴正方向
    /// 3. 第三点的Y为正，Z为0
    /// </summary>
    protected void XYZR2StandardXYZR() { }

    protected void makeWireFromYBC()
    {
        MakeWire maker = new();
        Ax2 currentAxis = new(new Pnt(), new(0, 0, 1), new(1, 0, 0));
        foreach (var ybc in ybcr)
        {
            Pnt lastPnt = (Pnt)currentAxis.Location.Clone();
            //Y
            Trsf tY = new();
            tY.SetTranslation(currentAxis.XDir.ToVec(ybc.Y));
            currentAxis.Transform(tY);
            maker.Add(new MakeEdge(lastPnt, currentAxis.Location));
            if (ybc.R == 0.0)
                continue;
            //B
            Trsf tB = new();
            tB.SetRotation(new(currentAxis.Location, currentAxis.XDir), ybc.B);
            currentAxis.Transform(tB);
            //bendingAxis
            Trsf tAxis = new();
            tAxis.SetTranslation(
                currentAxis.XDir.Crossed(currentAxis.ZDir).Reversed().ToVec(ybc.R)
            );
            Ax1 rotationAxis = new Ax1(currentAxis.Location, currentAxis.ZDir);
            rotationAxis.Transform(tAxis);
            //CR
            lastPnt = (Pnt)currentAxis.Location.Clone();
            Trsf tC = new();
            tC.SetRotation(rotationAxis, ybc.C);
            Circle circle = new(new(rotationAxis.Location, rotationAxis.Direction), ybc.R);
            currentAxis.Transform(tC);
            MakeEdge e = new MakeEdge(circle, lastPnt, currentAxis.Location);
            e.Error(); //错误处理
            maker.Add(new MakeEdge(circle, lastPnt, currentAxis.Location));
        }
        wire = maker;
    }

    protected void makePipeShape()
    {
        MakePipe makePipe = new(Wire, CrossSectionShape);
        topoShape = makePipe;
    }

    protected void makeCrossSectionShape()
    {
        MakeFace makeFace =
            new(
                new MakeWire(
                    new MakeEdge(new Circle(new Ax2(new Pnt(), new Dir(1, 0, 0)), OuterRadius))
                )
            );
        TWire innerCircle = new MakeWire(
            new MakeEdge(new Circle(new Ax2(new Pnt(), new Dir(1, 0, 0)), InnerRadius))
        );
        innerCircle.Reverse();
        makeFace.Add(innerCircle);
        crossSectionShape = makeFace;
    }
    #endregion

    public TShape? originSTEPShape;

    /// <summary>
    /// 将解析的管件起点移动到原点的变换
    /// </summary>
    private Trsf ToOriginTransform { get; set; } = new();
    public TShape? topoShape;
    public TShape TopoShape
    {
        get
        {
            if (topoShape == null)
            {
                if (originSTEPShape == null)
                {
                    makePipeShape();
                }
                else
                {
                    topoShape = originSTEPShape;
                }
            }
            return topoShape;
        }
    }

    private AShape? AISshape = null;
    public AShape AIS
    {
        get
        {
            AISshape ??= new(TopoShape);
            return AISshape;
        }
    }

    #region override
    // 定义隐式转换运算符，将 Pipe 隐式转换为 AShape
    public static implicit operator AShape(Pipe pipe)
    {
        return pipe.AIS;
    }

    public override string ToString()
    {
        return $"Diameter: {Diameter}, Thickness: {Thickness}";
    }
    #endregion
}
