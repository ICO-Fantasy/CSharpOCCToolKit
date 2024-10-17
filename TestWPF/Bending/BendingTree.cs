﻿using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Drawing;
using System.Linq;
using System.Net;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Forms;
using log4net;
using Microsoft.VisualBasic.Logging;
using OCCTK.Extension;
using OCCTK.OCC.AIS;
using OCCTK.OCC.BRep;
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
        InteractiveContext? debugContext = null
    )
    {
        C = debugContext;
        Shape = input;
        GetData();
        if (mainfaceRule == "Area")
        {
            MainFace = AllFaces.MaxBy(x => x.Area) ?? throw new Exception("找不到主平面");
        }
        else
        {
            throw new Exception("不支持的模式");
        }
        log.Info($"主平面为:{MainFace}");
        RootNode = new(MainFace, null, null);
        CreateTree();
    }

    private InteractiveContext? C;
    public TShape Shape { get; private set; }
    public HashSet<Face> AllFaces { get; private set; } = [];
    public HashSet<Edge> AllEdges { get; private set; } = [];
    public HashSet<BendingDS> AllBendings { get; private set; } = [];

    public Node RootNode { get; private set; }
    public List<Node> Nodes => GetAllNodes(RootNode);
    #region 整体属性
    public double Thickness { get; private set; }
    public Face MainFace { get; private set; }
    #endregion
    private static List<Node> GetAllNodes(Node node)
    {
        List<Node> allNodes = [];
        foreach (var n in node.Leafs)
        {
            allNodes.Add(n);
            if (n.Leafs.Count > 0)
            {
                allNodes.AddRange(GetAllNodes(n));
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
            .GroupBy(x => x.Length) // 按值分组
            .OrderByDescending(g => g.Count()) // 按分组数量降序排列
            .Take(3) // 取出现次数最多的前三个
            .Select(g => new { Value = g.Key, Count = g.Count() }); // 返回值和对应的出现次数
        foreach (var e in top3)
        {
            //获取所有折弯
            if (e.Value == null)
                continue;
            Thickness = (double)e.Value;
            GetAllBendings();
            if (AllBendings.Count > 0)
                break;
        }
        log.Info($"板厚度为:{Thickness}");
        #endregion
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
                foreach (var adj2 in adj1.AdjacentFaces.Values)
                {
                    if (found)
                        break;
                    if (adj2.Type != SurfaceType.Cylinder || adj2 == adj1 || adj2 == f)
                        continue;
                    foreach (var adj3 in adj2.AdjacentFaces.Values)
                    {
                        if (found)
                            break;
                        if (adj3 == adj2 || adj3 == adj1)
                            continue;
                        foreach (Face adj4 in adj3.AdjacentFaces.Values)
                        {
                            if (found)
                                break;
                            if (adj4 == adj3 || adj4 == adj2)
                                continue;
                            if (adj4 == f)
                            {
                                //两圆柱面需要有相同的圆心
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
        GetNode(MainFace, RootNode, temptFace);
    }

    private void GetNode(Face mainFace, Node parentNode, List<Face> temptFace)
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
                    Node newNode = new(adj2, b, parentNode) { FaceSet = faceSet };
                    parentNode.Leafs.Add(newNode);
                    temptFace.RemoveAll(item => faceSet.Contains(item));
                    GetNode(adj2, newNode, temptFace);
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
}

public class Node(Face mainFace, BendingDS? bending, Node? parentNode)
{
    public Face MainFace { get; private set; } = mainFace;
    public Node? Parent { get; private set; } = parentNode;
    public BendingDS? Bending { get; private set; } = bending;
    public List<BendingDS> Bendings { get; } = [];
    public bool IsRoot { get; private set; } = bending != null;
    public HashSet<Face> FaceSet { get; set; } = [];
    public List<Node> Leafs { get; set; } = [];
    public Ax1? BendingLine { get; set; }

    #region override

    // 重写 GetHashCode 方法，返回 Index 的哈希码
    public override int GetHashCode()
    {
        int hashCode = Bending?.GetHashCode() ?? 0;
        foreach (var bending in Bendings)
        {
            HashCode.Combine(hashCode, bending);
        }
        return hashCode;
    }
    #endregion
    private List<AShape> AISFaces = [];
    private List<AShape> AISBendingFaces = [];
    private AAxis? AISBendingNormal;
    private AAxis? AISMainFaceNormal;

    public void Show(
        InteractiveContext context,
        bool update,
        bool showBendingNormal,
        bool showMainFaceNormal,
        Color bendingColor,
        Color faceSetColor
    )
    {
        if (AISFaces.Count == 0)
        {
            foreach (Face f in FaceSet)
            {
                AShape ais = new(f.TopoFace);
                AISFaces.Add(ais);
                context.Display(ais, false);
                context.SetColor(ais, faceSetColor, false);
                context.SetTransparency(ais, 0.0, false);
            }
        }
        else
        {
            foreach (AShape ais in AISFaces)
            {
                context.Display(ais, false);
                context.SetColor(ais, faceSetColor, false);
                context.SetTransparency(ais, 0.0, false);
            }
        }
        if (AISBendingFaces.Count == 0)
        {
            if (Bending == null)
                return;
            foreach (Face f in Bending.BendingFaces)
            {
                AShape ais = new(f.TopoFace);
                AISBendingFaces.Add(ais);
                context.Display(ais, false);
                context.SetColor(ais, bendingColor, false);
                context.SetTransparency(ais, 0.0, false);
            }
        }
        else
        {
            foreach (AShape ais in AISBendingFaces)
            {
                context.Display(ais, false);
                context.SetColor(ais, bendingColor, false);
                context.SetTransparency(ais, 0.0, false);
            }
        }
        if (showBendingNormal)
        {
            if (AISBendingNormal == null)
            {
                AISBendingNormal = new(Bending?.Axis);
            }
            context.Display(AISBendingNormal, false);
            context.SetColor(AISBendingNormal, bendingColor, false);
        }
        if (showMainFaceNormal)
        {
            if (AISMainFaceNormal == null)
            {
                AISMainFaceNormal = new(MainFace.Normal);
            }
            context.Display(AISMainFaceNormal, false);
            context.SetColor(AISMainFaceNormal, faceSetColor, false);
        }
        if (update)
        {
            context.UpdateCurrentViewer();
        }
    }

    public void Erase(InteractiveContext context, bool update)
    {
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
        if (update)
        {
            context.UpdateCurrentViewer();
        }
    }
}