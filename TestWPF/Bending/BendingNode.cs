using System;
using System.Collections.Generic;
using System.Linq;
using OCCTK.OCC.AIS;
using OCCTK.OCC.BRepBuilderAPI;
using OCCTK.OCC.BRepCheck;
using OCCTK.OCC.BRepPrimAPI;
using OCCTK.OCC.GeomAbs;
using OCCTK.OCC.gp;
using OCCTK.OCC.Graphic3d;
using OCCTK.OCC.TopExp;
using OCCTK.OCC.Topo;
using OCCTK.OCC.TopoAbs;
using OCCTK.Tool;
using TestWPF.Geometry.Tools;
using Color = OCCTK.Extension.Color;
using ColorMap = OCCTK.Extension.ColorMap;

namespace TestWPF.Bending;

public class NodeDS
{
    public NodeDS(Face mainFace, HashSet<Face> faceSet)
    {
        _mainFace = mainFace;
        AISMainFaceNormal = new(mainFace.Normal);

        FaceSet = faceSet;
        AISFaceSet = new(
            new CompoundMaker(faceSet.Select(f => f.TopoFace).Cast<TShape>().ToList())
        );
        AISShapes.Add(AISMainFaceNormal);
        AISShapes.Add(AISFaceSet);
    }

    /// <summary>
    /// 节点的基面（与折弯处相邻）
    /// </summary>
    private Face _mainFace;
    public Face MainFace
    {
        get => _mainFace;
    }

    /// <summary>
    /// 不参与折弯的面组
    /// </summary>
    public HashSet<Face> FaceSet { get; set; } = [];
    public List<LeafNode> Children { get; protected set; } = [];
    public virtual Trsf Location { get; set; } = new();

    #region override
    // 重写 GetHashCode 方法，返回 Index 的哈希码
    public override int GetHashCode()
    {
        return MainFace.GetHashCode();
    }

    public override string ToString()
    {
        return MainFace.ToString();
    }
    #endregion

    #region 显示
    protected AAxis AISMainFaceNormal;
    protected AShape AISFaceSet;

    /// <summary>
    /// 记录已经在画布上显示的对象
    /// </summary>
    protected List<InteractiveObject> AISShapes { get; set; } = [];

    //public void Show(InteractiveContext context, bool update, bool showFaceNormal) { }

    public void RemoveSelf(bool update)
    {
        InteractiveContext? context = null;
        foreach (var ais in AISShapes)
        {
            ais.RemoveSelf(false);
            if (context == null && ais.HasInteractiveContext())
            {
                context = ais.GetContext();
            }
        }
        if (this is LeafNode leaf)
        {
            foreach (var node in leaf.CombinedNodes)
            {
                node.RemoveSelf(false);
            }
        }
        if (update)
        {
            context?.UpdateCurrentViewer();
        }
    }

    #endregion
}

public class RootNode : NodeDS
{
    public RootNode(Face mainFace, HashSet<Face> faceSet)
        : base(mainFace, faceSet) { }

    /// <summary>
    /// 在此之前需要调用EraseAll方法
    /// </summary>
    /// <param name="context"></param>
    /// <param name="update"></param>
    public void Show(InteractiveContext context, bool update)
    {
        #region 简化代码
        Action<InteractiveObject> Display = obj =>
        {
            context.Display(obj, false);
        };
        Action<InteractiveObject> DisplayAndSetColor = obj =>
        {
            context.Display(obj, false);
            context.SetColor(obj, ColorMap.Red, false);
        };
        #endregion

        DisplayAndSetColor(AISMainFaceNormal);
        DisplayAndSetColor(AISFaceSet);
        if (update)
        {
            context.UpdateCurrentViewer();
        }
    }
}

public class LeafNode : NodeDS
{
    public LeafNode(Face mainFace, HashSet<Face> faceSet, NodeDS parent, BendingDS bending)
        : base(mainFace, faceSet)
    {
        Parent = parent;
        Bending = bending;
        CalculateAuxiliaryInfo();
        #region 计算折弯轴和方向
        BendingAxis = bending.Axis;
        Trsf rotT = new();
        rotT.SetRotation(bending.Axis, bending.Angle);
        Dir parentDir = (Dir)parent.MainFace.Normal.Direction.Clone();
        Dir myDir = (Dir)mainFace.Normal.Direction.Clone();
        Dir RotDir = myDir.Transformed(rotT);
        if (RotDir.IsParallel(parentDir, 1e-2) && RotDir.Dot(parentDir) > 0)
        {
            BendingAxis.Reverse();
        }
        #endregion
        #region 计算展开方向
        UnfoldingDirection = parent.MainFace.Normal.Direction.Crossed(
            new(LeftInnerFatherPoint, RightInnerChildPoint)
        );
        if (UnfoldingDirection.Dot(bending.Normal.Direction) > 0)
        {
            UnfoldingDirection.Reverse();
        }

        #endregion
        #region 显示
        CompoundMaker maker =
            new(bending.BendingFaces.Select(f => f.TopoFace).Cast<TShape>().ToList());
        AISBendingFaces = new(maker);
        AISShapes.Add(AISBendingFaces);
        AISBendingNormal = new(bending.Normal, 10);
        AISShapes.Add(AISBendingNormal);
        AISBendingAxis = new(BendingAxis, (double)Bending.Length / 2);
        AISShapes.Add(AISBendingAxis);
        #endregion
    }

