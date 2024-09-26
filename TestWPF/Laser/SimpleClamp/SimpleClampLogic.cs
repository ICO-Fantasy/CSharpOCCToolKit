using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Net.NetworkInformation;
using System.Security.Cryptography.Pkcs;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms.VisualStyles;
using System.Windows.Shapes;
using System.Windows.Xps.Packaging;
using log4net;
using OCCTK.Extension;
using OCCTK.OCC.AIS;
using OCCTK.OCC.Bnd;
using OCCTK.OCC.BRep;
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

public partial class MyEdge
{
    public double Length
    {
        get { return Math.Sqrt(Math.Pow(Start.X - End.X, 2) + Math.Pow(Start.Y - End.Y, 2)); }
    }
}

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
    /// <summary>
    /// 环结构
    /// </summary>
    public struct Ring
    {
        public Ring() { }

        public Pnt Start = new();
        public Pnt End = new();
        public List<MyEdge> Edges = [];

        // 定义隐式转换运算符
        public static implicit operator List<MyEdge>(Ring platePose)
        {
            return platePose.Edges;
        }

        public void Add(MyEdge edge) => Edges.Add(edge);

        public void Remove(MyEdge edge) => Edges.Remove(edge);

        public void RemoveAt(int i) => Edges.RemoveAt(i);
    }

    private static readonly ILog log = LogManager.GetLogger(typeof(App));

    //横截面切出的环
    private List<Ring> rings = [];

    //原始的底部线段
    private List<MyEdge> buttomEdges = [];

    //移除过短的线段
    private List<MyEdge> filteredEdges = [];

    //修剪线段的两端
    private List<MyEdge> trimedEdges = [];

    //切断过长的线段
    private List<MyEdge> cuttedEdges = [];

    public List<Ring> DebugRings => rings;
    public List<MyEdge> DebugButtomEdges => buttomEdges;
    public List<MyEdge> DebugFilteredEdges => filteredEdges;
    public List<MyEdge> DebugTrimedEdges => trimedEdges;
    public List<MyEdge> DebugCuttedEdges => cuttedEdges;

    #region 得到下端线
    /// <summary>
    /// 从横截面得到边,并分离得到闭合的环
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
        //首先按照order排序，加快分离速度
        allEdges.Sort((edge1, edge2) => edge1.Order.CompareTo(edge2.Order));

        //分离环
        /// <summary>
        /// 从所有边线中构造环
        /// </summary>
        /// <returns></returns>
        static List<Ring> SplitRing(List<MyEdge> edges, double TOL = 5e-2)
        {
            List<Ring> rings = [];
            while (edges.Count > 0)
            {
                Ring aRing = new();
                aRing.Start = edges[0].Start;
                aRing.End = edges[0].End;
                aRing.Add(edges[0]);
                edges.RemoveAt(0); // 移除已处理的边

                while (edges.Count > 0) // 如果还有未处理的边
                {
                    bool foundRing = false; // 标记是否找到环的一部分
                    for (int i = edges.Count - 1; i >= 0; --i)
                    {
                        MyEdge edge = edges[i];

                        // 找到匹配的边后处理
                        if (aRing.Start.Distance(edge.Start) < TOL)
                        {
                            //log.Debug($"分离Ring-点的距离:{aRing.Start.Distance(edge.Start)}");
                            Debug.WriteLine($"分离Ring-点的距离:{aRing.Start.Distance(edge.Start)}");
                            aRing.Add(edge);
                            aRing.Start = edge.End; // 更新起点
                            edges.RemoveAt(i); // 移除已处理的边
                            foundRing = true;
                        }
                        else if (aRing.Start.Distance(edge.End) < TOL)
                        {
                            //log.Debug($"分离Ring-点的距离:{aRing.Start.Distance(edge.End)}");
                            Debug.WriteLine($"分离Ring-点的距离:{aRing.Start.Distance(edge.End)}");
                            aRing.Add(edge);
                            aRing.Start = edge.Start; // 更新起点
                            edges.RemoveAt(i);
                            foundRing = true;
                        }
                        else if (aRing.End.Distance(edge.Start) < TOL)
                        {
                            //log.Debug($"分离Ring-点的距离:{aRing.End.Distance(edge.Start)}");
                            Debug.WriteLine($"分离Ring-点的距离:{aRing.End.Distance(edge.Start)}");
                            aRing.Add(edge);
                            aRing.End = edge.End; // 更新终点
                            edges.RemoveAt(i);
                            foundRing = true;
                        }
                        else if (aRing.End.Distance(edge.End) < TOL)
                        {
                            //log.Debug($"分离Ring-点的距离:{aRing.End.Distance(edge.End)}");
                            Debug.WriteLine($"分离Ring-点的距离:{aRing.End.Distance(edge.End)}");
                            aRing.Add(edge);
                            aRing.End = edge.Start; // 更新终点
                            edges.RemoveAt(i);
                            foundRing = true;
                        }

                        // 如果找到匹配边，继续外层循环
                        if (foundRing)
                        {
                            i = edges.Count; // 重新遍历未处理的边
                            foundRing = false; // 重置foundRing标记，寻找下一个匹配边
                        }
                    }

                    // 如果无法找到新的匹配边，则结束环查找
                    if (!foundRing)
                    {
                        break;
                    }
                }

                rings.Add(aRing); // 将找到的环加入rings列表
            }

            return rings;
        }
        rings = SplitRing(allEdges);
        #endregion
    }

    /// <summary>
    /// 从环中获取下端线
    /// </summary>
    public void GetBottomEdges(double TOL = 5e-2)
    {
        //并行迭代
        Parallel.ForEach(
            rings,
            originRing =>
            {
                // 理论上 Ring 不应该少于4条边
                if (originRing.Edges.Count < 4)
                {
                    //todo 错误处理
                    throw new Exception("环小于四条边");
                }
                //如果工件无厚度，则提取的环为线段
                if (originRing.Start.Distance(originRing.End) > 5e-2)
                {
                    //todo 错误处理
                    throw new Exception("环不闭合");
                }
                Ring ring = new();
                foreach (var originEdge in originRing.Edges)
                {
                    ring.Add(originEdge);
                }
                Ring firstWire = new();
                //从Order最大值开始（反向循环从尾端开始）
                MyEdge? startEdge = ring.Edges.MaxBy(edge => edge.Order);
                firstWire.Add(startEdge);
                firstWire.Start = startEdge.Start;
                firstWire.End = startEdge.End;
                ring.Remove(startEdge);
                //在第一条线两端生长
                bool findAdj = false;
                for (int i = ring.Edges.Count - 1; i >= 0; --i)
                {
                    MyEdge edge = ring.Edges[i];
                    if (firstWire.Start.Distance(edge.End) < TOL)
                    {
                        firstWire.Start = edge.Start;
                        findAdj = true;
                    }
                    if (firstWire.End.Distance(edge.Start) < TOL)
                    {
                        firstWire.End = edge.End;
                        findAdj = true;
                    }
                    if (findAdj)
                    {
                        //添加并删除
                        firstWire.Add(edge);
                        ring.RemoveAt(i);
                        //重新找
                        i = ring.Edges.Count;
                        findAdj = false;
                    }
                }
                //剩余的线作为第二段
                List<MyEdge> secondWire = ring.Edges;
                //去掉与Z平行的边
                Vec Zaxis = new Vec(0, 0, 1);
                firstWire.Edges.RemoveAll(edge =>
                    new Vec(edge.Start, edge.End).IsParallel(Zaxis, Math.PI * 2 / 180)
                );
                secondWire.RemoveAll(edge =>
                    new Vec(edge.Start, edge.End).IsParallel(Zaxis, Math.PI * 2 / 180)
                );
                //比较谁更低，加入buttomEdges
                if (firstWire.Edges.Count == 0 || secondWire.Count == 0)
                {
                    throw new Exception("下端线分离失败");
                }
                double firstWireAvgZ = firstWire.Edges.Average(edge => edge.Start.Z);
                double secondWireAvgZ = secondWire.Average(edge => edge.Start.Z);
                if (firstWireAvgZ < secondWireAvgZ)
                {
                    foreach (var e in firstWire.Edges)
                    {
                        buttomEdges.Add(e);
                    }
                }
                else
                {
                    foreach (var e in secondWire)
                    {
                        buttomEdges.Add(e);
                    }
                }
            }
        );
    }
    #endregion
    #region 处理线段
    /// <summary>
    /// 修剪线段两端
    /// </summary>
    public void TrimEdgeEnds()
    {
        List<MyEdge> originEdges = [];
        //修剪前检查，长度需要大于2*clearances
        foreach (var edge in buttomEdges)
        {
            if (edge.Length < Clearances * 2)
            {
                continue;
            }
            originEdges.Add(edge);
        }
        //修剪线段
        List<MyEdge> trimedEdges = [];
        MyEdge TrimEdgeEnd(MyEdge edge)
        {
            MyEdge trimedEdge;

            Pnt p1 = (Pnt)edge.Start.Clone();
            Pnt p2 = (Pnt)edge.End.Clone();
            double ratio = Clearances / edge.Length;
            if (ratio > 1e-2)
            {
                p1.X = (p1.X + (p2.X - p1.X) * ratio);
                p1.Y = (p1.Y + (p2.Y - p1.Y) * ratio);

                p2.X = (p2.X + (p1.X - p2.X) * ratio);
                p2.Y = (p2.Y + (p1.Y - p2.Y) * ratio);

                TEdge outEdge = TrimEdge(aPiece.myEdge.edge, p1, p2);
                if (outEdge != null)
                {
                    tempPieces.add(VerticalPiece(aPiece.pose, myEdge(outEdge), aPiece.Z));
                }
            }
            else
            {
                //不做改变
                tempPieces.push_back(aPiece);
            }
            return trimedEdge;
        }
        foreach (var edge in originEdges)
        {
            trimedEdges.Add(TrimEdgeEnd(edge));
        }
    }
    #endregion

    #region Display
    public void ShowPlate(InteractiveContext context, bool update) { }

    public void ShowPieces(InteractiveContext context, bool update) { }

    public void Remove() { }
    #endregion
}
