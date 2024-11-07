using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Forms.Integration;
using System.Windows.Forms.VisualStyles;
using System.Windows.Input;
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
        OCCCanvas = new OCCCanvas();
        aHost.Child = OCCCanvas;
        Canvas_Grid.Children.Add(aHost);
        OCCCanvas.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
        OCCCanvas.Show();
        OCCCanvas.ShowGraduatedTrihedron = false;
        OCCCanvas.OnAISSelectionEvent += OnAISSelection;
    }

    #region 窗口属性和字段

    private readonly OCCCanvas OCCCanvas;
    private InteractiveContext Context => OCCCanvas.AISContext;
    private bool _isBendingArrowVisible = false;
    private bool _isMainFaceNormalVisible = false;

    //简化函数调用
    private Action<InteractiveObject, bool> Display => OCCCanvas.Display;
    private Action<InteractiveObject, Color, bool> SetColor => Context.SetColor;
    private Action<InteractiveObject, double, bool> SetTransparency => Context.SetTransparency;
    private Action<InteractiveObject, bool> Erase => OCCCanvas.Erase;
    private Action<bool> EraseAll => OCCCanvas.EraseAll;
    private Action Update => OCCCanvas.Update;
    private Action FitAll => OCCCanvas.FitAll;

    #endregion

    #region 算法逻辑

    public TShape InputWorkpiece { get; private set; }
    private AShape AISInputWorkpiece;

    public BendingTree BendingTree { get; private set; }

    public void CreateBendingTree()
    {
        try
        {
            BendingTree = new BendingTree(InputWorkpiece, debugContext: Context);
        }
        catch (Exception e)
        {
            MessageBox.Show($"折弯解析失败，错误：\n{e}", "警告", MessageBoxButton.OK, MessageBoxImage.Warning);
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
        //foreach (var n in BendingTree.AllLeafNodes)
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
        #region 列出所有面
        List<Face> allFaces = BendingTree.AllFaces.ToList().OrderBy(f => f.Index).ToList();
        foreach (Face face in allFaces)
        {
            TestBox_StackPanel.Children.Add(CreateStackItem(face));
        }
        #endregion
        Update();
    }

    // 递归创建 TreeViewItem
    private TreeViewItem CreateTreeViewItem(NodeDS node)
    {
        TreeViewItem? treeViewItem = null;
        if (node is RootNode root)
        {
            treeViewItem = new() { Header = $"根节点：{root.MainFace}", Tag = root };
        }
        if (node is LeafNode leaf)
        {
            treeViewItem = new()
            {
                Header =
                    $"{leaf.MainFace} | 角度:{leaf.BendingAngle.ToDegrees(1)} | 平移长度: {Math.Round(leaf.FlatLength, 3)} | 折弯边厚:  {leaf.Bending?.Thickness}", // 或其他合适的属性
                Tag = leaf
            };
        }
        if (treeViewItem == null)
        {
            throw new Exception("未知节点");
        }
        // 绑定事件
        treeViewItem.Selected += TreeViewItem_Selected;
        treeViewItem.Unselected += TreeViewItem_Unselected;
        treeViewItem.IsExpanded = true;
        // 递归添加子节点
        foreach (NodeDS child in node.Children)
        {
            treeViewItem.Items.Add(CreateTreeViewItem(child));
        }

        return treeViewItem;
    }

    private Grid CreateStackItem(Face face)
    {
        Grid grid = new Grid();
        // 定义显示内容和列索引
        var labelDefinitions = new (string Content, int ColumnIndex)[]
        {
            ($"{face.Index}", 0),
            ($"L: {face.Type}", 1),
        };

        // 创建并添加列定义
        for (int i = 0; i < labelDefinitions.Length + 1; i++)
        {
            ColumnDefinition column =
                new()
                {
                    Width =
                        i == 0
                            ? new GridLength(1, GridUnitType.Star)
                            : new GridLength(1, GridUnitType.Auto)
                };
            grid.ColumnDefinitions.Add(column);
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
            grid.Children.Add(label);
        }
        void FaceLabel_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            // 获取点击的 Label
            Label? clickedLabel = sender as Label;

            // 确保 Label 不为空且 Tag 不为空
            if (clickedLabel == null || clickedLabel.Tag == null)
                return;
            // 获取 StackPanel
            StackPanel? parentStack = ((Grid)clickedLabel.Parent).Parent as StackPanel;

            if ((Grid)clickedLabel.Parent == null || parentStack == null)
                return;
            // 获取被点击 Label 所在的行
            int clickedRow = parentStack.Children.IndexOf((Grid)clickedLabel.Parent);

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
                    if (row != clickedRow)
                    {
                        label.Background = Brushes.Transparent;
                        continue;
                    }

                    label.Background = Brushes.LightGreen;
                }
            }
            // 选中 AIS
            var piece = clickedLabel.Tag as Face;
            if (piece != null && piece.TopoFace != null)
            {
                AShape faceAIS = new AShape(piece.TopoFace);
                Context.Display(faceAIS, true);
                Context.SetColor(faceAIS, ColorMap.Red, true);
            }
        }

        // 绑定 thePiece 对象到第一个 Label 的 Tag 属性
        if (grid.Children[0] is Label firstLabel)
        {
            firstLabel.Tag = face;
            // 添加鼠标左键点击事件处理程序
            firstLabel.MouseLeftButtonDown += FaceLabel_MouseLeftButtonDown;
        }
        return grid;
    }

    // TreeViewItem 被选中时触发
    private void TreeViewItem_Selected(object sender, RoutedEventArgs e)
    {
        Random random = new();
        // 获取被选中的 TreeViewItem
        if (sender is TreeViewItem selectedItem && selectedItem.Tag is NodeDS node)
        {
            int i = random.Next(ColorMap.Colors.Count);
            //int j = random.Next(ColorMap.Colors.Count);
            // 调用 Node 的 Show 方法
            if (node is RootNode root)
            {
                root.Show(Context, true);
            }
            if (node is LeafNode leaf)
            {
                leaf.BendingColor = ColorMap.Colors[i];
                leaf.Show(Context, true, _isBendingArrowVisible, _isMainFaceNormalVisible);
            }
        }

        // 防止事件向父级传播
        e.Handled = true;
    }

    // TreeViewItem 取消选中时触发
    private void TreeViewItem_Unselected(object sender, RoutedEventArgs e)
    {
        // 获取取消选中的 TreeViewItem
        if (sender is TreeViewItem unselectedItem && unselectedItem.Tag is NodeDS node)
        {
            // 调用 Node 的 Erase 方法
            node.RemoveSelf(true);
        }

        // 防止事件向父级传播
        e.Handled = true;
    }

    // 搜索框内容变化时执行的事件
    private void OnSearchTextChanged(object sender, TextChangedEventArgs e)
    {
        var searchText = SearchTextBox.Text.ToLower();
        foreach (var child in TestBox_StackPanel.Children.OfType<Grid>())
        {
            if (child.Children[0] is Label label && label.Content is string content)
            {
                child.Visibility = content.Contains(
                    searchText,
                    StringComparison.CurrentCultureIgnoreCase
                )
                    ? Visibility.Visible
                    : Visibility.Collapsed;
            }
        }
    }

    // 重置按钮点击事件，恢复显示所有项
    private void OnResetButtonClick(object sender, RoutedEventArgs e)
    {
        SearchTextBox.Text = string.Empty;
        foreach (var child in TestBox_StackPanel.Children.OfType<Grid>())
        {
            child.Visibility = Visibility.Visible;
        }
    }

    #endregion
    public void OnAISSelection(AShape? theAIS)
    {
        if (theAIS == null)
        {
            return;
        }
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
                    UpdateSelectFace(f);
                    if (BendingTree == null)
                        return;
                    LeafNode? selectNode = BendingTree
                        .BendingFaceMap.Where(face => face.Key.IsEqual(f)) // 筛选出符合条件的元素
                        .Select(face => face.Value) // 获取对应的 value
                        .FirstOrDefault(); // 获取第一个匹配的值，若没有则为 null
                    if (selectNode != null)
                    {
                        selectNode.Unfolded = !selectNode.Unfolded;
                        selectNode.Show(
                            Context,
                            false,
                            _isBendingArrowVisible,
                            _isMainFaceNormalVisible
                        );
                        foreach (var node in BendingTree.GetAllChildNodes(selectNode))
                        {
                            node.Show(
                                Context,
                                false,
                                _isBendingArrowVisible,
                                _isMainFaceNormalVisible
                            );
                        }
                        Context.UpdateCurrentViewer();
                        Debug.WriteLine($"选中了节点{selectNode}");
                    }
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
                        $"{e.GetHashCode()} | {BE.GetType()} | {p1} | {p2} | D:{p1.Distance(p2)} | Orientation: {e.Orientation}"
                    );
                }
                catch (Exception e)
                {
                    Debug.WriteLine($"{e}");
                }
            }
        }
    }

    private void UpdateSelectFace(TFace face)
    {
        selectedFace2_StackPanel.Children.Clear();
        while (selectedFace1_StackPanel.Children.Count > 0)
        {
            UIElement item = selectedFace1_StackPanel.Children[0];
            selectedFace1_StackPanel.Children.Remove(item);
            selectedFace2_StackPanel.Children.Add(item);
        }
        selectedFace2_Label.Content = selectedFace1_Label.Content;
        selectedFace1_Label.Content = face.GetHashCode();
        Face f = new(face, InputWorkpiece);
        foreach (var edge in f.Edges)
        {
            Label label =
                new()
                {
                    Content = $"{edge.Index} |{edge.Type}| {edge.Points[0]} | {edge.Points[1]}"
                };
            selectedFace1_StackPanel.Children.Add(label);
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
        OCCCanvas.SetViewOrientation(ViewOrientation.Front);
    }

    private void BackView_Button_Click(object sender, RoutedEventArgs e)
    {
        OCCCanvas.SetViewOrientation(ViewOrientation.Back);
    }

    private void TopView_Button_Click(object sender, RoutedEventArgs e)
    {
        OCCCanvas.SetViewOrientation(ViewOrientation.Top);
    }

    private void BottomView_Button_Click(object sender, RoutedEventArgs e)
    {
        OCCCanvas.SetViewOrientation(ViewOrientation.Bottom);
    }

    private void LeftView_Button_Click(object sender, RoutedEventArgs e)
    {
        OCCCanvas.SetViewOrientation(ViewOrientation.Left);
    }

    private void RightView_Button_Click(object sender, RoutedEventArgs e)
    {
        OCCCanvas.SetViewOrientation(ViewOrientation.Right);
    }
    #endregion

    #region 选择模式

    private void SelectShape_Button_Click(object sender, RoutedEventArgs e)
    {
        OCCCanvas.SetDefaultSelectionMode(SelectionMode.Shape);
        Debug.WriteLine(SelectionMode.Shape.ToString());
    }

    private void SelectFace_Button_Click(object sender, RoutedEventArgs e)
    {
        OCCCanvas.SetDefaultSelectionMode(SelectionMode.Face);
        Debug.WriteLine(SelectionMode.Face.ToString());
    }

    private void SelectEdge_Button_Click(object sender, RoutedEventArgs e)
    {
        OCCCanvas.SetDefaultSelectionMode(SelectionMode.Edge);
        Debug.WriteLine(SelectionMode.Edge.ToString());
    }

    private void SelectVertex_Button_Click(object sender, RoutedEventArgs e)
    {
        OCCCanvas.SetDefaultSelectionMode(SelectionMode.Vertex);
        Debug.WriteLine(SelectionMode.Vertex.ToString());
    }

    private void SelectShell_Button_Click(object sender, RoutedEventArgs e)
    {
        OCCCanvas.SetDefaultSelectionMode(SelectionMode.Shell);
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
            //OCCCanvas.AISContext.Display(ex.AssemblyShape(), true);
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
        BendingTree.RootNode.Show(Context, false);
        Random random = new();
        foreach (LeafNode node in BendingTree.AllLeafNodes)
        {
            // 获取被选中的 TreeViewItem
            int i = random.Next(ColorMap.Colors.Count);
            node.BendingColor = ColorMap.Colors[i];
            node.Show(Context, false, _isBendingArrowVisible, _isMainFaceNormalVisible);
        }
        foreach (var node in BendingTree.NodesAfterHem)
        {
            node.BendingColor = ColorMap.Yellow;
            node.Show(Context, false, _isBendingArrowVisible, _isMainFaceNormalVisible);
        }
        Update();
    }

    private void ShowUnfold_Button_Click(object sender, RoutedEventArgs e)
    {
        EraseAll(false);
        Display(AISInputWorkpiece, false);
        SetTransparency(AISInputWorkpiece, 0.8, false);
        BendingTree.UnfoldAllBendings();
        BendingTree.RootNode.Show(Context, false);
        Random random = new();
        foreach (LeafNode node in BendingTree.AllLeafNodes)
        {
            int i = random.Next(ColorMap.Colors.Count);
            node.BendingColor = ColorMap.Colors[i];
            node.Show(Context, false, _isBendingArrowVisible, _isMainFaceNormalVisible);
        }
        foreach (var node in BendingTree.NodesAfterHem)
        {
            node.BendingColor = ColorMap.Yellow;
            node.Show(Context, false, _isBendingArrowVisible, _isMainFaceNormalVisible);
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
        NodeDS? selectedNode = selectedItem.Tag as NodeDS;

        if (selectedNode == null)
        {
            MessageBox.Show("无法找到关联的 Node 对象。");
            return;
        }
        if (selectedNode is RootNode)
        {
            MessageBox.Show("请选择叶子节点");
            return;
        }
        if (selectedNode is LeafNode leaf)
        {
            // 执行 Node 的 ShowSectors 方法
            leaf.CalculateSectors(5);
            leaf.ShowSectors(Context, false);
        }

        Update();
    }

    private void ShowAllBendings_Button_Click(object sender, RoutedEventArgs e)
    {
        EraseAll(false);
        Display(AISInputWorkpiece, false);
        SetTransparency(AISInputWorkpiece, 0.8, false);
        BendingTree.RootNode.Show(Context, false);
        foreach (LeafNode node in BendingTree.AllLeafNodes)
        {
            node.BendingColor = ColorMap.Green;
            node.Show(Context, false, _isBendingArrowVisible, _isMainFaceNormalVisible);
        }
        foreach (var node in BendingTree.NodesAfterHem)
        {
            node.BendingColor = ColorMap.Yellow;
            node.Show(Context, false, _isBendingArrowVisible, _isMainFaceNormalVisible);
        }
        Update();
    }
    #endregion
}
