using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Drawing;
using System.Linq;
using System.Net;
using System.Reflection.Metadata;
using System.Runtime.CompilerServices;
using System.Security.Cryptography.Pkcs;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Data;
using System.Windows.Forms;
using System.Windows.Media.Animation;
using System.Windows.Media.Media3D;
using log4net;
using MathNet.Spatial.Units;
using Microsoft.VisualBasic.Logging;
using OCCTK.Extension;
using OCCTK.OCC.AIS;
using OCCTK.OCC.BRepBuilderAPI;
using OCCTK.OCC.BRepCheck;
using OCCTK.OCC.BRepPrimAPI;
using OCCTK.OCC.GeomAbs;
using OCCTK.OCC.gp;
using OCCTK.OCC.TopExp;
using OCCTK.OCC.Topo;
using OCCTK.OCC.TopoAbs;
using TestWPF.Geometry.Tools;
using TestWPF.Utils;
using Windows.Media.Protection.PlayReady;
using Windows.UI.StartScreen;
using Color = OCCTK.Extension.Color;
using ColorMap = OCCTK.Extension.ColorMap;

namespace TestWPF.Bending;

public class BendingTree
{
    private static readonly ILog log = LogManager.GetLogger(typeof(App));
    private static readonly double MINAREA = 1.0;
    private static readonly double LINEAR_TOL = 1e-4;

    public BendingTree(
        TShape input,
        string mainfaceRule = "Area",
        string? KparamfilePath = null,
        InteractiveContext? debugContext = null
    )
    {
        _Kparam = new(KparamfilePath);
        C = debugContext;
        Shape = input;
        GetData();
        Face mainface;
        if (mainfaceRule == "Area")
        {
            mainface = AllFaces.MaxBy(x => x.Area) ?? throw new Exception("找不到主平面");
        }
        else
        {
            throw new Exception("不支持的模式");
        }
        log.Info($"主平面为:{mainface}");
        RootNode = new(mainface, null, null, _Kparam);
        CreateTree();
        CombineBendings();
    }

    public TShape Shape { get; private set; }
    private Kparam _Kparam;
    private InteractiveContext? C;
    public double Thickness { get; private set; }
    public Face MainFace => RootNode.MainFace;
    public Node RootNode { get; private set; }

    public HashSet<Face> AllFaces { get; private set; } = [];
    public HashSet<Edge> AllEdges { get; private set; } = [];
    public HashSet<BendingDS> AllBendings { get; private set; } = [];
    public List<Node> AllNodes => GetAllChildNodes(RootNode);

    public List<Node> NodesBeforeHem { get; private set; } = [];
    public List<Node> NodesAfterHem { get; private set; } = [];
    public List<Node> OriginNodes { get; private set; } = [];
    public List<Node> CombinedNodes { get; private set; } = [];
    public List<Node> Nodes
    {
        get
        {
            if (NodesAfterHem.Count > 0)
            {
                return AllNodes.Where(node => !NodesAfterHem.Contains(node)).ToList();
            }
            if (CombinedNodes.Count > 0)
            {
                return CombinedNodes;
            }
            else
            {
                return AllNodes;
            }
        }
    }

    /// <summary>
    /// 获取所有子节点(全深度)
    /// </summary>
    /// <param name="node"></param>
    /// <returns></returns>
    private static List<Node> GetAllChildNodes(Node node)
    {
        List<Node> allNodes = [];
        foreach (var n in node.Children)
        {
            allNodes.Add(n);
            if (n.Children.Count > 0)
            {
                allNodes.AddRange(GetAllChildNodes(n));
            }
        }
        return allNodes;
    }

