using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Diagnostics;
using System.Linq;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Forms.Integration;
using System.Windows.Forms.VisualStyles;
using Microsoft.Windows.Themes;
using OCCTK.Ex;
using OCCTK.IO;
using OCCTK.Laser;
using OCCTK.OCC.AIS;
using OCCTK.OCC.BRepPrimAPI;
using OCCTK.OCC.gp;
using OCCTK.OCC.TopoDS;
using OCCViewForm;

namespace TestWPF;

public class InputWorkpiece
{
    public InputWorkpiece(WTopoDS_Shape theShape)
    {
        _InputWorkpiece = theShape;
        _AISInputWorkpiece = new WAIS_Shape(theShape);
    }

    private WTopoDS_Shape _InputWorkpiece;
    private WAIS_Shape _AISInputWorkpiece;
    public WTopoDS_Shape Shape
    {
        get { return _InputWorkpiece; }
    }
    public WAIS_Shape AIS
    {
        get { return _AISInputWorkpiece; }
    }
}

/// <summary>
/// 创建竖板的模式 Vertical Plate Creation Mode
/// </summary>
public enum VPCMode
{
    Num,
    Offset,
}

/// <summary>
/// 交互逻辑 MainWindow.xaml
/// </summary>
public partial class MainWindow : Window
{
    private OCCCanvas Viewer;
    private WAIS_InteractiveContext AISContext;

    private bool _useManipulator = false;

    /// <summary>
    /// 创建的竖板列表
    /// </summary>
    public List<VerticalPlate> VerticalPlates = new List<VerticalPlate>();

    /// <summary>
    /// 展平后的结果
    /// </summary>
    public WTopoDS_Shape CombinedFixtureBoard;

    /// <summary>
    /// 输入的工件
    /// </summary>
    private InputWorkpiece InputWorkpiece;

    /// <summary>
    /// 当前展示的AIS对象列表
    /// </summary>
    private List<WAIS_Shape> _CurrentAIS = new List<WAIS_Shape>();

    // 显示AIS，并加入当前AIS列表
    private void Display(WAIS_Shape theAIS, bool update)
    {
        _CurrentAIS.Add(theAIS);
        AISContext.Display(theAIS, update);
    }

    // 移除AIS列表中的所有AIS对象
    private void EraseAll(bool update)
    {
        var aisToRemove = _CurrentAIS.ToList();
        foreach (var theAIS in aisToRemove)
        {
            _CurrentAIS.Remove(theAIS);
            AISContext.Erase(theAIS, update);
        }
    }

    #region 创建横板参数
    /// <summary>
    /// 横板
    /// </summary>
    private BasePlate? BasePlate;

    public double BasePlateOffsetX { get; set; } = 5;

    public double BasePlateOffsetY { get; set; } = 5;

    public double BasePlateOffsetZ { get; set; } = 100.0;

    private static readonly Wgp_Dir wgp_Dir = new Wgp_Dir(1.0, 0.0, 0.0);

    #endregion

    #region 创建竖板参数
    private static readonly Wgp_Dir DirectionX = new(1.0, 0.0, 0.0);
    private static readonly Wgp_Dir DirectionY = new(0.0, 1.0, 0.0);

    /// <summary>
    /// 创建竖板所用模式
    /// </summary>
    public VPCMode VPCMode { get; set; } = 0;

    public int XNum { get; set; } = 3;

    public int YNum { get; set; } = 3;

    //X方向
    /// <summary>
    /// 竖板横向初始偏移
    /// </summary>
    public double InitialOffsetXParameter { get; set; } = 5;

    /// <summary>
    /// 竖板横向偏移
    /// </summary>
    public double OffsetXParameter { get; set; } = 20;

    //Y方向
    /// <summary>
    /// 竖板纵向初始偏移
    /// </summary>
    public double InitialOffsetYParameter { get; set; } = 5;

    /// <summary>
    /// 竖板纵向偏移
    /// </summary>
    public double OffsetYParameter { get; set; } = 20;

    #endregion

    #region 竖板全局参数
    //Z方向
    /// <summary>
    /// 竖板连接高
    /// </summary>
    public double ConnectionHeightParameter { get; set; } = 20;

    /// <summary>
    /// 竖板最小支撑长度
    /// </summary>
    public double MinSupportingLenParameter { get; set; } = 5;

    //XY方向
    /// <summary>
    /// 竖板避让间隙
    /// </summary>
    public double ClearancesParameter { get; set; } = 4;

    /// <summary>
    /// 竖板切断距离
    /// </summary>
    public double CuttingDistanceParameter { get; set; } = 500;

