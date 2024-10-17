using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Drawing;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Forms.VisualStyles;
using System.Windows.Input;
using OCCTK.IO;
using OCCTK.OCC.AIS;
using OCCTK.OCC.BRepAlgoAPI;
using OCCTK.OCC.BRepBuilderAPI;
using OCCTK.OCC.gp;
using OCCTK.OCC.TopExp;
using OCCTK.OCC.Topo;
using OCCTK.OCC.TopoAbs;
using TestWPF.Laser.SimpleClamp;
using Windows.UI.WebUI;
using WinRT;
using Brushes = System.Windows.Media.Brushes;
using Color = OCCTK.Extension.Color;
using SelectionMode = OCCTK.OCC.AIS.SelectionMode;
using ViewOrientation = OCCTK.OCC.V3d.ViewOrientation;

namespace TestWPF;

public partial class SimpleClamp : Window
{
    private Grid? MakeStackItem(VerticalPiece thePiece)
    {
        if (thePiece == null)
        {
            return null; // 直接返回，不执行后续代码
        }

        // 创建一个新的 Grid
        Grid outerStackPanel = new Grid();

        // 定义显示内容和列索引
        var labelDefinitions = new (string Content, int ColumnIndex)[]
        {
            ($"{thePiece.Order}", 0),
        };

        // 创建并添加列定义
        for (int i = 0; i < labelDefinitions.Length + 1; i++)
        {
            ColumnDefinition column = new ColumnDefinition
            {
                Width =
                    i == 0
                        ? new GridLength(1, GridUnitType.Star)
                        : new GridLength(1, GridUnitType.Auto)
            };
            outerStackPanel.ColumnDefinitions.Add(column);
        }

        // 创建并添加子元素
        foreach (var (content, columnIndex) in labelDefinitions)
        {
            Label label = new Label
            {
                Content = content,
                HorizontalContentAlignment = HorizontalAlignment.Left
            };

            // 设置标签的列索引
            Grid.SetColumn(label, columnIndex);

            // 将标签添加到 Grid 中
            outerStackPanel.Children.Add(label);
        }
        //! 增加删除按钮
        Button deleteButton = new Button { Content = "删除" };
        Grid.SetColumn(deleteButton, labelDefinitions.Length);
        outerStackPanel.Children.Add(deleteButton);
        deleteButton.Click += (sender, e) =>
        {
            //删除对应的 Piece
            thePiece.Remove(true);
            //删除对应的grid
            if (outerStackPanel.Parent is StackPanel theS)
            {
                theS.Children.Remove(outerStackPanel);
            }
            Canvas.Update();
        };
        //! 绑定 thePiece 对象到第一个 Label 的 Tag 属性
        if (outerStackPanel.Children[0] is Label firstLabel)
        {
            firstLabel.Tag = thePiece;
            // 添加鼠标左键点击事件处理程序
            firstLabel.MouseLeftButtonDown += VerticalPieceLabel_MouseLeftButtonDown;
        }

        return outerStackPanel;
    }

    private void VerticalPieceLabel_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
    {
        // 获取点击的 Label
        Label? clickedLabel = sender as Label;

        // 确保 Label 不为空且 Tag 不为空
        if (clickedLabel == null || clickedLabel.Tag == null)
            return;

        // 获取 Grid（Label 是直接添加到 Grid 中的）
        Grid? parentGrid = clickedLabel.Parent as Grid;
        if (parentGrid == null || parentGrid.Parent is not StackPanel parentStack)
            return;

        // 获取被点击 Label 所在的行
        int clickedRow = parentStack.Children.IndexOf(parentGrid);

        // 遍历 StackPanel 的所有子元素
        foreach (UIElement theGrid in parentStack.Children)
        {
            if (theGrid is not Grid grid)
                continue;

            int row = parentStack.Children.IndexOf(grid);
            // 遍历每个 Grid 中的子元素
            foreach (UIElement element in grid.Children)
            {
                if (element is not Label label)
                    continue;

                int column = Grid.GetColumn(label);
                // 如果是点击的行，按照颜色数组设置背景颜色
                if (row == clickedRow)
                {
                    if (column >= colorSequence.Length)
                        continue;
                    label.Background = colorSequence[column];
                }
                else
                {
                    // 其他行设置为透明
                    label.Background = Brushes.Transparent;
                }
            }
        }

        // 选中 AIS
        var VP = clickedLabel.Tag as VerticalPiece;
        if (VP != null)
        {
            AISContext.SelectAIS(VP.AISPiece, true);
        }
    }