    private void GetData()
    {
        #region 获取所有面、边的信息
        Explorer faceExp = new(Shape, ShapeEnum.FACE);
        for (; faceExp.More(); faceExp.Next())
        {
            TFace tF = faceExp.Current().AsFace();
            Face f = new(tF, Shape);
            //if (f.Area < MINAREA)
            //{
            //    continue; //跳过面积过小的表面
            //}
            AllFaces.Add(f);
        }
        log.Info("解析拓扑信息");
        #endregion

        #region 获取邻接边
        //把所有边放入一个dict，同时把相同边的index改为一致
        List<Edge> tempAllEdges = [];
        foreach (var f in AllFaces)
        {
            foreach (var theE in f.Edges)
            {
                foreach (Edge otherE in tempAllEdges)
                {
                    if (theE.TopoEdge == otherE.TopoEdge)
                    {
                        theE.SetIndex(otherE.Index);
                        break;
                    }
                }
                tempAllEdges.Add((theE));
            }
        }
        //去除相同边
        foreach (Edge e in tempAllEdges)
        {
            AllEdges.Add(e);
        }
        //记录每条边被几个面共用
        foreach (Edge e in AllEdges)
        {
            var edges = tempAllEdges.FindAll(x => x.Index == e.Index);
            List<Face> parentFaces = edges.Select(item => item.Parent).ToList();
            e.SetFaces(parentFaces);
        }
        // 遍历所有边的关联面列表，互相添加相邻面
        foreach (var e in AllEdges)
        {
            // 如果同一条边有多个面，则这些面互为相邻
            if (e.Faces.Count <= 1)
                continue;
            //记录除自身以外的其它面为邻面(通常情况下一个edge只会有两个共用面)
            foreach (var f in e.Faces)
            {
                foreach (var otherF in e.Faces)
                {
                    if (f == otherF)
                        continue;
                    f.AdjacentFaces.TryAdd(e, otherF);
                }
            }
        }
        log.Info("获取邻面");
        #endregion

        log.Info("获取全部折弯");
        #region 计算板厚并获取折弯，计算主平面和主方向
        //! 必须在计算折弯之前计算板厚，板厚是判断折弯面的依据
        // 使用 GroupBy 分组并计算出现次数
        var top3 = AllEdges
            .GroupBy(x => Math.Round(x.Length, 2)) // 按值分组
            .OrderByDescending(g => g.Count()) // 按分组数量降序排列
            .Take(5) // 取出现次数最多的前5个
            .Select(g => new { Value = g.Key, Count = g.Count() }); // 返回值和对应的出现次数
        foreach (var e in top3)
        {
            log.Debug($"尝试板厚:{e.Value}");
            //! 获取所有折弯
            if (e.Value < 1e-2)
                continue;
            Thickness = (double)e.Value;
            GetAllBendings();
            if (AllBendings.Count > 0)
                break;
        }
        log.Info($"板厚度为:{Thickness}");
        #endregion
        //处理死边
        //折弯展开
    }

    private void GetAllBendings()
    {
        List<Face> tempFace = [.. AllFaces];
        for (int i = tempFace.Count - 1; i >= 0; i--)
        {
            Face f = tempFace[i];
            if (f.Type != SurfaceType.Cylinder)
                continue;
            bool found = false;

            foreach (var adj1 in f.AdjacentFaces.Values)
            {
                if (found)
                    break;
                if (adj1 == f)
                    continue;
                if (!tempFace.Contains(adj1))
                    continue;
                foreach (var adj2 in adj1.AdjacentFaces.Values)
                {
                    if (found)
                        break;
                    if (adj2.Type != SurfaceType.Cylinder || adj2 == adj1 || adj2 == f)
                        continue;
                    if (!tempFace.Contains(adj1) && !tempFace.Contains(adj2))
                        continue;
                    foreach (var adj3 in adj2.AdjacentFaces.Values)
                    {
                        if (found)
                            break;
                        if (adj3 == adj2 || adj3 == adj1)
                            continue;
                        if (
                            !tempFace.Contains(adj1)
                            && !tempFace.Contains(adj2)
                            && !tempFace.Contains(adj3)
                        )
                            continue;
                        foreach (Face adj4 in adj3.AdjacentFaces.Values)
                        {
                            if (found)
                                break;
                            if (adj4 == adj3 || adj4 == adj2)
                                continue;
                            if (
                                !tempFace.Contains(adj1)
                                && !tempFace.Contains(adj2)
                                && !tempFace.Contains(adj3)
                                && !tempFace.Contains(adj4)
                            )
                                continue;
                            if (adj4 == f)
                            {
                                //两圆柱面需要有相同的圆心
                                if (adj4.CircleCenter == null)
                                    continue;
                                if (adj2.CircleCenter?.Distance(adj4.CircleCenter) > LINEAR_TOL)
                                    continue;
                                //半径不相等
                                if (adj2.CircleRadius == adj4.CircleRadius)
                                    continue;
                                //半径差刚好为板厚
                                if (
                                    adj2.CircleRadius.HasValue
                                    && adj4.CircleRadius.HasValue
                                    && Math.Round(
                                        Math.Abs(adj2.CircleRadius.Value - adj4.CircleRadius.Value),
                                        2
                                    ) != Thickness
                                )
                                    continue;
                                AllBendings.Add(new BendingDS([adj1, adj2, adj3, adj4]));
                                found = true;
                                tempFace.Remove(adj1);
                                tempFace.Remove(adj2);
                                tempFace.Remove(adj3);
                                tempFace.Remove(adj4);
                                i = tempFace.Count - 1;
                            }
                        }
                    }
                }
            }
        }
    }