    /// <summary>
    /// 连接卡槽宽度
    /// </summary>
    public double ConnectionThicknessParameter { get; set; } = 5.0;
    public double FilletRadiusParameter { get; set; } = 2.0;

    #endregion

    #region 单块板属性

    public VerticalPlate? CurrentPlate { get; set; }

    #endregion

    public MainWindow()
    {
        InitializeComponent();
        // 创建 Windows Forms 控件和 WindowsFormsHost
        WindowsFormsHost aHost = new WindowsFormsHost();
        Viewer = new OCCCanvas();
        AISContext = Viewer.InteractiveContext;
        Viewer.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
        Viewer.Show();
        aHost.Child = Viewer;
        canvas_grid.Children.Add(aHost);
        #region 全局属性绑定

        BasePlateOffsetX_TextBox.Text = BasePlateOffsetX.ToString();
        BasePlateOffsetX_TextBox.TextChanged += BasePlateOffsetX_TextChanged;

        BasePlateOffsetY_TextBox.Text = BasePlateOffsetY.ToString();
        BasePlateOffsetY_TextBox.TextChanged += BasePlateOffsetY_TextChanged;

        BasePlateHight_TextBox.Text = BasePlateOffsetZ.ToString();
        BasePlateOffsetY_TextBox.TextChanged += BasePlateOffsetZ_TextChanged;

        InitialOffsetX_TextBox.Text = InitialOffsetXParameter.ToString();
        InitialOffsetX_TextBox.TextChanged += InitialOffsetX_TextChanged;

        // 设置OffsetXLabel的内容和TextBox的初始值，并添加TextChanged事件处理程序
        OffsetX_TextBox.Text = OffsetXParameter.ToString();
        OffsetX_TextBox.TextChanged += OffsetX_TextChanged;

        // 设置InitialOffsetYLabel的内容和TextBox的初始值，并添加TextChanged事件处理程序
        InitialOffsetY_TextBox.Text = InitialOffsetYParameter.ToString();
        InitialOffsetY_TextBox.TextChanged += InitialOffsetY_TextChanged;

        // 设置OffsetYLabel的内容和TextBox的初始值，并添加TextChanged事件处理程序
        OffsetY_TextBox.Text = OffsetYParameter.ToString();
        OffsetY_TextBox.TextChanged += OffsetY_TextChanged;

        // 设置ConnectionHeightLabel的内容和TextBox的初始值，并添加TextChanged事件处理程序
        ConnectionHeight_TextBox.Text = ConnectionHeightParameter.ToString();
        ConnectionHeight_TextBox.TextChanged += ConnectionHeight_TextChanged;

        // 设置MinSupportingLenLabel的内容和TextBox的初始值，并添加TextChanged事件处理程序
        MinSupportingLen_TextBox.Text = MinSupportingLenParameter.ToString();
        MinSupportingLen_TextBox.TextChanged += MinSupportingLen_TextChanged;

        // 设置ClearancesLabel的内容和TextBox的初始值，并添加TextChanged事件处理程序
        Clearances_TextBox.Text = ClearancesParameter.ToString();
        Clearances_TextBox.TextChanged += Clearances_TextChanged;

        // 设置CuttingDistanceLabel的内容和TextBox的初始值，并添加TextChanged事件处理程序
        CuttingDistance_TextBox.Text = CuttingDistanceParameter.ToString();
        CuttingDistance_TextBox.TextChanged += CuttingDistance_TextChanged;

        ConnectionThickness_TextBox.Text = ConnectionThicknessParameter.ToString();
        ConnectionThickness_TextBox.TextChanged += ConnectionThickness_TextChanged;

        FilletRadius_TextBox.Text = FilletRadiusParameter.ToString();
        FilletRadius_TextBox.TextChanged += FilletRadius_TextChanged;
        #endregion
        #region 创建竖板
        XNum_TextBox.Text = XNum.ToString();
        XNum_TextBox.TextChanged += XNum_TextChanged;
        YNum_TextBox.Text = YNum.ToString();
        YNum_TextBox.TextChanged += YNum_TextChanged;
        #endregion
        #region 单块板的属性
        #endregion
    }

    #region 视角
    private void ForntView_Button_Click(object sender, RoutedEventArgs e)
    {
        Viewer.SetViewOrientation(ViewOrientation.Front);
    }

    private void BackView_Button_Click(object sender, RoutedEventArgs e)
    {
        Viewer.SetViewOrientation(ViewOrientation.Back);
    }