    #region 折弯展开

    private void CalculateAuxiliaryInfo()
    {
        #region 找到内外四条线
        //+ 假设主平面对应内圆柱
        foreach (Edge e in Bending.InnerFace.Edges)
        {
            //理论上只会有两根直线
            if (e.Type != CurveType.Line)
                continue;
            //父节点主平面和圆柱面共享直线
            foreach (var pe in Parent.MainFace.Edges)
            {
                if (e == pe)
                {
                    InnerFatherEdge = e;
                    break;
                }
            }
            //当前节点主平面和圆柱面共享直线
            foreach (var pe in MainFace.Edges)
            {
                if (e == pe)
                {
                    InnerChildEdge = e;
                    break;
                }
            }
            //+ 如果找到了两条线则找另外两条，并确定八个点对应的位置
            if (InnerFatherEdge == null || InnerChildEdge == null)
                continue;
            #region 找剩余的两条线
            #region 找到与 Father 对应的直线（平面可能由多个面构成）
            //+ 找到扇形面对
            Face? leftFatherFace = null;
            Face? rightFatherFace = null;
            //取出边的两个点
            List<TVertex> InnerFatherEdgePoints = [];
            foreach (var v in new Explorer(InnerFatherEdge.TopoEdge, ShapeEnum.VERTEX))
            {
                InnerFatherEdgePoints.Add(v.AsVertex());
            }
            if (InnerFatherEdgePoints.Count != 2)
            {
                throw new Exception("构成边的点不为2");
            }

            //+ 将直线两个点与面上的点进行比较，找出扇形面对
            TVertex InnerFatherEdgePoint1 = InnerFatherEdgePoints[0];
            TVertex InnerFatherEdgePoint2 = InnerFatherEdgePoints[1];
            foreach (var face in Bending.LeftSectorFaces)
            {
                List<TVertex> facePoints = face.TopoVertices;
                if (facePoints.Any(e => e.IsEqual(InnerFatherEdgePoint1)))
                {
                    leftFatherFace = face;
                    LeftInnerFatherPoint = InnerFatherEdgePoint1.ToPnt();
                    break;
                }
                else if (facePoints.Any(e => e.IsEqual(InnerFatherEdgePoint2)))
                {
                    leftFatherFace = face;
                    LeftInnerFatherPoint = InnerFatherEdgePoint2.ToPnt();
                    break;
                }
            }
            foreach (var face in Bending.RightSectorFaces)
            {
                List<TVertex> facePoints = face.TopoVertices;
                if (facePoints.Any(e => e.IsEqual(InnerFatherEdgePoint1)))
                {
                    rightFatherFace = face;
                    RightInnerFatherPoint = InnerFatherEdgePoint1.ToPnt();
                    break;
                }
                else if (facePoints.Any(e => e.IsEqual(InnerFatherEdgePoint2)))
                {
                    rightFatherFace = face;
                    RightInnerFatherPoint = InnerFatherEdgePoint2.ToPnt();
                    break;
                }
            }
            if (leftFatherFace == null || rightFatherFace == null)
            {
                throw new Exception($"没找到InnerFatherEdge {Bending.OutterFace} 对应的扇形面对");
            }

            //+ 找到扇形面对后，找两端分别位于面对上的直线
            Pnt? leftOutterFatherPoint = null;
            Pnt? rightOutterFatherPoint = null;
            foreach (var outterEdge in Bending.OutterFace.Edges)
            {
                //理论上只会有两根直线
                if (e.Type != CurveType.Line)
                    continue;
                List<TVertex> outterEdgePoints = [];
                foreach (var v in new Explorer(outterEdge.TopoEdge, ShapeEnum.VERTEX))
                {
                    outterEdgePoints.Add(v.AsVertex());
                }
                if (outterEdgePoints.Count != 2)
                {
                    throw new Exception("构成边的点不为2");
                }
                //找到那条直线
                if (
                    (
                        leftFatherFace.TopoVertices.Any(v => v.IsEqual(outterEdgePoints[0]))
                        && rightFatherFace.TopoVertices.Any(v => v.IsEqual(outterEdgePoints[1]))
                    )
                )
                {
                    leftOutterFatherPoint = outterEdgePoints[0].ToPnt();
                    rightOutterFatherPoint = outterEdgePoints[1].ToPnt();
                    //此时还不确定是fater还是child（如果只有一对扇形面，则无法确定）
                    OutterFatherEdge = outterEdge;
                    break;
                }
                else if (
                    leftFatherFace.TopoVertices.Any(v => v.IsEqual(outterEdgePoints[1]))
                    && rightFatherFace.TopoVertices.Any(v => v.IsEqual(outterEdgePoints[0]))
                )
                {
                    leftOutterFatherPoint = outterEdgePoints[1].ToPnt();
                    rightOutterFatherPoint = outterEdgePoints[0].ToPnt();
                    //此时还不确定是fater还是child（如果只有一对扇形面，则无法确定）
                    OutterFatherEdge = outterEdge;
                    break;
                }
            }
            #endregion
            #region 找到与 Child 对应的直线（平面可能由多个面构成）
            //+ 找到扇形面对
            Face? leftChildFace = null;
            Face? rightChildFace = null;
            //取出边的两个点
            List<TVertex> InnerChildEdgePoints = [];
            foreach (var v in new Explorer(InnerChildEdge.TopoEdge, ShapeEnum.VERTEX))
            {
                InnerChildEdgePoints.Add(v.AsVertex());
            }
            if (InnerChildEdgePoints.Count != 2)
            {
                throw new Exception("构成边的点不为2");
            }

            //将直线两个点与面上的点进行比较，找出扇形面对
            TVertex InnerChildEdgePoint1 = InnerChildEdgePoints[0];
            TVertex InnerChildEdgePoint2 = InnerChildEdgePoints[1];
            foreach (var face in Bending.LeftSectorFaces)
            {
                List<TVertex> facePoints = face.TopoVertices;
                if (facePoints.Any(e => e.IsEqual(InnerChildEdgePoint1)))
                {
                    leftChildFace = face;
                    LeftInnerChildPoint = InnerChildEdgePoint1.ToPnt();
                    break;
                }
                else if (facePoints.Any(e => e.IsEqual(InnerChildEdgePoint2)))
                {
                    leftChildFace = face;
                    LeftInnerChildPoint = InnerChildEdgePoint2.ToPnt();
                    break;
                }
            }
            foreach (var face in Bending.RightSectorFaces)
            {
                List<TVertex> facePoints = face.TopoVertices;
                if (facePoints.Any(e => e.IsEqual(InnerChildEdgePoint1)))
                {
                    rightChildFace = face;
                    RightInnerChildPoint = InnerChildEdgePoint1.ToPnt();
                    break;
                }
                else if (facePoints.Any(e => e.IsEqual(InnerChildEdgePoint2)))
                {
                    rightChildFace = face;
                    RightInnerChildPoint = InnerChildEdgePoint2.ToPnt();
                    break;
                }
            }
            if (leftChildFace == null || rightChildFace == null)
            {
                throw new Exception($"没找到InnerChildEdge {Bending.OutterFace} 对应的扇形面对");
            }

            //+ 找到扇形面对后，找两端分别位于面对上的直线
            Pnt? leftOutterChildPoint = null;
            Pnt? rightOutterChildPoint = null;
            foreach (var outterEdge in Bending.OutterFace.Edges)
            {
                //理论上只会有两根直线
                if (e.Type != CurveType.Line)
                    continue;
                List<TVertex> outterEdgePoints = [];
                foreach (var v in new Explorer(outterEdge.TopoEdge, ShapeEnum.VERTEX))
                {
                    outterEdgePoints.Add(v.AsVertex());
                }
                if (outterEdgePoints.Count != 2)
                {
                    throw new Exception("构成边的点不为2");
                }

                //找到那条直线
                if (
                    (
                        leftChildFace.TopoVertices.Any(v => v.IsEqual(outterEdgePoints[0]))
                        && rightChildFace.TopoVertices.Any(v => v.IsEqual(outterEdgePoints[1]))
                    )
                )
                {
                    if (outterEdge.Index == OutterFatherEdge.Index)
                        continue;
                    leftOutterChildPoint = outterEdgePoints[0].ToPnt();
                    rightOutterChildPoint = outterEdgePoints[1].ToPnt();
                    //此时还不确定是fater还是child（如果只有一对扇形面，则无法确定）
                    OutterChildEdge = outterEdge;
                    break;
                }
                else if (
                    leftChildFace.TopoVertices.Any(v => v.IsEqual(outterEdgePoints[1]))
                    && rightChildFace.TopoVertices.Any(v => v.IsEqual(outterEdgePoints[0]))
                )
                {
                    if (outterEdge.Index == OutterFatherEdge.Index)
                        continue;
                    leftOutterChildPoint = outterEdgePoints[1].ToPnt();
                    rightOutterChildPoint = outterEdgePoints[0].ToPnt();
                    //此时还不确定是fater还是child（如果只有一对扇形面，则无法确定）
                    OutterChildEdge = outterEdge;
                    break;
                }
            }
            #endregion
            #region 区分Fater和Child
            //按照预期顺序构建平面，失败则调整顺序
            if (
                leftOutterFatherPoint == null
                || leftOutterChildPoint == null
                || rightOutterFatherPoint == null
                || rightOutterChildPoint == null
            )
            {
                throw new Exception("没找到扇形面对应4个点");
            }
            MakeWire wrieMaker =
                new(
                    [
                        new MakeEdge(LeftInnerFatherPoint, LeftInnerChildPoint),
                        new MakeEdge(LeftInnerChildPoint, leftOutterChildPoint),
                        new MakeEdge(leftOutterChildPoint, leftOutterFatherPoint),
                        new MakeEdge(leftOutterFatherPoint, LeftInnerFatherPoint),
                    ]
                );
            Analyzer ana = new(new MakeFace(wrieMaker));
            if (!ana.IsValid())
            {
                LeftOutterChildPoint = leftOutterChildPoint;
                LeftOutterFatherPoint = leftOutterFatherPoint;
                RightOutterFatherPoint = rightOutterFatherPoint;
                RightOutterChildPoint = rightOutterChildPoint;
            }
            else
            {
                MakeWire wrieMaker2 =
                    new(
                        [
                            new MakeEdge(LeftInnerFatherPoint, LeftInnerChildPoint),
                            new MakeEdge(LeftInnerChildPoint, leftOutterFatherPoint),
                            new MakeEdge(leftOutterFatherPoint, leftOutterChildPoint),
                            new MakeEdge(leftOutterChildPoint, LeftInnerFatherPoint),
                        ]
                    );
                Analyzer ana2 = new(new MakeFace(wrieMaker));
                if (!ana2.IsValid())
                {
                    throw new Exception("没找到扇形面对应4个点");
                }
                LeftOutterChildPoint = leftOutterFatherPoint;
                LeftOutterFatherPoint = leftOutterChildPoint;
                RightOutterFatherPoint = rightOutterChildPoint;
                RightOutterChildPoint = rightOutterFatherPoint;
            }
            #endregion
            #endregion
            break;
        }
        //+ 如果没有，则主平面对应外圆柱
        if (InnerFatherEdge == null || InnerChildEdge == null)
        {
            foreach (Edge e in Bending.OutterFace.Edges)
            {
                //理论上只会有两根直线
                if (e.Type != CurveType.Line)
                    continue;
                //父节点主平面和圆柱面共享直线
                foreach (var pe in Parent.MainFace.Edges)
                {
                    if (e == pe)
                    {
                        OutterFatherEdge = e;
                        break;
                    }
                }
                //当前节点主平面和圆柱面共享直线
                foreach (var pe in MainFace.Edges)
                {
                    if (e == pe)
                    {
                        OutterChildEdge = e;
                        break;
                    }
                }

                //如果找到了两条线
                if (OutterFatherEdge == null || OutterChildEdge == null)
                    continue;
                #region 找剩余的两条线
                #region 找到与 Father 对应的直线（平面可能由多个面构成）
                //+ 找到扇形面对
                Face? leftFatherFace = null;
                Face? rightFatherFace = null;
                //取出边的两个点
                List<TVertex> outterFatherEdgePoints = [];
                foreach (var v in new Explorer(OutterFatherEdge.TopoEdge, ShapeEnum.VERTEX))
                {
                    outterFatherEdgePoints.Add(v.AsVertex());
                }
                if (outterFatherEdgePoints.Count != 2)
                {
                    throw new Exception("构成边的点不为2");
                }

                //将直线两个点与面上的点进行比较，找出扇形面对
                foreach (var face in Bending.LeftSectorFaces)
                {
                    if (face.TopoVertices.Any(e => e.IsEqual(outterFatherEdgePoints[0])))
                    {
                        leftFatherFace = face;
                        LeftOutterFatherPoint = outterFatherEdgePoints[0].ToPnt();
                        break;
                    }
                    else if (face.TopoVertices.Any(e => e.IsEqual(outterFatherEdgePoints[1])))
                    {
                        leftFatherFace = face;
                        LeftOutterFatherPoint = outterFatherEdgePoints[1].ToPnt();
                        break;
                    }
                }
                foreach (var face in Bending.RightSectorFaces)
                {
                    if (face.TopoVertices.Any(e => e.IsEqual(outterFatherEdgePoints[0])))
                    {
                        rightFatherFace = face;
                        RightOutterFatherPoint = outterFatherEdgePoints[0].ToPnt();
                        break;
                    }
                    else if (face.TopoVertices.Any(e => e.IsEqual(outterFatherEdgePoints[1])))
                    {
                        rightFatherFace = face;
                        RightOutterFatherPoint = outterFatherEdgePoints[1].ToPnt();
                        break;
                    }
                }
                if (leftFatherFace == null || rightFatherFace == null)
                {
                    throw new Exception($"没找到OutterFatherEdge {Bending.OutterFace} 对应的扇形面对");
                }

                //+ 找到扇形面对后，找两端分别位于面对上的直线
                Pnt? leftInnerFatherPoint = null;
                Pnt? rightInnerFatherPoint = null;
                foreach (var innerEdge in Bending.InnerFace.Edges)
                {
                    //理论上只会有两根直线
                    if (e.Type != CurveType.Line)
                        continue;
                    List<TVertex> innerEdgePoints = [];
                    foreach (var v in new Explorer(innerEdge.TopoEdge, ShapeEnum.VERTEX))
                    {
                        innerEdgePoints.Add(v.AsVertex());
                    }
                    if (innerEdgePoints.Count != 2)
                    {
                        throw new Exception("构成边的点不为2");
                    }
                    //找到那条直线
                    if (
                        (
                            leftFatherFace.TopoVertices.Any(v => v.IsEqual(innerEdgePoints[0]))
                            && rightFatherFace.TopoVertices.Any(v => v.IsEqual(innerEdgePoints[1]))
                        )
                    )
                    {
                        leftInnerFatherPoint = innerEdgePoints[0].ToPnt();
                        rightInnerFatherPoint = innerEdgePoints[1].ToPnt();
                        //此时还不确定是fater还是child（如果只有一对扇形面，则无法确定）
                        InnerFatherEdge = innerEdge;
                        break;
                    }
                    else if (
                        leftFatherFace.TopoVertices.Any(v => v.IsEqual(innerEdgePoints[1]))
                        && rightFatherFace.TopoVertices.Any(v => v.IsEqual(innerEdgePoints[0]))
                    )
                    {
                        leftInnerFatherPoint = innerEdgePoints[1].ToPnt();
                        rightInnerFatherPoint = innerEdgePoints[0].ToPnt();
                        //此时还不确定是fater还是child（如果只有一对扇形面，则无法确定）
                        InnerFatherEdge = innerEdge;
                        break;
                    }
                }
                #endregion
                #region 找到与 Child 对应的直线（平面可能由多个面构成）
                //+ 找到扇形面对
                Face? leftChildFace = null;
                Face? rightChildFace = null;
                //取出边的两个点
                List<TVertex> outterChildEdgePoints = [];
                foreach (var v in new Explorer(OutterChildEdge.TopoEdge, ShapeEnum.VERTEX))
                {
                    outterChildEdgePoints.Add(v.AsVertex());
                }
                if (outterChildEdgePoints.Count != 2)
                {
                    throw new Exception("构成边的点不为2");
                }

                //将直线两个点与面上的点进行比较，找出扇形面对
                foreach (var face in Bending.LeftSectorFaces)
                {
                    if (face.TopoVertices.Any(e => e.IsEqual(outterChildEdgePoints[0])))
                    {
                        leftChildFace = face;
                        LeftOutterChildPoint = outterChildEdgePoints[0].ToPnt();
                        break;
                    }
                    else if (face.TopoVertices.Any(e => e.IsEqual(outterChildEdgePoints[1])))
                    {
                        leftChildFace = face;
                        LeftOutterChildPoint = outterChildEdgePoints[1].ToPnt();
                        break;
                    }
                }
                foreach (var face in Bending.RightSectorFaces)
                {
                    if (face.TopoVertices.Any(e => e.IsEqual(outterChildEdgePoints[0])))
                    {
                        rightChildFace = face;
                        RightOutterChildPoint = outterChildEdgePoints[0].ToPnt();
                        break;
                    }
                    else if (face.TopoVertices.Any(e => e.IsEqual(outterChildEdgePoints[1])))
                    {
                        rightChildFace = face;
                        RightOutterChildPoint = outterChildEdgePoints[1].ToPnt();
                        break;
                    }
                }
                if (leftChildFace == null || rightChildFace == null)
                {
                    throw new Exception($"没找到InnerChildEdge {Bending.OutterFace} 对应的扇形面对");
                }

                //+ 找到扇形面对后，找两端分别位于面对上的直线
                Pnt? leftInnerChildPoint = null;
                Pnt? rightInnerChildPoint = null;
                foreach (var innerEdge in Bending.InnerFace.Edges)
                {
                    //理论上只会有两根直线
                    if (e.Type != CurveType.Line)
                        continue;
                    List<TVertex> innerEdgePoints = [];
                    foreach (var v in new Explorer(innerEdge.TopoEdge, ShapeEnum.VERTEX))
                    {
                        innerEdgePoints.Add(v.AsVertex());
                    }
                    if (innerEdgePoints.Count != 2)
                    {
                        throw new Exception("构成边的点不为2");
                    }

                    //找到那条直线
                    if (
                        (
                            leftChildFace.TopoVertices.Any(v => v.IsEqual(innerEdgePoints[0]))
                            && rightChildFace.TopoVertices.Any(v => v.IsEqual(innerEdgePoints[1]))
                        )
                    )
                    {
                        if (innerEdge.Index == InnerFatherEdge.Index)
                            continue;
                        leftInnerChildPoint = innerEdgePoints[0].ToPnt();
                        rightInnerChildPoint = innerEdgePoints[1].ToPnt();
                        //此时还不确定是fater还是child（如果只有一对扇形面，则无法确定）
                        InnerChildEdge = innerEdge;
                        break;
                    }
                    else if (
                        leftChildFace.TopoVertices.Any(v => v.IsEqual(innerEdgePoints[1]))
                        && rightChildFace.TopoVertices.Any(v => v.IsEqual(innerEdgePoints[0]))
                    )
                    {
                        if (innerEdge.Index == InnerFatherEdge.Index)
                            continue;
                        leftInnerChildPoint = innerEdgePoints[1].ToPnt();
                        rightInnerChildPoint = innerEdgePoints[0].ToPnt();
                        //此时还不确定是fater还是child（如果只有一对扇形面，则无法确定）
                        InnerChildEdge = innerEdge;
                        break;
                    }
                }
                #endregion
                #region 区分Fater和Child
                //按照预期顺序构建平面，失败则调整顺序
                if (
                    leftInnerFatherPoint == null
                    || leftInnerChildPoint == null
                    || rightInnerFatherPoint == null
                    || rightInnerChildPoint == null
                )
                {
                    throw new Exception("没找到扇形面对应4个点");
                }
                //均按照左侧面去找点
                MakeWire wrieMaker =
                    new(
                        [
                            new MakeEdge(leftInnerFatherPoint, leftInnerChildPoint),
                            new MakeEdge(leftInnerChildPoint, LeftOutterChildPoint),
                            new MakeEdge(LeftOutterChildPoint, LeftOutterFatherPoint),
                            new MakeEdge(LeftOutterFatherPoint, leftInnerFatherPoint),
                        ]
                    );
                Analyzer ana = new(new MakeFace(wrieMaker));
                if (!ana.IsValid())
                {
                    LeftInnerFatherPoint = leftInnerFatherPoint;
                    LeftInnerChildPoint = leftInnerChildPoint;
                    RightInnerFatherPoint = rightInnerFatherPoint;
                    RightInnerChildPoint = rightInnerChildPoint;
                }
                else
                {
                    MakeWire wrieMaker2 =
                        new(
                            [
                                new MakeEdge(leftInnerChildPoint, leftInnerFatherPoint),
                                new MakeEdge(leftInnerFatherPoint, LeftOutterChildPoint),
                                new MakeEdge(LeftOutterChildPoint, LeftOutterFatherPoint),
                                new MakeEdge(LeftOutterFatherPoint, leftInnerChildPoint),
                            ]
                        );
                    Analyzer ana2 = new(new MakeFace(wrieMaker));
                    if (!ana2.IsValid())
                    {
                        throw new Exception("没找到扇形面对应4个点");
                    }
                    LeftInnerFatherPoint = leftInnerChildPoint;
                    LeftInnerChildPoint = leftInnerFatherPoint;
                    RightInnerFatherPoint = rightInnerChildPoint;
                    RightInnerChildPoint = rightInnerFatherPoint;
                }
                #endregion
                #endregion
                break;
            }
            if (OutterFatherEdge == null || OutterChildEdge == null)
                throw new ArgumentException("没找到主平面相邻圆柱的两条直线");
        }

        #endregion
    }