    private void CreateTree()
    {
        if (MainFace == null)
            return;
        List<Face> temptFace = new(AllFaces);
        temptFace.RemoveAll(item => AllBendings.SelectMany(b => b.BendingFaces).Contains(item));

        //构建根节点
        HashSet<Face> mainFaceSet = [];
        GetFaceSet(MainFace, ref mainFaceSet);
        RootNode.FaceSet = mainFaceSet;
        temptFace.RemoveAll(item => mainFaceSet.Contains(item));

        //首先找到相邻的折弯面
        CreateNode(MainFace, RootNode, temptFace);
    }

    private void CreateNode(Face mainFace, Node parentNode, List<Face> temptFace)
    {
        if (temptFace.Count == 0)
            return;
        foreach (var adj1 in mainFace.AdjacentFaces.Values)
        {
            foreach (var b in AllBendings)
            {
                if (!b.BendingFaces.Contains(adj1))
                    continue;
                //该折弯面的下一个邻面即为下一个节点的主平面
                foreach (var adj2 in adj1.AdjacentFaces.Values)
                {
                    if (b.BendingFaces.Contains(adj2) || !temptFace.Contains(adj2))
                        continue;
                    //迭代获取所有非折弯面的邻面，构建节点面组
                    HashSet<Face> faceSet = [];
                    GetFaceSet(adj2, ref faceSet);
                    Node? newNode = null;
                    try
                    {
                        newNode = new(adj2, parentNode, b, _Kparam) { FaceSet = faceSet };
                        parentNode.Children.Add(newNode);
                        temptFace.RemoveAll(item => faceSet.Contains(item));
                    }
                    catch (Exception e)
                    {
                        Debug.WriteLine($"节点创建失败\n{e}");
                    }
                    if (newNode == null)
                    {
                        return;
                    }
                    CreateNode(adj2, newNode, temptFace);
                }
            }
        }
    }

    private void GetFaceSet(Face mainFace, ref HashSet<Face> faceSet)
    {
        faceSet.Add(mainFace);
        Stack<Face> templist = new(mainFace.AdjacentFaces.Values);
        while (templist.Count > 0)
        {
            Face f = templist.Pop();
            if (f == null)
                break;
            //排除折弯面
            bool isBendingFace = false;
            foreach (BendingDS bds in AllBendings)
            {
                if (bds.BendingFaces.Contains(f))
                {
                    isBendingFace = true;
                    break;
                }
            }
            if (isBendingFace)
                continue;
            if (faceSet.Contains(f))
                continue;
            GetFaceSet(f, ref faceSet);
        }
    }

    public void UnfoldAllBendings()
    {
        foreach (Node node in OriginNodes)
        {
            if (node.IsRoot)
            {
                continue;
            }
            node.Unfolded = true;
        }
    }

    public void FoldAllBendings()
    {
        foreach (Node node in OriginNodes)
        {
            if (node.IsRoot)
            {
                continue;
            }
            node.Unfolded = false;
        }
    }

    private void ProcessHemData()
    {
        if (AllBendings.Any(bending => bending.Type == BendingType.ClosedHem))
        {
            List<Node> hemNodes = GetAllChildNodes(RootNode)
                .Where(node => node.Bending.Type == BendingType.ClosedHem)
                .ToList();
            List<Node> nodesAfterHem = [];
            foreach (var hembending in hemNodes)
            {
                GetBendingAfterHem(hembending, ref nodesAfterHem);
            }
            NodesAfterHem = nodesAfterHem;
            NodesBeforeHem = GetAllChildNodes(RootNode)
                .Where(node => !NodesAfterHem.Contains(node))
                .Where(node => !hemNodes.Contains(node))
                .ToList();
            //找到和死边后折弯具有完全相同折弯轴的折弯，用于后续同时折弯
            foreach (var nb in NodesBeforeHem)
            {
                foreach (var na in NodesAfterHem)
                {
                    if (
                        na.BendingAxis.IsCoaxial(nb.BendingAxis, 1e-2, 1e-2)
                        && na.BendingAxis.Location.Distance(nb.BendingAxis.Location) < 1e-2
                    )
                    {
                        nb.HemNodes.Add(na);
                    }
                }
            }
        }
    }

    private static void GetBendingAfterHem(Node node, ref List<Node> list)
    {
        foreach (var n in node.Children)
        {
            list.Add(n);
            GetBendingAfterHem(n, ref list);
        }
    }