    private void TopView_Button_Click(object sender, RoutedEventArgs e)
    {
        Viewer.SetViewOrientation(ViewOrientation.Top);
    }

    private void BottomView_Button_Click(object sender, RoutedEventArgs e)
    {
        Viewer.SetViewOrientation(ViewOrientation.Bottom);
    }

    private void LeftView_Button_Click(object sender, RoutedEventArgs e)
    {
        Viewer.SetViewOrientation(ViewOrientation.Left);
    }

    private void RightView_Button_Click(object sender, RoutedEventArgs e)
    {
        Viewer.SetViewOrientation(ViewOrientation.Right);
    }

    private void Wireframe_Button_Click(object sender, RoutedEventArgs e)
    {
        Viewer.SetDisplayMode(DisplayMode.Wireframe);
    }

    private void Shading_Button_Click(object sender, RoutedEventArgs e)
    {
        Viewer.SetDisplayMode(DisplayMode.Shading);
    }
    #endregion

    #region 测试
    private void FitAll_Button_Click(object sender, RoutedEventArgs e)
    {
        Viewer.FitAll();
    }

    private void Test_Button_Click(object sender, RoutedEventArgs e)
    {
        InputWorkpiece = new(new WBRepPrimAPI_MakeBox(20, 20, 0.1).Shape());
        Viewer.Display(InputWorkpiece.AIS, true);
        Viewer.FitAll();
    }
    #endregion

    #region 导入
    private void Test_Input_Button_Click(object sender, RoutedEventArgs e)
    {
        Viewer.EraseAll();
        //InputWorkpiece = SimpleClampMaker.TestInputWorkpiece("mods\\mytest.STEP");
        //InputWorkpiece = SimpleClampMaker.TestInputWorkpiece("mods\\test1Small.STEP");
        InputWorkpiece = new(new STEPExchange("mods\\test4Small.STEP").Shape());
        BasePlate = null;
        Viewer.Display(InputWorkpiece.AIS, true);
        Viewer.FitAll();
    }

    private void Test_input_test_1_Click(object sender, RoutedEventArgs e)
    {
        Viewer.EraseAll();
        InputWorkpiece = new(new STEPExchange("mods\\test1.stp").Shape());
        BasePlate = null;
        Viewer.Display(InputWorkpiece.AIS, true);
        Viewer.FitAll();
    }

    private void Test_input_test_2_Click(object sender, RoutedEventArgs e)
    {
        Viewer.EraseAll();
        InputWorkpiece = new(new STEPExchange("mods\\test2.stp").Shape());
        BasePlate = null;
        Viewer.Display(InputWorkpiece.AIS, true);
        Viewer.FitAll();
    }

    private void Test_input_test_3_Click(object sender, RoutedEventArgs e)
    {
        Viewer.EraseAll();
        InputWorkpiece = new(new STEPExchange("mods\\test3.STEP").Shape());
        BasePlate = null;
        Viewer.Display(InputWorkpiece.AIS, true);
        Viewer.FitAll();
    }

    private void Test_input_test_4_Click(object sender, RoutedEventArgs e)
    {
        Viewer.EraseAll();
        InputWorkpiece = new(new STEPExchange("mods\\test4.stp").Shape());
        BasePlate = null;
        Viewer.Display(InputWorkpiece.AIS, true);
        Viewer.FitAll();
    }

    private void Test_input_test_5_Click(object sender, RoutedEventArgs e)
    {
        Viewer.EraseAll();
        InputWorkpiece = new(new STEPExchange("mods\\test5.stp").Shape());
        BasePlate = null;
        Viewer.Display(InputWorkpiece.AIS, true);
        Viewer.FitAll();
    }

    #endregion

    #region 选择模式
    private void Select_Shape_Button_Click(object sender, RoutedEventArgs e)
    {
        Viewer.SetSelectionMode(OCCTK.Visualization.SelectionMode.Shape);
        Debug.WriteLine(OCCTK.Visualization.SelectionMode.Shape.ToString());
    }

    private void Select_Face_Button_Click(object sender, RoutedEventArgs e)
    {
        Viewer.SetSelectionMode(OCCTK.Visualization.SelectionMode.Face);
    }

    private void Select_Wire_Button_Click(object sender, RoutedEventArgs e)
    {
        Viewer.SetSelectionMode(OCCTK.Visualization.SelectionMode.Edge);
    }

    private void Select_Vertex_Button_Click(object sender, RoutedEventArgs e)
    {
        Viewer.SetSelectionMode(OCCTK.Visualization.SelectionMode.Vertex);
    }