    /// <summary>
    /// 当前折弯相对于父节点的变换
    /// </summary>
    public override Trsf Location
    {
        get
        {
            if (Unfolded)
            {
                return Parent.Location.Multiplied(UnfoldTransform);
            }
            else
            {
                return Parent.Location;
            }
            throw new Exception("未知的父节点");
        }
    }

    /// <summary>
    /// 当前节点是否展开
    /// </summary>
    public bool Unfolded { get; set; } = false;

    /// <summary>
    /// 用于折弯展开的变换
    /// </summary>
    protected Trsf UnfoldTransform { get; set; } = new();

    /// <summary>
    /// 内圆柱面上靠近父节点的线段
    /// </summary>
    protected Edge InnerFatherEdge;

    /// <summary>
    /// 外圆柱面上靠近父节点的线段
    /// </summary>
    protected Edge OutterFatherEdge;

    /// <summary>
    /// 一侧的折弯面对应的点（按照内侧父、内侧子、外侧子、外侧父存储）
    /// </summary>
    protected List<Pnt> LeftPoints = [null, null, null, null];
    protected Pnt LeftInnerFatherPoint
    {
        get => LeftPoints[0];
        set { LeftPoints[0] = value; }
    }
    protected Pnt LeftInnerChildPoint
    {
        get => LeftPoints[1];
        set { LeftPoints[1] = value; }
    }
    protected Pnt LeftOutterFatherPoint
    {
        get => LeftPoints[2];
        set { LeftPoints[2] = value; }
    }
    protected Pnt LeftOutterChildPoint
    {
        get => LeftPoints[3];
        set { LeftPoints[3] = value; }
    }