    public void CombineBendings()
    {
        //保留原始树
        OriginNodes = AllNodes;
        //! 合并折弯边，首先需要展开所有折弯（只操作折弯轴）
        //! 对包含死边的钣金需要特殊处理，死边之后的折弯与其它折弯合并之后再进行展开合并操作。
        #region 死边操作


        #endregion
        #region 常规合并
        //以折弯展开后的折弯轴作为判断依据
        UnfoldAllBendings();
        foreach (var node in Nodes)
        {
            bool sameBend = false;
            foreach (var otherNode in Nodes)
            {
                if (node == otherNode)
                    continue;
                //必须检查node是否还在树结构中
                if (!AllNodes.Contains(node))
                    continue;
                Ax1 unfoldedNodeAxis = node.BendingAxis.Transformed(node.Location);
                Ax1 unfoldedOtherNodeAxis = otherNode.BendingAxis.Transformed(otherNode.Location);
                if (unfoldedNodeAxis.IsCoaxial(unfoldedOtherNodeAxis, 1e-2, 1e-2))
                {
                    sameBend = true;
                    node.CombinedNodes.Add(otherNode);
                    otherNode.Parent.Children.Remove(otherNode);
                }
            }
            if (sameBend)
            {
                CombinedNodes.Add(node);
            }
        }
        #endregion
    }
}

public class Node
{
    public Node(Face mainFace, Node? parent, BendingDS? bending, Kparam kparam)
    {
        MainFace = mainFace;
        Parent = parent;
        Bending = bending;
        IsRoot = bending == null;
        if (parent == null || bending == null)
            return;
        #region 计算展开长度
        KRadius = kparam.GetKRadius(
            bending.InnerFace.CircleRadius ?? throw new Exception("内圆柱面半径为空"),
            bending.Thickness
        );
        if (bending.Type == BendingType.VBend)
        {
            BendingAngle = Math.PI - bending.Angle;
            FlatLength = KRadius * bending.Angle;
        }
        else if (bending.Type == BendingType.ClosedHem)
        {
            BendingAngle = Math.PI;
            //死边的K值暂时按1.2算
            FlatLength = (1.2 * bending.Thickness);
        }
        Debug.WriteLine(
            $"死边计算：{Math.Round((double)bending.InnerFace.CircleRadius, 2)}|{Math.Round((double)KRadius, 2)}|{((double)BendingAngle).ToDegrees(1)}|{FlatLength}"
        );
        #endregion
        #region 计算辅助信息
        foreach (Edge e in bending.InnerFace.Edges)
        {
            //理论上只会有两根直线
            if (e.Type != CurveType.Line)
                continue;
            if (parent.MainFace.Edges.Contains(e))
            {
                InnerFatherEdge = e;
                continue;
            }
            InnerChildEdge = e;
        }
        //如果没有，则主平面对应外圆柱
        if (InnerFatherEdge == null || InnerChildEdge == null)
        {
            foreach (Edge e in bending.OutterFace.Edges)
            {
                //理论上只会有两根直线
                if (e.Type != CurveType.Line)
                    continue;
                if (parent.MainFace.Edges.Contains(e))
                {
                    OutterFatherEdge = e;
                    continue;
                }
                OutterChildEdge = e;
            }
        }
        if (InnerFatherEdge == null || InnerChildEdge == null)
        {
            if (OutterFatherEdge == null || OutterChildEdge == null)
                throw new ArgumentException("折弯边检查失败");
        }
        #endregion
        #region 计算折弯轴和方向
        BendingAxis = bending.Axis;
        Trsf rotT = new();
        rotT.SetRotation(bending.Axis, bending.Angle);
        Edge child,
            father;
        if (InnerChildEdge != null && InnerFatherEdge != null)
        {
            child = InnerChildEdge;
            father = InnerFatherEdge;
        }
        else if (OutterChildEdge != null && OutterFatherEdge != null)
        {
            child = OutterChildEdge;
            father = OutterFatherEdge;
        }
        else
        {
            throw new ArgumentException("折弯边检查失败");
        }
        (Pnt p1, Pnt p2) = BrepGeomtryTools.GetEdgeEndPoints(child.TopoEdge);
        (Pnt p3, Pnt p4) = BrepGeomtryTools.GetEdgeEndPoints(father.TopoEdge);
        Dir parentDir = (Dir)parent.MainFace.Normal.Direction.Clone();
        Dir myDir = (Dir)mainFace.Normal.Direction.Clone();
        Dir RotDir = myDir.Transformed(rotT);
        Debug.WriteLine($"dot:{RotDir.Dot(parentDir)}");
        if (RotDir.IsParallel(parentDir, 1e-2) && RotDir.Dot(parentDir) > 0)
        {
            BendingAxis.Reverse();
        }
        DebugBendingAxis = new(BendingAxis, (double)Bending.Length / 2);
        #endregion
        #region 计算展开方向
        UnfoldingDirection = parent.MainFace.Normal.Direction.Crossed(new(p1, p2));
        if (UnfoldingDirection.Dot(bending.Normal.Direction) > 0)
        {
            UnfoldingDirection.Reverse();
        }
        var edge = InnerFatherEdge?.TopoEdge ?? OutterFatherEdge?.TopoEdge;
        Pnt local = BrepGeomtryTools.GetEdgeMidlePoint(edge);
        AISUnfoldingLocation = new(new MakeSphere(local, 1));
        AISUnfoldingDirection = new(new(local, UnfoldingDirection), (double)FlatLength);
        #endregion
        #region 计算展开变换
        Trsf T = new();
        T.SetTranslation(UnfoldingDirection.ToVec((double)FlatLength));
        Trsf R = new();
        //展开方向与弯曲方向相反
        R.SetRotation(BendingAxis.Reversed(), (double)bending.Angle);
        //先平移再旋转
        UnfoldTransform = T.Multiplied(R);
        #endregion
        //! 计算折弯过程中的扇形面
        //foreach ((double angle, Ax1 axis) in GetProcessBendAxis(5))
        //{
        //    AAxis aaxis = new(axis);
        //    DebugSectorAxis.Add(aaxis);
        //    CreateSectors(angle, axis);
        //}
        DebugSectorAxis.Add(new(BendingAxis));
    }

