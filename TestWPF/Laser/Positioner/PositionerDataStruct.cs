using System.Collections;
using System.Collections.Generic;
using System.Linq;
using OCCTK.OCC.gp;
using TestWPF.Robotics;

namespace TestWPF.Laser.Positioner;

public class AngleMatrix<T> : IEnumerable<T>
{
    // 使用字典来存储角度矩阵的值，支持泛型 T
    private Dictionary<(double dx, double dy), T> data = new();

    // 索引器用于访问角度矩阵的元素
    public T this[double dx, double dy]
    {
        get
        {
            // 检查字典中是否存在给定的索引
            return data.TryGetValue((dx, dy), out T value) ? value : default; // 默认值为 default(T)
        }
        set
        {
            // 在字典中设置或更新给定索引的值
            data[(dx, dy)] = value;
        }
    }

    // 检查是否存在某个索引的值
    public bool ContainsIndex(double dx, double dy)
    {
        return data.ContainsKey((dx, dy));
    }

    // Implement IEnumerable<T>.GetEnumerator()
    public IEnumerator<T> GetEnumerator()
    {
        // Return an enumerator over the values in the dictionary
        return data.Values.GetEnumerator();
    }

    // Implement non-generic IEnumerable.GetEnumerator()
    IEnumerator IEnumerable.GetEnumerator()
    {
        // Call the generic GetEnumerator() implementation
        return GetEnumerator();
    }

    /// <summary>
    /// 从小到大排列的 dx 值
    /// </summary>
    public List<double> Dx
    {
        get { return data.Keys.Select(k => k.dx).Distinct().OrderBy(x => x).ToList(); }
    }

    /// <summary>
    /// 从小到大排列的 dy 值
    /// </summary>
    public List<double> Dy
    {
        get { return data.Keys.Select(k => k.dy).Distinct().OrderBy(y => y).ToList(); }
    }
}

public class Knot
{
    public Knot(
        Ax2 pose,
        FanucRobotConfig robotConfig,
        double dx,
        double dy,
        double diffAngle,
        Dir dir
    )
    {
        OriginPose = pose;
        RobotConfig = robotConfig;
        diffAngleXY = (dx, dy);
        DiffAngle = diffAngle;
        Direction = dir;
    }

    public Dir Direction { get; protected set; }

    /// <summary>
    /// 初始位姿
    /// </summary>
    public Ax2 OriginPose { get; protected set; }

    /// <summary>
    /// 偏置量
    /// </summary>
    public (double, double) diffAngleXY { get; protected set; }

    /// <summary>
    /// 角度差
    /// </summary>
    public double DiffAngle { get; protected set; }

    /// <summary>
    /// 有向邻接矩阵
    /// </summary>
    public Dictionary<Knot, double> AdjacencyMatrix { get; set; } = [];

    /// <summary>
    /// 变位机角度
    /// </summary>
    public List<double> PositionerAngle { get; set; } = [];

    /// <summary>
    /// 逆解角度
    /// </summary>
    public (double, double, double, double, double, double) SixRAngle { get; set; }

    /// <summary>
    /// 机器人姿态
    /// </summary>
    public FanucRobotConfig RobotConfig { get; set; }
}
