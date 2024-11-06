using System;
using System.Collections.Generic;
using System.Linq;
using log4net;
using OCCTK.OCC.AIS;
using OCCTK.OCC.GeomAbs;
using OCCTK.OCC.gp;
using OCCTK.OCC.TopExp;
using OCCTK.OCC.Topo;
using OCCTK.OCC.TopoAbs;
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
        DebugC = debugContext;
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
        HashSet<Face> mainFaceSet = [];
        GetFaceSet(mainface, ref mainFaceSet);
        log.Info("获取其余平面组");
        RootNode = new RootNode(mainface, mainFaceSet);
        CreateTree();
        for (int i = 1; i < AllLeafNodes.Count + 1; ++i)
        {
            AllLeafNodes[i - 1].Order = i;
        }
        log.Info($"构建折弯树，共{TreeNodes.Count}个节点");
        CombineBendings();
        log.Info($"合并了{0}个节点");
    }

    public TShape Shape { get; private set; }

    /// <summary>
    /// K参数表
    /// </summary>
    private readonly Kparam _Kparam;
    private InteractiveContext? DebugC { get; set; }

    /// <summary>
    /// 板厚度
    /// </summary>
    public double Thickness { get; private set; }

    /// <summary>
    /// 根节点
    /// </summary>
    public RootNode RootNode { get; private set; }

    /// <summary>
    /// 主平面
    /// </summary>
    public Face MainFace => RootNode.MainFace;

    private HashSet<Face> AllFaces { get; set; } = [];
    private HashSet<Edge> AllEdges { get; set; } = [];
    private HashSet<BendingDS> AllBendings { get; set; } = [];

    /// <summary>
    /// 找出的原始折弯
    /// </summary>
    private List<LeafNode> OriginNodes { get; set; } = [];
    public List<LeafNode> AllLeafNodes => GetAllChildNodes(RootNode);

    private List<ClosedHemNode> HemNodes =>
        AllLeafNodes
            .Where(node => node.Bending.Type == BendingType.ClosedHem)
            .Cast<ClosedHemNode>()
            .ToList();
    public List<LeafNode> NodesAfterHem { get; } = [];
    private HashSet<LeafNode> CombinedNodes { get; set; } = [];

    /// <summary>
    /// 树上显示的节点（合并了折弯边和死边后）
    /// </summary>
    public List<NodeDS> TreeNodes => [RootNode, .. AllLeafNodes];
    public Dictionary<TShape, LeafNode> BendingFaceMap
    {
        get
        {
            Dictionary<TShape, LeafNode> map = [];
            foreach (var node in AllLeafNodes)
            {
                foreach (var face in node.Bending.BendingFaces)
                {
                    map[face.TopoFace] = node;
                }
            }
            return map;
        }
    }

    /// <summary>
    /// 获取所有子节点(全深度)
    /// </summary>
    /// <param name="node"></param>
    /// <returns></returns>
    public static List<LeafNode> GetAllChildNodes(NodeDS node)
    {
        List<LeafNode> allNodes = [];
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
            //if (cylinStart.Area < MINAREA)
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
            //+ 获取所有折弯
            if (e.Value < 1e-2)
                continue;
            Thickness = (double)e.Value;
            GetAllBendings();
            if (AllBendings.Count > 0)
                break;
        }
        log.Info($"板厚度为:{Thickness}");
        log.Info($"获取全部折弯：{AllBendings.Count}个");
        #endregion
        //处理死边
        //折弯展开
    }

    private void GetAllBendings()
    {
        List<Face> tempFace = [.. AllFaces];
        for (int i = tempFace.Count - 1; i >= 0; i--)
        {
            Face cylinStart = tempFace[i];
            if (cylinStart.Type != SurfaceType.Cylinder)
                continue;
            if (
                cylinStart.CircleAixs == null
                || cylinStart.CircleAngle == null
                || cylinStart.CircleCenter == null
                || cylinStart.CircleRadius == null
            )
                continue;
            bool found = false;

            foreach (var adj1 in cylinStart.AdjacentFaces.Values)
            {
                if (found)
                    break;
                if (adj1 == cylinStart)
                    continue;
                if (!tempFace.Contains(adj1))
                    continue;
                foreach (var cylin1 in adj1.AdjacentFaces.Values)
                {
                    if (found)
                        break;
                    if (
                        cylin1.Type != SurfaceType.Cylinder
                        || cylin1 == adj1
                        || cylin1 == cylinStart
                    )
                        continue;
                    if (!tempFace.Contains(adj1) && !tempFace.Contains(cylin1))
                        continue;
                    foreach (var adj3 in cylin1.AdjacentFaces.Values)
                    {
                        if (found)
                            break;
                        if (adj3 == cylin1 || adj3 == adj1)
                            continue;
                        if (
                            !tempFace.Contains(adj1)
                            && !tempFace.Contains(cylin1)
                            && !tempFace.Contains(adj3)
                        )
                            continue;
                        foreach (Face cylin2 in adj3.AdjacentFaces.Values)
                        {
                            if (found)
                                break;
                            if (cylin2 == adj3 || cylin2 == cylin1)
                                continue;
                            if (
                                !tempFace.Contains(adj1)
                                && !tempFace.Contains(cylin1)
                                && !tempFace.Contains(adj3)
                                && !tempFace.Contains(cylin2)
                            )
                                continue;
                            if (cylin2 == cylinStart)
                            {
                                //两圆柱面需要有相同的圆心
                                if (cylin1.CircleCenter?.Distance(cylin2.CircleCenter) > LINEAR_TOL)
                                    continue;
                                //半径不相等
                                if (cylin1.CircleRadius == cylin2.CircleRadius)
                                    continue;
                                //半径差刚好为板厚
                                if (
                                    cylin1.CircleRadius.HasValue
                                    && cylin2.CircleRadius.HasValue
                                    && Math.Round(
                                        Math.Abs(
                                            cylin1.CircleRadius.Value - cylin2.CircleRadius.Value
                                        ),
                                        2
                                    ) != Thickness
                                )
                                    continue;
                                HashSet<Face> sectorFaces = [];
                                foreach (var adjCylin1 in cylin1.AdjacentFaces.Values)
                                {
                                    foreach (var adjCylin2 in cylin2.AdjacentFaces.Values)
                                    {
                                        if (adjCylin1 == adjCylin2)
                                        {
                                            sectorFaces.Add(adjCylin1);
                                            break;
                                        }
                                    }
                                }
                                AllBendings.Add(new BendingDS(cylin1, cylin2, sectorFaces));
                                found = true;
                                tempFace.Remove(cylin1);
                                tempFace.Remove(adj3);
                                foreach (var f in sectorFaces)
                                {
                                    tempFace.Remove(f);
                                }
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
        temptFace.RemoveAll(RootNode.FaceSet.Contains);

        //迭代获取子节点
        CreateNode(MainFace, RootNode, ref temptFace);
    }

    private void CreateNode(Face parentMainFace, NodeDS parentNode, ref List<Face> temptFace)
    {
        if (temptFace.Count == 0)
            return;
        foreach (var adj1 in parentMainFace.AdjacentFaces.Values)
        {
            foreach (var b in AllBendings)
            {
                if (!b.CylinderFaces.Contains(adj1))
                    continue;

                //该折弯面(圆柱面)的下一个邻面即为下一个节点的主平面
                foreach (var adj2 in adj1.AdjacentFaces.Values)
                {
                    if (b.BendingFaces.Contains(adj2) || !temptFace.Contains(adj2))
                        continue;
                    //迭代获取所有非折弯面的邻面，构建节点面组
                    HashSet<Face> faceSet = [];
                    GetFaceSet(adj2, ref faceSet);
                    LeafNode? newNode = null;
                    try
                    {
                        if (b.Type == BendingType.VBend)
                        {
                            newNode = new VBendNode(adj2, faceSet, parentNode, b, _Kparam);
                        }
                        else if (b.Type == BendingType.Hem)
                        {
                            newNode = new ClosedHemNode(adj2, faceSet, parentNode, b, _Kparam);
                        }
                    }
                    catch (Exception e)
                    {
                        log.Debug($"节点创建失败\n{e}");
                    }
                    if (newNode == null)
                    {
                        return;
                    }
                    parentNode.Children.Add(newNode);
                    temptFace.RemoveAll(item => faceSet.Contains(item));
                    CreateNode(adj2, newNode, ref temptFace);
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

    private void CombineBendings()
    {
        OriginNodes = AllLeafNodes;
        //! 合并折弯边，首先需要展开所有折弯（只操作折弯轴）
        //! 对包含死边的钣金需要特殊处理，死边之后的折弯与其它折弯合并之后再进行展开合并操作。
        #region 死边操作
        FoldAllBendings();
        //! 死边合并必须在折叠状态下，展开后折弯轴会发生偏移
        foreach (var hemNode in HemNodes)
        {
            foreach (var child in GetAllChildNodes(hemNode))
            {
                NodesAfterHem.Add(child);
            }
            hemNode.Children.Clear();
        }
        foreach (var temp1 in NodesAfterHem)
        {
            foreach (var temp2 in AllLeafNodes)
            {
                //todo 合并的逻辑有问题，压死边会让两个轴的Location有0.1的差距，不能用轴共线的逻辑去处理
                if (
                    temp1.BendingAxis.Location.Distance(temp2.BendingAxis.Location)
                    < temp1.Bending.Thickness
                )
                {
                    if (temp1.BendingAxis.Direction.IsParallel(temp2.BendingAxis.Direction, 1e-4))
                    {
                        temp2.HemNode = temp1;
                        temp1.Order = temp2.Order;
                        break;
                    }
                }
            }
        }
        #endregion

        #region 常规合并
        //! 以折弯展开后的折弯轴作为判断依据
        UnfoldAllBendings();
        List<LeafNode> tempNodes = AllLeafNodes;
        foreach (var node in tempNodes)
        {
            if (CombinedNodes.Contains(node))
                continue;
            foreach (var otherNode in tempNodes)
            {
                if (node == otherNode)
                    continue;
                Ax1 unfoldedNodeAxis = node.BendingAxis.Transformed(node.Location);
                Ax1 unfoldedOtherNodeAxis = otherNode.BendingAxis.Transformed(otherNode.Location);
                //两节点同轴
                if (unfoldedNodeAxis.IsCoaxial(unfoldedOtherNodeAxis, 1e-2, 1e-2))
                {
                    node.CombinedNodes.Add(otherNode);
                    otherNode.Parent.Children.Remove(otherNode);
                    otherNode.Order = node.Order;
                    CombinedNodes.Add(node);
                    CombinedNodes.Add(otherNode);
                }
            }
        }
        #endregion
    }

    private static void GetBendingAfterHem(LeafNode hemNode, ref List<LeafNode> list)
    {
        foreach (var n in hemNode.Children)
        {
            list.Add(n);
            GetBendingAfterHem(n, ref list);
        }
    }

    /// <summary>
    /// 将所有折弯设置为展开
    /// </summary>
    public void UnfoldAllBendings()
    {
        foreach (var node in OriginNodes)
        {
            if (node is LeafNode leafNode)
            {
                leafNode.Unfolded = true;
            }
        }
    }

    /// <summary>
    /// 将所有折弯设置为折叠
    /// </summary>
    public void FoldAllBendings()
    {
        foreach (var node in OriginNodes)
        {
            if (node is LeafNode leafNode)
            {
                leafNode.Unfolded = false;
            }
        }
    }
}
