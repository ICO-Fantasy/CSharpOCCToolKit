using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Controls;
using MathNet.Numerics.Integration;
using OCCTK.OCC.gp;

namespace TestWPF.Utils;

public class WorkSpaceTree { }

public enum DOF
{
    X,
    Y,
    Z,
    W,
    P,
    R
}

public enum WorkSpaceType
{
    Robot,

    /// <summary>
    /// 外部设备
    /// </summary>
    ExternalMachine,

    /// <summary>
    /// 变位机
    /// </summary>
    Positioner,

    /// <summary>
    /// 导轨
    /// </summary>
    Slider
}

public class Node
{
    /// <summary>
    /// 节点名称
    /// </summary>
    public string Name { get; set; }

    /// <summary>
    /// 节点类型
    /// </summary>
    public WorkSpaceType Type { get; set; }

    public Node Parent { get; set; }
    public List<Node> Children { get; set; }
    public Trsf Transfrom
    {
        get
        {
            return Parent.Transfrom.Multiplied(offset).Multiplied(DOFTransfrom).Multiplied(pose);
        }
    }

    #region 偏置
    /// <summary>
    /// 节点原点偏置
    /// </summary>
    public Trsf offset = new();
    public Pnt OffsetLocation
    {
        get
        {
            (double x, double y, double z) = offset.Translation;
            return new(x, y, z);
        }
    }
    public Tuple<double, double, double> OffsetPosition
    {
        get { return offset.Rotation.ToEuler(EulerSequence.Intrinsic_XYZ); }
    }
    public double OffsetX
    {
        get => OffsetLocation.X;
        set { offset.SetTranslationPart(new(new Pnt(), OffsetLocation.SetX(value))); }
    }
    public double OffsetY
    {
        get => OffsetLocation.Y;
        set { offset.SetTranslationPart(new(new Pnt(), OffsetLocation.SetY(value))); }
    }
    public double OffsetZ
    {
        get => OffsetLocation.Z;
        set { offset.SetTranslationPart(new(new Pnt(), OffsetLocation.SetZ(value))); }
    }
    public double OffsetW
    {
        get => OffsetPosition.Item1;
        set
        {
            offset.SetRotationPart(
                new(value, OffsetPosition.Item2, OffsetPosition.Item3, EulerSequence.Intrinsic_XYZ)
            );
        }
    }
    public double OffsetP
    {
        get => OffsetPosition.Item2;
        set
        {
            offset.SetRotationPart(
                new(OffsetPosition.Item1, value, OffsetPosition.Item3, EulerSequence.Intrinsic_XYZ)
            );
        }
    }
    public double OffsetR
    {
        get => OffsetPosition.Item3;
        set
        {
            offset.SetRotationPart(
                new(OffsetPosition.Item1, OffsetPosition.Item2, value, EulerSequence.Intrinsic_XYZ)
            );
        }
    }
    #endregion

    #region 位姿
    /// <summary>
    /// 节点位姿
    /// </summary>
    private Trsf pose = new();
    public Pnt Location
    {
        get
        {
            (double x, double y, double z) = pose.Translation;
            return new(x, y, z);
        }
    }
    public double X
    {
        get => Location.X;
        set { pose.SetTranslationPart(new(new Pnt(), Location.SetX(value))); }
    }
    public double Y
    {
        get => Location.Y;
        set { pose.SetTranslationPart(new(new Pnt(), Location.SetY(value))); }
    }
    public double Z
    {
        get => Location.Z;
        set { pose.SetTranslationPart(new(new Pnt(), Location.SetZ(value))); }
    }
    public Tuple<double, double, double> Position
    {
        get { return pose.Rotation.ToEuler(EulerSequence.Intrinsic_XYZ); }
    }
    public double W
    {
        get => Position.Item1;
        set
        {
            pose.SetRotationPart(
                new(value, Position.Item2, Position.Item3, EulerSequence.Intrinsic_XYZ)
            );
        }
    }
    public double P
    {
        get => Position.Item2;
        set
        {
            pose.SetRotationPart(
                new(Position.Item1, value, Position.Item3, EulerSequence.Intrinsic_XYZ)
            );
        }
    }
    public double R
    {
        get => Position.Item3;
        set
        {
            pose.SetRotationPart(
                new(Position.Item1, Position.Item2, value, EulerSequence.Intrinsic_XYZ)
            );
        }
    }
    #endregion

    #region 运动
    /// <summary>
    /// 初始值
    /// </summary>
    public double InitialValue { get; set; }

    /// <summary>
    /// 当前值（用于计算动画）
    /// </summary>
    public double CurrentValue => InitialValue + DeltaValue;

    private double deltaValue = 0.0;
    public double DeltaValue
    {
        get { return deltaValue; }
        set
        {
            if (InitialValue + value < Max || InitialValue + value > Min)
            {
                throw new ArgumentException("设置值超过范围");
            }
            deltaValue = value;
        }
    }

    /// <summary>
    /// 最大行程
    /// </summary>
    public double Max { get; set; }

    /// <summary>
    /// 最小行程
    /// </summary>
    public double Min { get; set; }

    /// <summary>
    /// 自由度
    /// </summary>
    public DOF DOF { get; set; }
    public Trsf DOFTransfrom
    {
        get
        {
            Trsf T = new();
            if (DOF == DOF.X)
            {
                T.SetTranslation(new(CurrentValue, 0, 0));
            }
            if (DOF == DOF.Y)
            {
                T.SetTranslation(new(0, CurrentValue, 0));
            }
            if (DOF == DOF.Z)
            {
                T.SetTranslation(new(0, 0, CurrentValue));
            }
            if (DOF == DOF.W)
            {
                T.SetRotation(new(new(), new(1, 0, 0)), CurrentValue);
            }
            if (DOF == DOF.W)
            {
                T.SetRotation(new(new(), new(0, 1, 0)), CurrentValue);
            }
            if (DOF == DOF.W)
            {
                T.SetRotation(new(new(), new(0, 0, 1)), CurrentValue);
            }
            return T;
        }
    }

    #endregion
}
