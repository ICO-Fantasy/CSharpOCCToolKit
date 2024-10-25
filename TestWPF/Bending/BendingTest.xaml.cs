using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Forms.Integration;
using System.Windows.Forms.VisualStyles;
using System.Windows.Shapes;
using MathNet.Spatial.Units;
using Microsoft.Win32;
using OCCTK.Extension;
using OCCTK.IO;
using OCCTK.OCC.AIS;
using OCCTK.OCC.BRepAdaptor;
using OCCTK.OCC.BRepBuilderAPI;
using OCCTK.OCC.gp;
using OCCTK.OCC.Topo;
using OCCTK.OCC.TopoAbs;
using OCCViewForm;
using TestWPF.Utils;
//设置别名
using Brushes = System.Windows.Media.Brushes;
using Color = OCCTK.Extension.Color;
using SelectionMode = OCCTK.OCC.AIS.SelectionMode;
using ViewOrientation = OCCTK.OCC.V3d.ViewOrientation;

namespace TestWPF.Bending;

/// <summary>
/// BendingTest.xaml 的交互逻辑
/// </summary>
public partial class BendingTest : Window, IAISSelectionHandler
{
    public BendingTest()
    {
        InitializeComponent();
        // 创建 Windows Forms 控件和 WindowsFormsHost
        WindowsFormsHost aHost = new WindowsFormsHost();
        Canvas = new OCCCanvas();
        aHost.Child = Canvas;
        Canvas_Grid.Children.Add(aHost);
        Canvas.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
        Canvas.Show();
        Canvas.OnAISSelectionEvent += OnAISSelection;
    }

    #region 窗口属性和字段

    private readonly OCCCanvas Canvas;
    private InteractiveContext Context => Canvas.AISContext;
    private bool _isBendingArrowVisible = false;
    private bool _isMainFaceNormalVisible = false;

    //简化函数调用
    private Action<InteractiveObject, bool> Display => Canvas.Display;
    private Action<InteractiveObject, Color, bool> SetColor => Context.SetColor;
    private Action<InteractiveObject, double, bool> SetTransparency => Context.SetTransparency;
    private Action<InteractiveObject, bool> Erase => Canvas.Erase;
    private Action<bool> EraseAll => Canvas.EraseAll;
    private Action Update => Canvas.Update;
    private Action FitAll => Canvas.FitAll;

    #endregion

    #region 算法逻辑

    public TShape InputWorkpiece { get; private set; }
    private AShape AISInputWorkpiece;

    public BendingTree BendingTree { get; private set; }

    public void CreateBendingTree()
    {
        BendingTree = new BendingTree(InputWorkpiece, debugContext: Context);
        try { }
        catch (Exception e)
        {
            Debug.WriteLine($"{e}");
        }
    }

    #endregion

    #region 界面更新
    private void UpdateTestBox_StackPanel()
    {
        if (BendingTree == null)
        {
            return;
        }
        TestBox_StackPanel.Children.Clear();
        BendingTree_TreeView.Items.Clear();
        #region 构建折弯树
        // 从 RootNode 开始构建树
        TreeViewItem rootItem = CreateTreeViewItem(BendingTree.RootNode);
        BendingTree_TreeView.Items.Add(rootItem);
        #endregion
        #region 展示Bending
        //if (BendingTree.AllBendings.Count == 0)
        //    return;
        //Random random = new();
        //foreach (var bending in BendingTree.AllBendings)
        //{
        //    Grid g = new();
        //    Label l = new();
        //    String str = $"";
        //    int i = random.Next(ColorMap.Count);
        //    foreach (var f in bending.BendingFaces)
        //    {
        //        str += $"{f.Index}|";
        //        AShape ais = new(f.TopoFace);
        //        Display(ais, false);
        //        SetColor(ais, ColorMap[i], false);
        //        SetTransparency(ais, 0.0, false);
        //    }
        //    l.Content = str;
        //    g.Children.Add(l);
        //    TestBox_StackPanel.Children.Add(g);
        //}
        #endregion
        #region 展示FaceSet
        //foreach (var f in BendingTree.RootNode.FaceSet)
        //{
        //    AShape aisF = new(f.TopoFace);
        //    Display(aisF, false);
        //}
        //Random random = new();
        //foreach (var n in BendingTree.AllNodes)
        //{
        //    int i = random.Next(ColorMap.Colors.Count);
        //    foreach (var f in n.FaceSet)
        //    {
        //        AShape aisF = new(f.TopoFace);
        //        Display(aisF, false);
        //        SetColor(aisF, ColorMap.Colors[i], false);
        //    }
        //}
        #endregion
        Update();
    }