    #endregion
    private void Erase_Button_Click(object sender, RoutedEventArgs e)
    {
        Viewer.EraseSelect();
    }

    #region 监听事件
    private void BasePlateOffsetX_TextChanged(object sender, EventArgs e)
    {
        // 获取TextBox的新值
        TextBox textBox = (TextBox)sender;
        double newValue;
        if (double.TryParse(textBox.Text, out newValue))
        {
            // 更新 BasePlateOffsetX 的值
            BasePlateOffsetX = newValue;
        }
    }

    private void BasePlateOffsetY_TextChanged(object sender, EventArgs e)
    {
        // 获取TextBox的新值
        TextBox textBox = (TextBox)sender;
        double newValue;
        if (double.TryParse(textBox.Text, out newValue))
        {
            // 更新 BasePlateOffsetY 的值
            BasePlateOffsetY = newValue;
        }
    }

    private void BasePlateOffsetZ_TextChanged(object sender, EventArgs e)
    {
        // 获取TextBox的新值
        TextBox textBox = (TextBox)sender;
        double newValue;
        if (double.TryParse(textBox.Text, out newValue))
        {
            // 更新 BasePlateOffsetZ 的值
            BasePlateOffsetZ = newValue;
        }
    }

    private void InitialOffsetX_TextChanged(object sender, EventArgs e)
    {
        // 获取TextBox的新值
        TextBox textBox = (TextBox)sender;
        double newValue;
        if (double.TryParse(textBox.Text, out newValue))
        {
            // 更新InitialOffsetXParameter的值
            InitialOffsetXParameter = newValue;
        }
    }

    private void OffsetX_TextChanged(object sender, EventArgs e)
    {
        // 获取TextBox的新值
        TextBox textBox = (TextBox)sender;
        double newValue;
        if (double.TryParse(textBox.Text, out newValue))
        {
            // 更新OffsetXParameter的值
            OffsetXParameter = newValue;
        }
    }

    private void InitialOffsetY_TextChanged(object sender, EventArgs e)
    {
        // 获取TextBox的新值
        TextBox textBox = (TextBox)sender;
        double newValue;
        if (double.TryParse(textBox.Text, out newValue))
        {
            // 更新InitialOffsetYParameter的值
            InitialOffsetYParameter = newValue;
        }
    }

    private void OffsetY_TextChanged(object sender, EventArgs e)
    {
        // 获取TextBox的新值
        TextBox textBox = (TextBox)sender;
        double newValue;
        if (double.TryParse(textBox.Text, out newValue))
        {
            // 更新OffsetYParameter的值
            OffsetYParameter = newValue;
        }
    }

    private void ConnectionHeight_TextChanged(object sender, EventArgs e)
    {
        // 获取TextBox的新值
        TextBox textBox = (TextBox)sender;
        double newValue;
        if (double.TryParse(textBox.Text, out newValue))
        {
            // 更新ConnectionHeightParameter的值
            ConnectionHeightParameter = newValue;
        }
    }

    private void MinSupportingLen_TextChanged(object sender, EventArgs e)
    {
        // 获取TextBox的新值
        TextBox textBox = (TextBox)sender;
        double newValue;
        if (double.TryParse(textBox.Text, out newValue))
        {
            // 更新MinSupportingLenParameter的值
            MinSupportingLenParameter = newValue;
        }
    }

    private void Clearances_TextChanged(object sender, EventArgs e)
    {
        // 获取TextBox的新值
        TextBox textBox = (TextBox)sender;
        double newValue;
        if (double.TryParse(textBox.Text, out newValue))
        {
            // 更新ClearancesParameter的值
            ClearancesParameter = newValue;
        }
    }

    private void CuttingDistance_TextChanged(object sender, EventArgs e)
    {
        // 获取TextBox的新值
        TextBox textBox = (TextBox)sender;
        double newValue;
        if (double.TryParse(textBox.Text, out newValue))
        {
            // 更新CuttingDistanceParameter的值
            CuttingDistanceParameter = newValue;
        }
    }

    private void XNum_TextChanged(object sender, EventArgs e)
    {
        // 获取TextBox的新值
        TextBox textBox = (TextBox)sender;
        int newValue;
        if (int.TryParse(textBox.Text, out newValue))
        {
            // 更新XNum的值
            XNum = newValue;
        }
    }

    private void YNum_TextChanged(object sender, EventArgs e)
    {
        // 获取TextBox的新值
        TextBox textBox = (TextBox)sender;
        int newValue;
        if (int.TryParse(textBox.Text, out newValue))
        {
            // 更新XNum的值
            YNum = newValue;
        }
    }