    /// <summary>
    /// 内圆柱面上远离父节点的线段
    /// </summary>
    protected Edge InnerChildEdge;

    /// <summary>
    /// 外圆柱面上远离父节点的线段
    /// </summary>
    protected Edge OutterChildEdge;

    /// <summary>
    /// 另一侧的折弯面对应的点（按照内侧父、内侧子、外侧子、外侧父存储）
    /// </summary>
    protected List<Pnt> RightPoints = [null, null, null, null];
    protected Pnt RightInnerFatherPoint
    {
        get => RightPoints[0];
        set { RightPoints[0] = value; }
    }
    protected Pnt RightInnerChildPoint
    {
        get => RightPoints[1];
        set { RightPoints[1] = value; }
    }
    protected Pnt RightOutterFatherPoint
    {
        get => RightPoints[2];
        set { RightPoints[2] = value; }
    }
    protected Pnt RightOutterChildPoint
    {
        get => RightPoints[3];
        set { RightPoints[3] = value; }
    }

    /// <summary>
    /// 折弯角度为内圆弧角，与折弯展开的角度之和为 π
    /// </summary>
    public double BendingAngle { get; protected set; }
    public double FlatLength { get; protected set; }

    /// <summary>
    /// 折弯线长度（取圆柱面两直线中最长的一条，用于计算折弯力）
    /// </summary>
    public double? BendingLength => Bending.Length;
    public Dir UnfoldingDirection { get; protected set; }