    // 递归创建 TreeViewItem
    private TreeViewItem CreateTreeViewItem(Node node)
    {
        TreeViewItem treeViewItem =
            new()
            {
                Header =
                    $"{node.MainFace} | 角度:{node.BendingAngle?.ToDegrees(1)} | 平移长度: {Math.Round(node.FlatLength ?? 0.0, 3)} | 折弯边厚:  {node.Bending?.Thickness}", // 或其他合适的属性
                Tag = node
            };
        // 绑定事件
        treeViewItem.Selected += TreeViewItem_Selected;
        treeViewItem.Unselected += TreeViewItem_Unselected;
        treeViewItem.IsExpanded = true;
        // 递归添加子节点
        foreach (Node child in node.Children)
        {
            treeViewItem.Items.Add(CreateTreeViewItem(child));
        }

        return treeViewItem;
    }

    // TreeViewItem 被选中时触发
    private void TreeViewItem_Selected(object sender, RoutedEventArgs e)
    {
        Random random = new();
        // 获取被选中的 TreeViewItem
        if (sender is TreeViewItem selectedItem && selectedItem.Tag is Node node)
        {
            int i = random.Next(ColorMap.Colors.Count);
            //int j = random.Next(ColorMap.Colors.Count);
            // 调用 Node 的 Show 方法
            node.Show(
                Context,
                true,
                _isBendingArrowVisible,
                _isMainFaceNormalVisible,
                ColorMap.Colors[i]
            );
        }

        // 防止事件向父级传播
        e.Handled = true;
    }

    // TreeViewItem 取消选中时触发
    private void TreeViewItem_Unselected(object sender, RoutedEventArgs e)
    {
        // 获取取消选中的 TreeViewItem
        if (sender is TreeViewItem unselectedItem && unselectedItem.Tag is Node node)
        {
            // 调用 Node 的 Erase 方法
            node.Erase(Context, true);
        }

        // 防止事件向父级传播
        e.Handled = true;
    }

    #endregion
    public void OnAISSelection(AShape theAIS)
    {
        if (theAIS.IsShape())
        {
            //TShape
            TShape shape = theAIS.Shape();
            if (shape.ShapeType() == ShapeEnum.FACE)
            {
                try
                {
                    TFace f = shape.AsFace();
                    //Surface BE = new(f);
                    Face face = new(f, InputWorkpiece);
                    double? angle = null;
                    if (face.CircleAngle != null)
                    {
                        angle = ((double)face.CircleAngle).ToDegrees(1);
                    }
                    double? radius = null;
                    if (face.CircleRadius != null)
                    {
                        radius = Math.Round((double)face.CircleRadius, 1);
                    }
                    Debug.WriteLine(
                        $"{face} | {face.Type} | Center: {face.CircleCenter} | Angle: {angle} | Radius: {radius}"
                    );
                }
                catch (Exception e)
                {
                    Debug.WriteLine($"{e}");
                }
            }
            if (shape.ShapeType() == ShapeEnum.EDGE)
            {
                try
                {
                    TEdge e = shape.AsEdge();
                    Curve BE = new(e);
                    (var p1, var p2) = Geometry.Tools.BrepGeomtryTools.GetEdgeEndPoints(e);
                    Debug.WriteLine(
                        $"{e.GetHashCode()} | {BE.GetType()} | {p1} | {p2} | D:{p1.Distance(p2)}"
                    );
                }
                catch (Exception e)
                {
                    Debug.WriteLine($"{e}");
                }
            }
        }
    }

    #region 选中所有树节点

    // 获取指定 TreeViewItem 的子项
    private IEnumerable<TreeViewItem> GetTreeViewItems(ItemsControl parent)
    {
        for (int i = 0; i < parent.Items.Count; i++)
        {
            TreeViewItem item = (TreeViewItem)parent.ItemContainerGenerator.ContainerFromIndex(i);

            if (item != null)
            {
                yield return item;
            }
        }
    }