    #region 折弯展开
    public Trsf Location
    {
        get
        {
            if (Unfolded)
            {
                return Parent?.Location.Multiplied(UnfoldTransform) ?? UnfoldTransform;
            }
            else
            {
                return new();
            }
        }
    }
    public bool Unfolded { get; set; } = false;
    public Trsf UnfoldTransform { get; private set; } = new();

    /// <summary>
    /// 内圆柱面上靠近父节点的线段
    /// </summary>
    private Edge? InnerFatherEdge;

    /// <summary>
    /// 内圆柱面上远离父节点的线段
    /// </summary>
    private Edge? InnerChildEdge;

    /// <summary>
    /// 外圆柱面上靠近父节点的线段
    /// </summary>
    private Edge? OutterFatherEdge;

    /// <summary>
    /// 外圆柱面上远离父节点的线段
    /// </summary>
    private Edge? OutterChildEdge;
    private readonly double? KRadius;

    /// <summary>
    /// 折弯角度为内圆弧角，与折弯展开的角度之和为 π
    /// </summary>
    public double? BendingAngle { get; private set; }
    public double? FlatLength { get; private set; }

    /// <summary>
    /// 折弯线长度（取圆柱面两直线中最长的一条，用于计算折弯力）
    /// </summary>
    public double? BendingLength => Bending.Length;
    public Dir? UnfoldingDirection { get; private set; }

    /// <summary>
    /// 折弯轴为右手坐标轴，旋转方向与折弯方向保持一致
    /// </summary>
    public Ax1? BendingAxis { get; private set; }
    public AAxis DebugBendingAxis { get; private set; }

    /// <summary>
    /// 角度和对应的扇形旋转体
    /// </summary>
    public Dictionary<double, TShape> Sectors { get; private set; } = [];
    public List<AAxis> DebugSectorAxis { get; private set; } = [];
    public AAxis DebugSectorDir { get; private set; }