    /// <summary>
    /// 折弯轴为右手坐标轴，旋转方向与折弯方向保持一致
    /// </summary>
    public Ax1 BendingAxis { get; protected set; }

    /// <summary>
    /// 角度和对应的扇形旋转体
    /// </summary>
    public Dictionary<double, TShape> Sectors { get; protected set; } = [];

    #region Debug
    //todo 用于debug的AIS对象需要采用延迟创建
    public AAxis AISBendingAxis;
    public List<AAxis> DebugSectorAxis { get; private set; } = [];
    public AAxis AISSectorDir;
    #endregion

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
        //先找到扇形面的所有点（可能不止8个）
        List<Pnt> sectorPnts = [];
        foreach (var f in Bending.CylinderFaces)
        {
            foreach (var e in f.Edges)
            {
                foreach (var p in e.Points)
                {
                    if (!sectorPnts.Any(pp => pp.Distance(p) < 1e-1))
                    {
                        sectorPnts.Add(p);
                    }
                }
            }
        }
        //找到平面1的所有点，筛选出和扇形面点重合的点
        List<Pnt> t1 = [];
        foreach (var f in Bending.LeftSectorFaces)
        {
            foreach (var e in f.Edges)
            {
                foreach (var p in e.Points)
                {
                    if (!t1.Any(pp => pp.Distance(p) < 1e-1))
                    {
                        t1.Add(p);
                    }
                }
            }
        }
        //筛选相同点(理论上有4个)
        foreach (var p in t1)
        {
            foreach (var pp in sectorPnts)
            {
                //todo 此处的精度不确定
                if (p.Distance(pp) < 5e-2)
                {
                    face1Pnts.Add(p);
                }
            }
        }
        //找到平面2的所有点(理论上有4个)，筛选出和扇形面点重合的点
        List<Pnt> t2 = [];
        foreach (var e in Bending.SectorFaces[1].Edges)
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
            foreach (var pp in sectorPnts)
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
        AISSectorDir = new(new(p1, new Dir(vec)), l);
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