    #endregion

    #region CheckBox
    private void ShowBendingArrow_CheckBox_Checked(object sender, RoutedEventArgs e)
    {
        _isBendingArrowVisible = true;
    }

    private void ShowBendingArrow_CheckBox_Unchecked(object sender, RoutedEventArgs e)
    {
        _isBendingArrowVisible = false;
    }

    private void ShowMainArrow_CheckBox_Checked(object sender, RoutedEventArgs e)
    {
        _isMainFaceNormalVisible = true;
    }

    private void ShowMainArrow_CheckBox_Unchecked(object sender, RoutedEventArgs e)
    {
        _isMainFaceNormalVisible = false;
    }

    #endregion

    #region Buttons

    #region ToolBar

    #region 视角
    private void ForntView_Button_Click(object sender, RoutedEventArgs e)
    {
        Canvas.SetViewOrientation(ViewOrientation.Front);
    }

    private void BackView_Button_Click(object sender, RoutedEventArgs e)
    {
        Canvas.SetViewOrientation(ViewOrientation.Back);
    }

    private void TopView_Button_Click(object sender, RoutedEventArgs e)
    {
        Canvas.SetViewOrientation(ViewOrientation.Top);
    }

    private void BottomView_Button_Click(object sender, RoutedEventArgs e)
    {
        Canvas.SetViewOrientation(ViewOrientation.Bottom);
    }

    private void LeftView_Button_Click(object sender, RoutedEventArgs e)
    {
        Canvas.SetViewOrientation(ViewOrientation.Left);
    }

    private void RightView_Button_Click(object sender, RoutedEventArgs e)
    {
        Canvas.SetViewOrientation(ViewOrientation.Right);
    }
    #endregion

    #region 选择模式

    private void SelectShape_Button_Click(object sender, RoutedEventArgs e)
    {
        Canvas.SetDefaultSelectionMode(SelectionMode.Shape);
        Debug.WriteLine(SelectionMode.Shape.ToString());
    }

    private void SelectFace_Button_Click(object sender, RoutedEventArgs e)
    {
        Canvas.SetDefaultSelectionMode(SelectionMode.Face);
        Debug.WriteLine(SelectionMode.Face.ToString());
    }

    private void SelectEdge_Button_Click(object sender, RoutedEventArgs e)
    {
        Canvas.SetDefaultSelectionMode(SelectionMode.Edge);
        Debug.WriteLine(SelectionMode.Edge.ToString());
    }

    private void SelectVertex_Button_Click(object sender, RoutedEventArgs e)
    {
        Canvas.SetDefaultSelectionMode(SelectionMode.Vertex);
        Debug.WriteLine(SelectionMode.Vertex.ToString());
    }

    private void SelectShell_Button_Click(object sender, RoutedEventArgs e)
    {
        Canvas.SetDefaultSelectionMode(SelectionMode.Shell);
        Debug.WriteLine(SelectionMode.Shell.ToString());
    }

    #endregion

    #endregion

    private void InputWorkPiece_Button_Click(object sender, RoutedEventArgs e)
    {
        // 创建文件选择对话框
        OpenFileDialog openFileDialog = new OpenFileDialog
        {
            Filter = "STEP Files (*.STEP;*.step;*.stp)|*.STEP;*.step;*.stp|All Files (*.*)|*.*", // 设置文件过滤器
            InitialDirectory = System.IO.Path.Combine(
                AppDomain.CurrentDomain.BaseDirectory,
                "D:\\Projects\\FANUC折弯机器人\\折弯离线编程软件参考资料"
            ) // 设置初始目录为指定的路径
            //InitialDirectory = System.IO.Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "mods") // 设置初始目录为指定的路径
        };

        // 如果用户选择了文件并点击了“打开”按钮
        if (openFileDialog.ShowDialog() == true)
        {
            string selectedFilePath = openFileDialog.FileName; // 获取选择的文件路径
            //更新Label
            InputWorkpiece_Label.Content = $"导入工件：{System.IO.Path.GetFileName(selectedFilePath)}";
            ;
            TShape inputWorkpiece = new STEPExchange(selectedFilePath); // 使用选择的文件路径
            //包围盒左下角点作为原点
            BoundingBox bnd = new(inputWorkpiece);
            var AABB = bnd.GetAABB();
            Trsf toOrigin = new();
            toOrigin.SetTranslation(AABB.CornerMin(), new());
            InputWorkpiece = new Transform(inputWorkpiece, toOrigin);
            EraseAll(false);
            AISInputWorkpiece = new(InputWorkpiece);
            Display(AISInputWorkpiece, false);
            SetTransparency(AISInputWorkpiece, 0.4, false);
            Update();
            FitAll();

            //! DEBUG 读取组合体
            //STEPExchange ex = new(selectedFilePath); // 使用选择的文件路径
            //EraseAll(false);
            //Canvas.AISContext.Display(ex.AssemblyShape(), true);
            FitAll();
        }
    }