    public List<(double, Ax1)> GetProcessBendAxis(int intervals)
    {
        //todo 可能为空的引用需要错误处理
        List<(double, Ax1)> processBendAxis = [];
        double tempA = 0.0;
        double step = (double)BendingAngle / (double)intervals;

        #region 计算轴移动方向
        //! 从父节点直接找可能出现错误
        //! 改为从八个点中找
        //任取一个为基准点
        Pnt? p1 = null;
        Pnt? p2 = null;
        Pnt? p41 = null;
        Pnt? p42 = null;
        bool outter = false;
        if (InnerFatherEdge != null)
        {
            p1 = InnerFatherEdge.Points[0];
            (p41, p42) = (InnerChildEdge.Points[0], InnerChildEdge.Points[1]);
            outter = false;
        }
        else if (OutterFatherEdge != null)
        {
            p1 = OutterFatherEdge.Points[0];
            (p41, p42) = (OutterChildEdge.Points[0], OutterChildEdge.Points[1]);
            outter = true;
        }
        if (p1 == null)
            throw new Exception("找不到展开向量的基准点");
        //+ 找扇形面的其余三点(有可能超过3个点)
        List<Pnt> face1Pnts = [];
        List<Pnt> face2Pnts = [];
        List<Pnt> eightPoints = [];
        //先找到扇形面的所有点（理论上有8个）
        foreach (var f in Bending.CylinderFaces)
        {
            foreach (var e in f.Edges)
            {
                foreach (var p in e.Points)
                {
                    if (!eightPoints.Any(pp => pp.Distance(p) < 1e-1))
                    {
                        eightPoints.Add(p);
                    }
                }
            }
        }
        //找到平面1的所有点(理论上有4个)，筛选出和扇形面点重合的点
        List<Pnt> t1 = [];
        foreach (var e in Bending.PlaneFaces[0].Edges)
        {
            foreach (var p in e.Points)
            {
                if (!t1.Any(pp => pp.Distance(p) < 1e-2))
                {
                    t1.Add(p);
                }
            }
        }
        //筛选相同点(理论上有4个)
        foreach (var p in t1)
        {
            foreach (var pp in eightPoints)
            {
                if (p.Distance(pp) < 5e-2)
                {
                    face1Pnts.Add(p);
                }
            }
        }
        //找到平面2的所有点(理论上有4个)，筛选出和扇形面点重合的点
        List<Pnt> t2 = [];
        foreach (var e in Bending.PlaneFaces[1].Edges)
        {
            foreach (var p in e.Points)
            {
                if (!t2.Any(pp => pp.Distance(p) < 1e-2))
                {
                    t2.Add(p);
                }
            }
        }
        //筛选相同点(理论上有4个)
        foreach (var p in t2)
        {
            foreach (var pp in eightPoints)
            {
                if (p.Distance(pp) < 5e-2)
                {
                    face2Pnts.Add(p);
                }
            }
        }

        if (face1Pnts.Any(p => p.Distance(p1) < 1e-2))
        {
            if (face1Pnts.Count != 4)
                throw new Exception("扇形面对应的点不为4");
            Pnt? p4 = null;
            for (int i = face1Pnts.Count - 1; i >= 0; i--)
            {
                var p = face1Pnts[i];

                if (p.Distance(p1) < 1e-2)
                {
                    face1Pnts.RemoveAt(i);
                }
                else if (p.Distance(p41) < 1e-2)
                {
                    p4 = p;
                    face1Pnts.RemoveAt(i);
                }
                else if (p.Distance(p42) < 1e-2)
                {
                    p4 = p;
                    face1Pnts.RemoveAt(i);
                }
            }
            if (face1Pnts.Count != 2 || p4 == null)
            {
                throw new Exception("找不到另一个扇形面点");
            }
            //+ 四个点构建一个面，找到和方向对应的另一个点
            MakeWire maker1 =
                new(
                    [
                        new MakeEdge(p1, p4),
                        new MakeEdge(p4, face1Pnts[0]),
                        new MakeEdge(face1Pnts[0], face1Pnts[1]),
                        new MakeEdge(face1Pnts[1], p1)
                    ]
                );
            Analyzer ana1 = new(new MakeFace(maker1));
            if (!ana1.IsValid())
            {
                MakeWire maker2 =
                    new(
                        [
                            new MakeEdge(p1, p4),
                            new MakeEdge(p4, face1Pnts[1]),
                            new MakeEdge(face1Pnts[1], face1Pnts[0]),
                            new MakeEdge(face1Pnts[0], p1)
                        ]
                    );
                Analyzer ana2 = new(new MakeFace(maker2));
                if (ana2.IsValid())
                {
                    p2 = face1Pnts[0];
                }
                else
                {
                    throw new Exception("找不到扇形面对应点p2");
                }
            }
            else
            {
                p2 = face1Pnts[1];
            }
        }

        if (face2Pnts.Any(p => p.Distance(p1) < 1e-2))
        {
            if (face2Pnts.Count != 4)
                throw new Exception("扇形面对应的点不为4");
            Pnt? p4 = null;
            for (int i = face2Pnts.Count - 1; i >= 0; i--)
            {
                var p = face2Pnts[i];

                if (p.Distance(p1) < 1e-2)
                {
                    face2Pnts.RemoveAt(i);
                }
                else if (p.Distance(p41) < 1e-2)
                {
                    p4 = p;
                    face2Pnts.RemoveAt(i);
                }
                else if (p.Distance(p42) < 1e-2)
                {
                    p4 = p;
                    face2Pnts.RemoveAt(i);
                }
            }
            if (face2Pnts.Count != 2 || p4 == null)
            {
                throw new Exception("找不到另一个扇形面点");
            }
            //+ 四个点构建一个面，找到和方向对应的另一个点
            MakeWire maker1 =
                new(
                    [
                        new MakeEdge(p1, p4),
                        new MakeEdge(p4, face2Pnts[0]),
                        new MakeEdge(face2Pnts[0], face2Pnts[1]),
                        new MakeEdge(face2Pnts[1], p1)
                    ]
                );
            Analyzer ana1 = new(new MakeFace(maker1));
            if (!ana1.IsValid())
            {
                MakeWire maker2 =
                    new(
                        [
                            new MakeEdge(p1, p4),
                            new MakeEdge(p4, face2Pnts[1]),
                            new MakeEdge(face2Pnts[1], face2Pnts[0]),
                            new MakeEdge(face2Pnts[0], p1)
                        ]
                    );
                Analyzer ana2 = new(new MakeFace(maker2));
                if (ana2.IsValid())
                {
                    p2 = face2Pnts[0];
                }
                else
                {
                    throw new Exception("找不到扇形面对应点p2");
                }
            }
            else
            {
                p2 = face2Pnts[1];
            }
        }

        if (p2 == null)
            throw new Exception("展开向量创建失败，另一个点为空");
        //p1为外侧点，指向圆心
        if (!outter)
        {
            (p1, p2) = (p2, p1);
        }
        if (p1 == null || p2 == null)
            throw new Exception("展开向量创建失败，另一个点为空");
        Vec vec = new Vec(p1, p2).Normalized();
        double l = Bending.Thickness * 1.5;
        if (Bending.Thickness == 0.0)
        {
            l = 10.0;
        }
        DebugSectorDir = new(new(p1, new(vec)), l);
        #endregion
        while (tempA < BendingAngle)
        {
            tempA += step;
            double newR = (double)FlatLength / tempA - Bending.Thickness / 2;
            double addR = newR - (double)Bending.InnerFace.CircleRadius;
            Vec addvec = vec.Multiplied(addR);
            Trsf sf = new();
            sf.SetTranslation(addvec);
            Ax1 newRotateAxis = BendingAxis.Transformed(sf);
            processBendAxis.Add((tempA, newRotateAxis));
        }
        return processBendAxis;
    }