    private void ConnectionThickness_TextChanged(object sender, EventArgs e)
    {
        // 获取TextBox的新值
        TextBox textBox = (TextBox)sender;
        int newValue;
        if (int.TryParse(textBox.Text, out newValue))
        {
            // 更新XNum的值
            ConnectionThicknessParameter = newValue;
            if (FilletRadiusParameter > newValue / 2)
            {
                FilletRadiusParameter = Math.Round(ConnectionThicknessParameter / 2);
            }
        }
    }

    private void FilletRadius_TextChanged(object sender, EventArgs e)
    {
        // 获取TextBox的新值
        TextBox textBox = (TextBox)sender;
        int newValue;
        if (int.TryParse(textBox.Text, out newValue))
        {
            // 更新XNum的值
            FilletRadiusParameter = newValue;
        }
    }

    private void CurrentPlateDirection_ComboBox_SelectionChanged(
        object sender,
        SelectionChangedEventArgs e
    )
    {
        ComboBoxItem seletedItem = (ComboBoxItem)CurrentPlateDirection_ComboBox.SelectedItem;
        string selectedDirection = "";
        if (seletedItem != null)
        {
            selectedDirection = seletedItem.Content.ToString();
        }

        if (selectedDirection == "X")
        {
            CurrentPlateLocationX_ComboBox.Visibility = Visibility.Visible;
            CurrentPlateLocationY_ComboBox.Visibility = Visibility.Collapsed;
        }
        else if (selectedDirection == "Y")
        {
            CurrentPlateLocationX_ComboBox.Visibility = Visibility.Collapsed;
            CurrentPlateLocationY_ComboBox.Visibility = Visibility.Visible;
        }
    }

    private void CurrentPlateLocationX_ComboBox_SelectionChanged(
        object sender,
        SelectionChangedEventArgs e
    )
    {
        //清空列表
        CurrentPlate_StackPanel.Children.Clear();
        CurrentPlate = (VerticalPlate)CurrentPlateLocationX_ComboBox.SelectedItem;
        if (CurrentPlate != null)
        {
            //显示选中板的坐标
            CurrentPlateSelectedLocation_TextBox.Text = CurrentPlate
                .Direction.Point.X()
                .ToString("F1");
            //清空画布并重新显示
            Viewer.EraseAll();
            Viewer.Display(InputWorkpiece.AIS, false);
            Viewer.Display(BasePlate.AIS, false);
            if (CurrentPlate.AIS != null)
            {
                Viewer.Display(CurrentPlate.AIS, true);
                return;
            }
            foreach (var item in CurrentPlate.Pieces)
            {
                CurrentPlate_StackPanel.Children.Add(MakeStackItem(item));
            }
            Viewer.Viewer.UpdateCurrentViewer();
        }
    }

    private void CurrentPlateLocationY_ComboBox_SelectionChanged(
        object sender,
        SelectionChangedEventArgs e
    )
    {
        //清空列表
        CurrentPlate_StackPanel.Children.Clear();
        //更新当前选中的板
        CurrentPlate = (VerticalPlate)CurrentPlateLocationY_ComboBox.SelectedItem;
        if (CurrentPlate != null)
        {
            //显示选中板的坐标
            CurrentPlateSelectedLocation_TextBox.Text = CurrentPlate
                .Direction.Point.X()
                .ToString("F1");
            //清空画布并重新显示
            Viewer.EraseAll();
            Viewer.Display(InputWorkpiece.AIS, false);
            Viewer.Display(BasePlate.AIS, false);
            if (CurrentPlate.AIS != null)
            {
                Viewer.Display(CurrentPlate.AIS, true);
                return;
            }
            foreach (var item in CurrentPlate.Pieces)
            {
                CurrentPlate_StackPanel.Children.Add(MakeStackItem(item));
            }
            Viewer.Viewer.UpdateCurrentViewer();
        }
    }

    //单板