    public int Order { get; set; } = 0;

    #endregion

    public NodeDS Parent { get; set; }
    public BendingDS Bending { get; private set; }

    #region VBend
    /// <summary>
    /// 合并的其它折弯
    /// </summary>
    public List<LeafNode> CombinedNodes { get; } = [];
    #endregion

    #region ClosedHem
    /// <summary>
    /// 同时折弯的节点
    /// </summary>
    public LeafNode HemNode { get; set; }
    #endregion

    #region 显示
    protected AShape AISBendingFaces;
    protected AAxis AISBendingNormal;
    protected AShape AISUnfoldingLocation;
    protected AAxis AISUnfoldingDirection;
    protected TextLabel AISLabel = new();
    public Color BendingColor { get; set; } = ColorMap.Red;

    /// <summary>
    /// 在此之前需要调用EraseAll方法
    /// </summary>
    /// <param name="context"></param>
    /// <param name="update"></param>
    public void Show(
        InteractiveContext context,
        bool update,
        bool showBendingNormal,
        bool showMainFaceNormal
    )
    {
        #region 简化代码
        Action<InteractiveObject> Display = obj =>
        {
            context.Display(obj, false);
        };
        Action<InteractiveObject> DisplayAndSetColor = obj =>
        {
            if (context.IsDisplayed(obj))
            {
                context.Redisplay(obj, false);
            }
            else
            {
                context.Display(obj, false);
                context.SetColor(obj, BendingColor, false);
            }
        };
        #endregion

        foreach (var node in CombinedNodes)
        {
            node.BendingColor = BendingColor;
            node.Show(context, false, showBendingNormal, showMainFaceNormal);
        }

        AISFaceSet.SetLocalTransformation(Location);
        DisplayAndSetColor(AISFaceSet);
        context.SetTransparency(AISFaceSet, 0.0, false);

        if (!Unfolded)
        {
            DisplayAndSetColor(AISBendingFaces);
            context.SetTransparency(AISBendingFaces, 0.0, false);
        }

        #region Debug
        foreach (var pnt in LeftPoints)
        {
            AShape DebugAISVertex = new(new MakeSphere(pnt, 1));
            DisplayAndSetColor(DebugAISVertex);
        }
        foreach (var pnt in RightPoints)
        {
            AShape DebugAISVertex = new(new MakeSphere(pnt, 1));
            DisplayAndSetColor(DebugAISVertex);
        }

        Trsf normalT = new();
        if (Unfolded)
        {
            normalT.SetRotationPart(new Quat(BendingAxis, BendingAngle / 2.0));
            normalT.SetTranslationPart(UnfoldingDirection.ToVec(FlatLength / 2.0));
        }
        AISBendingNormal.SetLocalTransformation(Location.Multiplied(normalT));
        DisplayAndSetColor(AISBendingNormal);

        DisplayAndSetColor(AISUnfoldingDirection);
        DisplayAndSetColor(AISUnfoldingLocation);

        AISMainFaceNormal.SetLocalTransformation(Location);
        DisplayAndSetColor(AISMainFaceNormal);

        DisplayAndSetColor(AISBendingAxis);

        //Bending.InnerFace.DebugShow(context, bendingColor);
        AISLabel.SetText(Order.ToString());
        AISLabel.SetFont("SimSun");
        AISLabel.SetPosition(Bending.InnerFace.CircleCenter?.Transformed(Location));
        AISLabel.SetHeight(Bending.Thickness * 20);
        AISLabel.SetZLayer(ZLayerId.TopOSD);
        AISLabel.SetZoomable(false);
        DisplayAndSetColor(AISLabel);
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
            //Trsf up = new(new Pnt(), new Pnt(0, 0, 5));
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
                AISShapes.Add(DebugSectorAxis[i]);
            }
            context.Display(AISSectorDir, false);
            context.SetColor(AISSectorDir, ColorMap.Black, false);
            AISShapes.Add(AISSectorDir);
        }
    }

    #endregion
}