    private void OCCFunctionTest()
    {
        #region 遍历shape
        //Explorer faceExp = new(InputWorkpiece.Shape, ShapeEnum.EDGE);
        //int faceNum = 0;
        //while (faceExp.More())
        //{
        //    faceNum += 1;
        //    var currentEdge = faceExp.Current().AsEdge();
        //    Explorer PntExp = new(currentEdge, ShapeEnum.VERTEX);
        //    int edgeNum = 0;
        //    for (var endPoint = PntExp.Current().AsVertex(); PntExp.More(); PntExp.Next())
        //    {
        //        edgeNum += 1;
        //    }
        //    Debug.WriteLine($"Face: {faceNum},EndPoint:{edgeNum}");
        //    faceExp.Next();
        //}
        #endregion

        #region 构造Wire
        //MakeWire wireMaker = new();
        //wireMaker.Add(new MakeEdge(new Pnt(0, 0, 0), new Pnt(1, 0, 0)));
        //wireMaker.Add(new MakeEdge(new Pnt(1, 0, 0), new Pnt(2, 0, 0)));
        //Debug.WriteLine($"Wire is done: {wireMaker.Error()}");
        #endregion

        #region 输入工件
        Workpiece testWorkpiece;
        testWorkpiece = new Workpiece(new BrepExchange("mods\\test01.brep"));
        log.Info("工件加载完成");
        testWorkpiece.Show(AISContext, false);
        //testWorkpiece = new Workpiece(new BrepExchange("mods\\new_test01.brep"));
        //testWorkpiece.Dispose();
        //Display(testWorkpiece.AIS, true);
        //Debug.WriteLine($"{testWorkpiece.BndBox.XMax}");
        #endregion

        #region 构造底板
        BasePlate testBasePlate = new(testWorkpiece);
        log.Info("底板构造完成");
        testBasePlate.Show(AISContext, false);
        #endregion

        #region 计算横截位置
        Pnt testCenterLocation =
            new(
                testWorkpiece.BndBox.XMin + testWorkpiece.DX / 2,
                testWorkpiece.BndBox.YMin + testWorkpiece.DY / 2,
                testBasePlate.Z
            );
        Dir testVerticalDir = new(15, 10, 0);
        PlatePose testPose = new(testCenterLocation, testVerticalDir);
        #endregion

        //#region 构造片（Pieces）

        //List<VerticalPiece> testVerticalPieces = [];
        //int num = 0;
        //foreach (TEdge? edge in splitter.SectionEdges()) {
        //VerticalPiece testPiece = new(new MyEdge(edge, testPose), testBasePlate);
        //testPiece.Show(AISContext, false);
        //testPiece.Order = num;
        //CurrentPlate_StackPanel.Children.Add(MakeStackItem(testPiece));
        //num += 1;
        //}
        //#endregion
        #region 构造竖板
        var testVerticalPlate = CreateVerticalPlate(testBasePlate, testPose, 1, 0, 0);
        #endregion
        testVerticalPlate.ShowPlate(AISContext, true);

        Update();
        FitAll();
    }