    private Grid MakeStackItem(VerticalPiece thePiece)
    {
        if (CurrentPlate == null)
        {
            return null; // 直接返回，不执行后续代码
        }
        // 创建一个新的 StackPanel
        Grid outerStackPanel = new Grid();

        // 创建列定义
        ColumnDefinition column1 = new ColumnDefinition();
        ColumnDefinition column2 = new ColumnDefinition();
        ColumnDefinition column3 = new ColumnDefinition();

        // 设置列宽度为相对值
        column1.Width = new GridLength(1, GridUnitType.Star);
        column2.Width = new GridLength(1, GridUnitType.Auto);
        column3.Width = new GridLength(1, GridUnitType.Auto);
        // 将列添加到 Grid 中
        outerStackPanel.ColumnDefinitions.Add(column1);
        outerStackPanel.ColumnDefinitions.Add(column2);
        outerStackPanel.ColumnDefinitions.Add(column3);

        // 创建要添加到 StackPanel 中的子元素
        Label PieceLabel = new Label();
        PieceLabel.Content = "位置: " + thePiece.Number.ToString();
        PieceLabel.HorizontalContentAlignment = HorizontalAlignment.Left; // 设置 Label 的水平内容对齐方式为 Left

        // 将子元素添加到 Grid 中，并指定列索引
        Grid.SetColumn(PieceLabel, 0);

        // 将子元素添加到 StackPanel 的 Children 集合中
        outerStackPanel.Children.Add(PieceLabel);
        //显示
        Viewer.Display(thePiece.AIS, false);
        return outerStackPanel;
    }
    #endregion
    // 更新当前板坐标
    private void UpdateCurrentPlateLocation_Button_Click(object sender, RoutedEventArgs e)
    {
        // 获取TextBox的新值
        double newValue;
        if (CurrentPlate != null)
        {
            //更新视图显示
            foreach (var item in CurrentPlate.Pieces)
            {
                Display(item.AIS, false);
            }
            Viewer.Viewer.UpdateCurrentViewer();
            // 更新CurrentPlateValueLable的值
            if (double.TryParse(CurrentPlateAddLocation_TextBox.Text, out newValue))
            {
                CurrentPlate = SimpleClampMaker.MakeVerticalPlate(
                    InputWorkpiece.Shape,
                    BasePlate,
                    new PlatePose(new Wgp_Pnt(5, 0, 0), DirectionY),
                    CurrentPlate.Clearances,
                    CurrentPlate.MinSupportLen,
                    CurrentPlate.CuttingDistance
                );
            }
            //更新stack中的元素
            CurrentPlate_StackPanel.Children.Clear();
            foreach (var item in CurrentPlate.Pieces)
            {
                CurrentPlate_StackPanel.Children.Add(MakeStackItem(item));
            }
            //显示新创建的板
            foreach (var item in CurrentPlate.Pieces)
            {
                Viewer.Display(item.AIS, false);
            }
            Viewer.Viewer.UpdateCurrentViewer();
        }
    }

    // 添加板
    private void addPlate_Button_Click(object sender, RoutedEventArgs e) { }

    private void SetXYNum_Button_Click(object sender, RoutedEventArgs e)
    {
        XNum_TextBox.Text = XNum.ToString();
        YNum_TextBox.Text = YNum.ToString();
        XNum_StackPanel.Visibility = Visibility.Visible;
        YNum_StackPanel.Visibility = Visibility.Visible;
        OffsetX_StackPanel.Visibility = Visibility.Collapsed;
        OffsetY_StackPanel.Visibility = Visibility.Collapsed;
        VPCMode = VPCMode.Num;
    }

    private void SetXYOffset_Button_Click(object sender, RoutedEventArgs e)
    {
        OffsetX_TextBox.Text = OffsetXParameter.ToString();
        OffsetY_TextBox.Text = OffsetYParameter.ToString();
        XNum_StackPanel.Visibility = Visibility.Collapsed;
        YNum_StackPanel.Visibility = Visibility.Collapsed;
        OffsetX_StackPanel.Visibility = Visibility.Visible;
        OffsetY_StackPanel.Visibility = Visibility.Visible;
        VPCMode = VPCMode.Offset;
    }