    public void CreateSectors(double angle, Ax1 axis)
    {
        List<TFace> sectors = [];
        //+ 构建靠近父节点的平面
        if (InnerFatherEdge == null || OutterFatherEdge == null)
            return;
        (Pnt p1, Pnt p2) = BrepGeomtryTools.GetEdgeEndPoints(InnerFatherEdge.TopoEdge);
        (Pnt p3, Pnt p4) = BrepGeomtryTools.GetEdgeEndPoints(OutterFatherEdge.TopoEdge);

        MakePolygon polygonBuilder = new(p1, p2, p3, p4, true);
        Analyzer ana = new(polygonBuilder);
        if (!ana.IsValid())
        {
            polygonBuilder = new(p1, p2, p4, p3, true);
        }
        MakeFace makeface = new(polygonBuilder.Wire());
        TShape sector = new MakeRevol(makeface, axis, angle);
        Sectors.Add(angle, sector);
        //try
        //{
        //}
        //catch (Exception ex)
        //{
        //    //! Debug
        //    Trsf testR = new();
        //    testR.SetRotation(axis, angle);
        //    Transform testF = new(makeface, testR);
        //    Sectors.Add(angle, testF);
        //}
    }

    #endregion

    public Face MainFace { get; private set; }
    public Node? Parent { get; private set; }
    public BendingDS? Bending { get; private set; }
    public bool IsRoot { get; private set; }

    /// <summary>
    /// 不参与折弯的面组
    /// </summary>
    public HashSet<Face> FaceSet { get; set; } = [];
    public List<Node> Children { get; set; } = [];

    #region VBend
    /// <summary>
    /// 合并的其它折弯
    /// </summary>
    public List<Node> CombinedNodes { get; } = [];
    #endregion
    #region ClosedHem
    /// <summary>
    /// 同时折弯的节点
    /// </summary>
    public List<Node> HemNodes { get; set; } = [];
    #endregion

    #region override

    // 重写 GetHashCode 方法，返回 Index 的哈希码
    public override int GetHashCode()
    {
        int hashCode = Bending?.GetHashCode() ?? 0;
        foreach (var bending in CombinedNodes)
        {
            HashCode.Combine(hashCode, bending);
        }
        return hashCode;
    }

    public override string ToString()
    {
        return MainFace.ToString();
    }
    #endregion

    #region 显示
    private List<AShape> AISFaces = [];
    private List<AShape> AISBendingFaces = [];
    private AAxis? AISBendingNormal;
    private AAxis? AISMainFaceNormal;
    private AShape? AISUnfoldingLocation;
    private AAxis? AISUnfoldingDirection;