public class VBendNode : LeafNode
{
    public VBendNode(
        Face mainFace,
        HashSet<Face> faceSet,
        NodeDS parent,
        BendingDS bending,
        Kparam kparam
    )
        : base(mainFace, faceSet, parent, bending)
    {
        #region 计算展开长度
        // 计算中性层半径
        KRadius = kparam.GetKRadius(
            bending.InnerFace.CircleRadius ?? throw new Exception("内圆柱面半径为空"),
            bending.Thickness
        );
        BendingAngle = Math.PI - bending.Angle;
        FlatLength = KRadius * bending.Angle;
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
        //    CreateSectors(angle, axis);
        #region Debug
        //    DebugSectorAxis.Add(aaxis);
        //}
        DebugSectorAxis.Add(new(BendingAxis));
        TEdge edge = InnerFatherEdge?.TopoEdge ?? OutterFatherEdge?.TopoEdge;
        Pnt local = BrepGeomtryTools.GetEdgeMidlePoint(edge);
        AISUnfoldingLocation = new(new MakeSphere(local, 1));
        AISShapes.Add(AISUnfoldingLocation);
        AISUnfoldingDirection = new(new(local, UnfoldingDirection), (double)FlatLength);
        AISShapes.Add(AISUnfoldingLocation);
        #endregion
    }