    // 创建竖板
    private void MakeVerticalPlate_Button_Click(object sender, RoutedEventArgs e)
    {
        #region 测试用代码，之后删除
        if (InputWorkpiece == null)
        {
            InputWorkpiece = new(new STEPExchange("mods\\mytest.STEP").Shape());
            Display(InputWorkpiece.AIS, false);
        }
        if (BasePlate == null)
        {
            MakeBasePlate();
        }
        #endregion

        #region 更新界面的逻辑
        //清空原来的板
        VerticalPlates.Clear();
        clearPlates();
        #endregion

        if (InputWorkpiece != null && BasePlate != null)
        {
            EraseAll(false);
            Display(InputWorkpiece.AIS, true);
            AISContext.SetTransparency(InputWorkpiece.AIS, 0.3, false);
            AISContext.SetColor(InputWorkpiece.AIS, new WColor(255, 255, 0), false);

            Display(BasePlate.AIS, true);
            AISContext.SetTransparency(BasePlate.AIS, 0.5, false);
            AISContext.SetColor(BasePlate.AIS, new WColor(204, 102, 51), false);

            MakeVerticalPlates();
            UpdateComboBox();
            foreach (var onePlate in VerticalPlates)
            {
                WColor TempColor = new();
                if (onePlate.Direction.Dir.IsParallel(DirectionX, 0.1))
                {
                    TempColor = new(255, 0, 0);
                }
                if (onePlate.Direction.Dir.IsParallel(DirectionY, 0.1))
                {
                    TempColor = new(0, 255, 0);
                }
                foreach (var onePiece in onePlate.Pieces)
                {
                    Display(onePiece.AIS, false);
                    AISContext.SetTransparency(onePiece.AIS, 0.3, false);
                    AISContext.SetColor(onePiece.AIS, TempColor, false);
                }
            }
        }
        Viewer.Update();
        Viewer.FitAll();
    }

    // 创建竖板
    void MakeVerticalPlates()
    {
        double OffextX = 0.0;
        double OffextY = 0.0;
        double tempX = BasePlate.X + InitialOffsetXParameter;
        double tempY = BasePlate.Y + InitialOffsetYParameter;
        switch (VPCMode)
        {
            case VPCMode.Num:
                OffextX = (BasePlate.DX - InitialOffsetXParameter * 2) / (float)(XNum - 2 + 1);
                OffextY = (BasePlate.DY - InitialOffsetYParameter * 2) / (float)(YNum - 2 + 1);
                break;
            case VPCMode.Offset:
                OffextX = OffsetXParameter;
                OffextY = OffsetYParameter;
                break;
        }

        //创建X方向竖板
        while (BasePlate.X < tempX && tempX < BasePlate.X + BasePlate.DX)
        {
            VerticalPlates.Add(
                SimpleClampMaker.MakeVerticalPlate(
                    InputWorkpiece.Shape,
                    BasePlate,
                    new PlatePose(new Wgp_Pnt(tempX, BasePlate.Y, 0.0), DirectionX),
                    ClearancesParameter,
                    MinSupportingLenParameter,
                    CuttingDistanceParameter
                )
            );
            tempX += OffextX;
        }

        //创建Y方向竖板
        while (BasePlate.Y < tempY && tempY < BasePlate.Y + BasePlate.DY)
        {
            VerticalPlates.Add(
                SimpleClampMaker.MakeVerticalPlate(
                    InputWorkpiece.Shape,
                    BasePlate,
                    new PlatePose(new Wgp_Pnt(BasePlate.X, tempY, 0.0), DirectionY),
                    ClearancesParameter,
                    MinSupportingLenParameter,
                    CuttingDistanceParameter
                )
            );
            tempY += OffextY;
        }

        //! test
        foreach (var onePlate in VerticalPlates)
        {
            var tempa = onePlate;
            var c = tempa.Clearances;
            var m = tempa.MinSupportLen;
            var cd = tempa.CuttingDistance;
            Debug.WriteLine($"Clearances: {c}, MinSupportLen: {m}, CuttingDistance: {cd}");
        }
    }

    // 创建底板
    private void MakeBasePlate_Button_Clcik(object sender, RoutedEventArgs e)
    {
        Viewer.EraseAll();
        if (InputWorkpiece == null)
        {
            InputWorkpiece = new(new STEPExchange("mods\\mytest.STEP").Shape());
        }
        Viewer.Display(InputWorkpiece.AIS, true);
        MakeBasePlate();
    }

    // 连接竖板
    private void ConnectPlate_Button_Click(object sender, RoutedEventArgs e)
    {
        //VerticalPlates = SimpleClampMaker.SuturePLates(
        //    VerticalPlates,
        //    BasePlate,
        //    ConnectionHeightParameter,
        //    ConnectionThicknessParameter,
        //    FilletRadiusParameter
        //);
        UpdateComboBox();
        //重新显示更新后的shape
        //if (AISInputWorkpiece != null && AISBasePlate != null)
        //{
        //    Viewer.EraseAll();
        //    Viewer.Display(AISInputWorkpiece, true);
        //    Viewer.Display(AISBasePlate, true);

        //    foreach (var onePlate in VerticalPlates)
        //    {
        //        Viewer.Display(onePlate.aisShape, true);
        //    }
        //}
        Viewer.FitAll();
    }