    public void Show(
        InteractiveContext context,
        bool update,
        bool showBendingNormal,
        bool showMainFaceNormal,
        Color bendingColor
    )
    {
        foreach (var node in CombinedNodes)
        {
            node.Show(context, update, showBendingNormal, showMainFaceNormal, bendingColor);
        }
        if (AISFaces.Count == 0)
        {
            foreach (Face f in FaceSet)
            {
                AShape ais = new(f.TopoFace);
                ais.SetLocalTransformation(Location);
                AISFaces.Add(ais);
                context.Display(ais, false);
                context.SetColor(ais, bendingColor, false);
                context.SetTransparency(ais, 0.0, false);
            }
        }
        else
        {
            foreach (AShape ais in AISFaces)
            {
                ais.SetLocalTransformation(Location);
                context.Display(ais, false);
                context.SetColor(ais, bendingColor, false);
                context.SetTransparency(ais, 0.0, false);
            }
        }
        if (AISBendingFaces.Count == 0)
        {
            if (Bending != null && Unfolded == false)
            {
                foreach (Face f in Bending.BendingFaces)
                {
                    AShape ais = new(f.TopoFace);
                    AISBendingFaces.Add(ais);
                    context.Display(ais, false);
                    context.SetColor(ais, bendingColor, false);
                    context.SetTransparency(ais, 0.0, false);
                }
            }
        }
        else
        {
            if (Unfolded == false)
            {
                foreach (AShape ais in AISBendingFaces)
                {
                    context.Display(ais, false);
                    context.SetColor(ais, bendingColor, false);
                    context.SetTransparency(ais, 0.0, false);
                }
            }
        }
        if (showBendingNormal && Bending != null && Unfolded == false)
        {
            AISBendingNormal ??= new(Bending?.Normal, 5);
            context.Display(AISBendingNormal, false);
            context.SetColor(AISBendingNormal, bendingColor, false);
            if (AISUnfoldingDirection != null)
            {
                context.Display(AISUnfoldingDirection, false);
                context.Display(AISUnfoldingLocation, false);
                context.SetColor(AISUnfoldingDirection, bendingColor, false);
                context.SetColor(AISUnfoldingLocation, bendingColor, false);
            }
        }
        if (showMainFaceNormal)
        {
            AISMainFaceNormal ??= new(MainFace.Normal, 10);
            AISMainFaceNormal.SetLocalTransformation(Location);
            context.Display(AISMainFaceNormal, false);
            context.SetColor(AISMainFaceNormal, bendingColor, false);
        }
        #region Debug
        Bending?.InnerFace.DebugShow(context, bendingColor);
        if (DebugBendingAxis != null)
        {
            context.Display(DebugBendingAxis, false);
            context.SetColor(DebugBendingAxis, bendingColor, false);
        }
        #endregion
        if (update)
        {
            context.UpdateCurrentViewer();
        }
    }

    public void Erase(InteractiveContext context, bool update)
    {
        foreach (var node in CombinedNodes)
        {
            node.Erase(context, update);
        }

        if (AISFaces.Count != 0)
        {
            foreach (AShape f in AISFaces)
            {
                context.Erase(f, false);
            }
        }
        if (AISBendingFaces.Count != 0)
        {
            foreach (AShape f in AISBendingFaces)
            {
                context.Erase(f, false);
            }
        }
        if (AISBendingNormal != null)
        {
            context.Erase(AISBendingNormal, false);
        }
        if (AISMainFaceNormal != null)
        {
            context.Erase(AISMainFaceNormal, false);
        }
        if (AISUnfoldingLocation != null)
        {
            context.Erase(AISUnfoldingLocation, false);
        }
        if (AISUnfoldingDirection != null)
        {
            context.Erase(AISUnfoldingDirection, false);
        }

        foreach (var sector in AISSectors)
        {
            context.Erase(sector, false);
            context.Erase(DebugSectorDir, false);
        }
        #region Debug
        Bending?.InnerFace.DebugErase();
        foreach (var axis in DebugSectorAxis)
        {
            context.Erase(axis, false);
        }
        if (DebugBendingAxis != null)
        {
            context.Erase(DebugBendingAxis, false);
        }
        #endregion
        if (update)
        {
            context.UpdateCurrentViewer();
        }
    }

    private List<AShape> AISSectors = [];

    public void ShowSectors(InteractiveContext context, bool update)
    {
        if (AISSectors.Count == 0)
        {
            foreach ((double angle, TShape sector) in Sectors)
            {
                AISSectors.Add(new(sector));
            }
        }
        Trsf t = new();
        for (int i = 0; i < AISSectors.Count; i++)
        {
            AShape? sector = AISSectors[i];
            Trsf up = new(new Pnt(), new Pnt(0, 0, 5));
            //t = t.Multiplied(up);
            sector.SetLocalTransformation(t);
            context.Display(sector, false);
            context.SetColor(sector, ColorMap.Colors[i], false);
        }
        if (DebugSectorAxis.Count > 0)
        {
            for (int i = 0; i < DebugSectorAxis.Count; i++)
            {
                context.Display(DebugSectorAxis[i], false);
                context.SetColor(DebugSectorAxis[i], ColorMap.Colors[i], false);
            }
            context.Display(DebugSectorDir, false);
            context.SetColor(DebugSectorDir, ColorMap.Black, false);
        }
    }

    #endregion
}