    private readonly double KRadius;
}

public class ClosedHemNode : LeafNode
{
    public ClosedHemNode(
        Face mainFace,
        HashSet<Face> faceSet,
        NodeDS parent,
        BendingDS bending,
        Kparam kparam
    )
        : base(mainFace, faceSet, parent, bending)
    {
        //+ 计算中性层半径
        //KRadius = kparam.GetKRadius(
        //    bending.InnerFace.CircleRadius ?? throw new Exception("内圆柱面半径为空"),
        //    bending.Thickness
        //);
        BendingAngle = Math.PI;
        #region 计算展开长度
        //死边的K值暂时按1.2算
        FlatLength = (1.2 * bending.Thickness);
        //Debug.WriteLine(
        //    $"死边计算：{Math.Round((double)bending.InnerFace.CircleRadius, 2)}|{Math.Round((double)KRadius, 2)}|{((double)BendingAngle).ToDegrees(1)}|{FlatLength}"
        //);
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
        //    CreateSectors(angle, axis);
        #region Debug
        //    DebugSectorAxis.Add(aaxis);
        //}
        DebugSectorAxis.Add(new(BendingAxis));
        TEdge edge = InnerFatherEdge?.TopoEdge ?? OutterFatherEdge?.TopoEdge;
        Pnt local = BrepGeomtryTools.GetEdgeMidlePoint(edge);
        AISUnfoldingLocation = new(new MakeSphere(local, 1));
        AISShapes.Add(AISUnfoldingLocation);
        AISUnfoldingDirection = new(new(local, UnfoldingDirection), (double)FlatLength);
        AISShapes.Add(AISUnfoldingLocation);
        #endregion
    }

    //private readonly double KRadius;
}