    private void testconnect()
    {
        //VerticalPlates = SimpleClampMaker.SuturePLates(
        //    VerticalPlates,
        //    BasePlate,
        //    ConnectionHeightParameter,
        //    ConnectionThicknessParameter,
        //    FilletRadiusParameter
        //);
    }

    private void MakeBasePlate()
    {
        BasePlate = SimpleClampMaker.MakeBasePlate_NoInteract(
            InputWorkpiece.Shape,
            BasePlateOffsetZ,
            BasePlateOffsetX,
            BasePlateOffsetY
        );
        Display(BasePlate.AIS, true);
        //根据计算结果，设置推荐值
        InitialOffsetXParameter = Math.Round(BasePlate.DX * 0.1);
        InitialOffsetX_TextBox.Text = InitialOffsetXParameter.ToString("F1");
        InitialOffsetYParameter = Math.Round(BasePlate.DY * 0.1);
        InitialOffsetY_TextBox.Text = InitialOffsetYParameter.ToString("F1");
    }

    private void UpdateComboBox()
    {
        clearPlates();
        foreach (var onePlate in VerticalPlates)
        {
            if (onePlate.Direction.Dir.IsParallel(DirectionX, 0.1))
            {
                CurrentPlateLocationX_ComboBox.Items.Add(onePlate);
            }
            if (onePlate.Direction.Dir.IsParallel(DirectionY, 0.1))
            {
                CurrentPlateLocationY_ComboBox.Items.Add(onePlate);
            }
        }
    }

    /// <summary>
    /// 删除控件中板的信息
    /// </summary>
    private void clearPlates()
    {
        CurrentPlate_StackPanel.Children.Clear();

        CurrentPlateDirection_ComboBox.SelectedIndex = -1;
        CurrentPlateLocationX_ComboBox.SelectedIndex = -1;
        CurrentPlateLocationY_ComboBox.SelectedIndex = -1;
        CurrentPlateLocationX_ComboBox.Visibility = Visibility.Collapsed;
        CurrentPlateLocationY_ComboBox.Visibility = Visibility.Collapsed;
        CurrentPlateLocationX_ComboBox.Items.Clear();
        CurrentPlateLocationY_ComboBox.Items.Clear();
        CurrentPlateSelectedLocation_TextBox.Text = null;
    }

    private void ChangeCurrentLocation(object sender, RoutedEventArgs e)
    {
        Button button = (Button)sender;
        VerticalPlate thePlate = (VerticalPlate)button.Tag;
        //todo 改变PlateDirection的点
        CurrentPlateSelectedLocation_TextBox.Text = thePlate.Direction.Point.X().ToString("F1");
    }

    private void PermutationPlate_Button_Click(object sender, RoutedEventArgs e)
    {
        //重新显示更新后的shape
        if (InputWorkpiece != null && BasePlate != null)
        {
            Viewer.EraseAll();
        }
        //CombinedFixtureBoard = SimpleClampMaker.DeployPlates(VerticalPlates, BasePlate);
        //Viewer.Display(new WAIS_Shape(CombinedFixtureBoard), true);
        Viewer.FitAll();
    }

    private void OutputSTEP_Button_Click(object sender, RoutedEventArgs e)
    {
        STEPExchange a = new(CombinedFixtureBoard);
        a.SaveFile("mods\\test_output.STEP");
    }

    private void NumberMark_Button_Click(object sender, RoutedEventArgs e)
    {
        for (int i = 0; i < VerticalPlates.Count; i++)
        {
            VerticalPlate thePlate = VerticalPlates[i];
            //SimpleClampMaker.BrandNumber(
            //    ref thePlate,
            //    thePlate.connectionHeight * 0.8,
            //    4
            ////new Wgp_Pnt(40.0, 40.0, -80.0)
            //);
            VerticalPlates[i] = thePlate; // 将修改后的对象重新赋值给集合中的元素
        }
        //重新显示更新后的shape
        if (InputWorkpiece != null && BasePlate != null)
        {
            EraseAll(false);
            Viewer.Display(InputWorkpiece.AIS, false);
            Viewer.Display(BasePlate.AIS, false);

            foreach (var onePlate in VerticalPlates)
            {
                Viewer.Display(onePlate.AIS, true);
            }
        }
        Viewer.FitAll();
        Viewer.Update();
    }

    private void Manipulator_Button_Click(object sender, RoutedEventArgs e)
    {
        _useManipulator = !_useManipulator;
        Viewer.CurrentActionMode = _useManipulator ? ActionMode.Manipulator : ActionMode.Normal;

        if (_useManipulator)
        {
            Viewer.Attach(InputWorkpiece.AIS);
        }
        else { }
    }
}