    private void Unfold_Button_Click(object sender, RoutedEventArgs e)
    {
        if (InputWorkpiece == null)
        {
            MessageBox.Show("请先导入工件", "提示", MessageBoxButton.OK, MessageBoxImage.Warning);
            return;
        }
        CreateBendingTree();
        UpdateTestBox_StackPanel();
    }

    private void ShowAllNode_Button_Click(object sender, RoutedEventArgs e)
    {
        if (BendingTree == null)
        {
            MessageBox.Show("请计算折弯树", "提示", MessageBoxButton.OK, MessageBoxImage.Warning);
            return;
        }
        EraseAll(false);
        Display(AISInputWorkpiece, false);
        SetTransparency(AISInputWorkpiece, 0.8, false);
        BendingTree.FoldAllBendings();
        BendingTree.RootNode.Show(Context, false, false, _isMainFaceNormalVisible, ColorMap.Red);
        Random random = new();
        foreach (Node node in BendingTree.AllNodes)
        {
            // 获取被选中的 TreeViewItem
            int i = random.Next(ColorMap.Colors.Count);
            node.Show(
                Context,
                false,
                _isBendingArrowVisible,
                _isMainFaceNormalVisible,
                ColorMap.Colors[i]
            );
        }
        Update();
    }

    private void ShowUnfold_Button_Click(object sender, RoutedEventArgs e)
    {
        EraseAll(false);
        Display(AISInputWorkpiece, false);
        SetTransparency(AISInputWorkpiece, 0.8, false);
        BendingTree.UnfoldAllBendings();
        BendingTree.RootNode.Show(Context, false, false, _isMainFaceNormalVisible, ColorMap.Red);
        Random random = new();
        foreach (Node node in BendingTree.AllNodes)
        {
            int i = random.Next(ColorMap.Colors.Count);
            node.Show(
                Context,
                false,
                _isBendingArrowVisible,
                _isMainFaceNormalVisible,
                ColorMap.Colors[i]
            );
        }
        Update();
    }

    private void ShowWorkPiece_Button_Click(object sender, RoutedEventArgs e)
    {
        if (InputWorkpiece == null || AISInputWorkpiece == null)
        {
            MessageBox.Show("请先导入工件", "提示", MessageBoxButton.OK, MessageBoxImage.Warning);
            return;
        }

        EraseAll(false);
        Display(AISInputWorkpiece, false);
        SetTransparency(AISInputWorkpiece, 0.4, false);
        Update();
    }

    private void ShowSector_Button_Click(object sender, RoutedEventArgs e)
    {
        // 获取当前选中的 TreeViewItem
        TreeViewItem? selectedItem = BendingTree_TreeView.SelectedItem as TreeViewItem;

        if (selectedItem == null)
        {
            MessageBox.Show("请先选择一个 TreeViewItem。");
            return;
        }

        // 从 TreeViewItem 的 Tag 中获取 Node 对象
        Node? selectedNode = selectedItem.Tag as Node;

        if (selectedNode == null)
        {
            MessageBox.Show("无法找到关联的 Node 对象。");
            return;
        }
        // 执行 Node 的 ShowSectors 方法
        selectedNode.ShowSectors(Context, false);
        Update();
    }
    #endregion
}