    /// <summary>
    /// 创建竖板
    /// </summary>
    /// <param name="basePlate"></param>
    /// <param name="pose"></param>
    /// <param name="clearances">两端避让长度</param>
    /// <param name="minSupportLen">最小支撑长度</param>
    /// <param name="cutDistance">切断距离</param>
    private VerticalPlate CreateVerticalPlate(
        BasePlate basePlate,
        PlatePose pose,
        double clearances,
        double minSupportLen,
        double cutDistance
    )
    {
        List<Color> testColorMap =
        [
            new(255, 0, 0),
            new(0, 255, 0),
            new(0, 0, 255),
            new(125, 0, 0),
            new(125, 125, 0),
            new(0, 125, 125)
        ];
        VerticalPlate testVerticalPlate = new(basePlate, pose);
        testVerticalPlate.Clearances = clearances;
        testVerticalPlate.MinSupportLen = minSupportLen;
        testVerticalPlate.CutDistance = cutDistance;
        double debugZValue = 20.0;
        //! 获取环
        try
        {
            testVerticalPlate.GetRings();
            log.Info("截取环");
            for (int i = 0; i < testVerticalPlate.DebugRings.Count; i++)
            {
                Color color = testColorMap[i % testColorMap.Count];
                List<MyEdge>? ring = testVerticalPlate.DebugRings[i];
                foreach (var edge in ring)
                {
                    //var newEdge = new Transform(edge.Edge, edge.pose.Trsf);
                    //AShape AISEdge = new(newEdge);
                    AShape AISEdge = new(edge);
                    AISEdge.SetLocalTransformation(new(new Pnt(), new(0, 0, debugZValue)));
                    Display(AISEdge, false);
                    SetColor(AISEdge, color, false);
                }
            }
        }
        catch (Exception e)
        {
            log.Debug(e);
        }
        //! 从环中获取下端线
        try
        {
            testVerticalPlate.GetBottomEdges();
            log.Info("分离下端线");
            foreach (var edge in testVerticalPlate.DebugButtomEdges)
            {
                AShape AISEdge = new(edge);
                AISEdge.SetLocalTransformation(new(new Pnt(), new(0, 0, debugZValue * 0.8)));
                Display(AISEdge, false);
                SetColor(AISEdge, new(0, 0, 255), false);
                //AISContext.SetWidth(AISEdge, 5, false);
            }
        }
        catch (Exception e)
        {
            log.Debug(e);
        }
        //! 修剪
        try
        {
            testVerticalPlate.TrimEdgeEnds();
            log.Info("修剪线两端");
            foreach (MyEdge edge in testVerticalPlate.DebugTrimedEdges)
            {
                AShape AISEdge = new(edge);
                AISEdge.SetLocalTransformation(new(new Pnt(), new(0, 0, debugZValue * 0.7)));
                Display(AISEdge, false);
                SetColor(AISEdge, new(0, 125, 200), false);
            }
        }
        catch (Exception e)
        {
            Debug.WriteLine(e);
        }
        //! 移除
        try
        {
            testVerticalPlate.RemoveShortEdge();
            log.Info("移除过短边");
            foreach (MyEdge edge in testVerticalPlate.DebugFilteredEdges)
            {
                AShape AISEdge = new(edge);
                AISEdge.SetLocalTransformation(new(new Pnt(), new(0, 0, debugZValue * 0.6)));
                Display(AISEdge, false);
                SetColor(AISEdge, new(125, 0, 150), false);
            }
        }
        catch (Exception e)
        {
            Debug.WriteLine(e);
        }
        //! 分割
        try
        {
            testVerticalPlate.CutLongEdges();
            log.Info("切割过长边");
            foreach (MyEdge edge in testVerticalPlate.DebugCuttedEdges)
            {
                AShape AISEdge = new(edge);
                AISEdge.SetLocalTransformation(new(new Pnt(), new(0, 0, debugZValue * 0.5)));
                Display(AISEdge, false);
                SetColor(AISEdge, new(0, 0, 255), false);
            }
        }
        catch (Exception e)
        {
            Debug.WriteLine(e);
        }
        //保存结果
        foreach (var edge in testVerticalPlate.DebugCuttedEdges)
        {
            testVerticalPlate.Pieces.Add(new VerticalPiece(edge, basePlate));
        }
        return testVerticalPlate;
    }
}
