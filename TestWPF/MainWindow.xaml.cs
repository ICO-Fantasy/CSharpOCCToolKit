using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Diagnostics;
using System.Linq;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Forms.Integration;
using System.Windows.Forms.VisualStyles;
using Microsoft.VisualBasic;
using Microsoft.Windows.Themes;
using OCCTK.Ex;
using OCCTK.IO;
using OCCTK.Laser;
using OCCTK.OCC.AIS;
using OCCTK.OCC.BRepPrimAPI;
using OCCTK.OCC.gp;
using OCCTK.OCC.TopoDS;
using OCCViewForm;
using static System.Net.Mime.MediaTypeNames;

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

public class CombinedFixtureBoard
{
    public CombinedFixtureBoard(WTopoDS_Shape theShape)
    {
        _InputShape = theShape;
        _AIS = new WAIS_Shape(theShape);
    }

    private WTopoDS_Shape _InputShape;
    private WAIS_Shape _AIS;
    public WTopoDS_Shape Shape
    {
        get { return _InputShape; }
    }
    public WAIS_Shape AIS
    {
        get { return _AIS; }
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

#region ViewModel

//public class BasePlateProperty : INotifyPropertyChanged
//{
//    private double _basePlateOffsetX = 5;
//    private double _basePlateOffsetY = 5;
//    private double _basePlateOffsetZ = 100.0;

//    public double BasePlateOffsetX
//    {
//        get { return _basePlateOffsetX; }
//        set
//        {
//            if (_basePlateOffsetX != value)
//            {
//                _basePlateOffsetX = value;
//                OnPropertyChanged(nameof(BasePlateOffsetX));
//            }
//        }
//    }

//    public double BasePlateOffsetY
//    {
//        get { return _basePlateOffsetY; }
//        set
//        {
//            if (_basePlateOffsetY != value)
//            {
//                _basePlateOffsetY = value;
//                OnPropertyChanged(nameof(BasePlateOffsetY));
//            }
//        }
//    }

//    public double BasePlateOffsetZ
//    {
//        get { return _basePlateOffsetZ; }
//        set
//        {
//            if (_basePlateOffsetZ != value)
//            {
//                _basePlateOffsetZ = value;
//                OnPropertyChanged(nameof(BasePlateOffsetZ));
//            }
//        }
//    }

//    public event PropertyChangedEventHandler PropertyChanged;

//    protected void OnPropertyChanged(string propertyName)
//    {
//        PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
//    }
//}

//public class VerticalPlateProperty : INotifyPropertyChanged
//{
//    private VPCMode _vpcMode = 0;
//    private int _xNum = 3;
//    private int _yNum = 3;
//    private double _initialOffsetXParameter = 5;
//    private double _offsetXParameter = 20;
//    private double _initialOffsetYParameter = 5;
//    private double _offsetYParameter = 20;
//    private double _connectionHeightParameter = 20;
//    private double _minSupportingLenParameter = 5;
//    private double _clearancesParameter = 4;
//    private double _cuttingDistanceParameter = 500;
//    private double _connectionThicknessParameter = 5.0;
//    private double _filletRadiusParameter = 2.0;

//    public VPCMode VPCMode
//    {
//        get { return _vpcMode; }
//        set
//        {
//            if (_vpcMode != value)
//            {
//                _vpcMode = value;
//                OnPropertyChanged(nameof(VPCMode));
//            }
//        }
//    }

//    public int XNum
//    {
//        get { return _xNum; }
//        set
//        {
//            if (_xNum != value)
//            {
//                _xNum = value;
//                OnPropertyChanged(nameof(XNum));
//            }
//        }
//    }

//    public int YNum
//    {
//        get { return _yNum; }
//        set
//        {
//            if (_yNum != value)
//            {
//                _yNum = value;
//                OnPropertyChanged(nameof(YNum));
//            }
//        }
//    }

//    public double InitialOffsetXParameter
//    {
//        get { return _initialOffsetXParameter; }
//        set
//        {
//            if (_initialOffsetXParameter != value)
//            {
//                _initialOffsetXParameter = value;
//                OnPropertyChanged(nameof(InitialOffsetXParameter));
//            }
//        }
//    }

//    public double OffsetXParameter
//    {
//        get { return _offsetXParameter; }
//        set
//        {
//            if (_offsetXParameter != value)
//            {
//                _offsetXParameter = value;
//                OnPropertyChanged(nameof(OffsetXParameter));
//            }
//        }
//    }

//    public double InitialOffsetYParameter
//    {
//        get { return _initialOffsetYParameter; }
//        set
//        {
//            if (_initialOffsetYParameter != value)
//            {
//                _initialOffsetYParameter = value;
//                OnPropertyChanged(nameof(InitialOffsetYParameter));
//            }
//        }
//    }

//    public double OffsetYParameter
//    {
//        get { return _offsetYParameter; }
//        set
//        {
//            if (_offsetYParameter != value)
//            {
//                _offsetYParameter = value;
//                OnPropertyChanged(nameof(OffsetYParameter));
//            }
//        }
//    }

//    public double ConnectionHeightParameter
//    {
//        get { return _connectionHeightParameter; }
//        set
//        {
//            if (_connectionHeightParameter != value)
//            {
//                _connectionHeightParameter = value;
//                OnPropertyChanged(nameof(ConnectionHeightParameter));
//            }
//        }
//    }

//    public double MinSupportingLenParameter
//    {
//        get { return _minSupportingLenParameter; }
//        set
//        {
//            if (_minSupportingLenParameter != value)
//            {
//                _minSupportingLenParameter = value;
//                OnPropertyChanged(nameof(MinSupportingLenParameter));
//            }
//        }
//    }

//    public double ClearancesParameter
//    {
//        get { return _clearancesParameter; }
//        set
//        {
//            if (_clearancesParameter != value)
//            {
//                _clearancesParameter = value;
//                OnPropertyChanged(nameof(ClearancesParameter));
//            }
//        }
//    }

//    public double CuttingDistanceParameter
//    {
//        get { return _cuttingDistanceParameter; }
//        set
//        {
//            if (_cuttingDistanceParameter != value)
//            {
//                _cuttingDistanceParameter = value;
//                OnPropertyChanged(nameof(CuttingDistanceParameter));
//            }
//        }
//    }

//    public double ConnectionThicknessParameter
//    {
//        get { return _connectionThicknessParameter; }
//        set
//        {
//            if (_connectionThicknessParameter != value)
//            {
//                _connectionThicknessParameter = value;
//                OnPropertyChanged(nameof(ConnectionThicknessParameter));
//            }
//        }
//    }

//    public double FilletRadiusParameter
//    {
//        get { return _filletRadiusParameter; }
//        set
//        {
//            if (_filletRadiusParameter != value)
//            {
//                _filletRadiusParameter = value;
//                OnPropertyChanged(nameof(FilletRadiusParameter));
//            }
//        }
//    }

//    public event PropertyChangedEventHandler PropertyChanged;

//    protected void OnPropertyChanged(string propertyName)
//    {
//        PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
//    }
//}

//public class MainViewModel : INotifyPropertyChanged
//{
//    private BasePlateProperty _basePlate;
//    private VerticalPlateProperty _verticalPlate;

//    public MainViewModel()
//    {
//        BasePlate = new BasePlateProperty();
//        VerticalPlate = new VerticalPlateProperty();
//    }

//    public BasePlateProperty BasePlate
//    {
//        get { return _basePlate; }
//        set
//        {
//            if (_basePlate != value)
//            {
//                _basePlate = value;
//                OnPropertyChanged(nameof(BasePlate));
//            }
//        }
//    }

//    public VerticalPlateProperty VerticalPlate
//    {
//        get { return _verticalPlate; }
//        set
//        {
//            if (_verticalPlate != value)
//            {
//                _verticalPlate = value;
//                OnPropertyChanged(nameof(VerticalPlate));
//            }
//        }
//    }

//    public event PropertyChangedEventHandler PropertyChanged;

//    protected void OnPropertyChanged(string propertyName)
//    {
//        PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
//    }
//}

#endregion

/// <summary>
/// 交互逻辑 MainWindow.xaml
/// </summary>
public partial class MainWindow : Window
{
    private OCCCanvas Viewer;
    private WAIS_InteractiveContext AISContext;

    private bool _useManipulator = false;

    /// <summary>
    /// 创建的竖板列表，分为两部分储存
    /// </summary>
    public List<VerticalPlate> MiddleToDownPlates = new List<VerticalPlate>(); // 从连接高的一半开始往下开槽
    public List<VerticalPlate> MiddleToUpPlates = new List<VerticalPlate>(); // 从连接高的一半开始往上开槽

    public List<VerticalPlate> VerticalPlates
    {
        get
        {
            List<VerticalPlate> Plates = new List<VerticalPlate>();
            Plates.AddRange(MiddleToDownPlates);
            Plates.AddRange(MiddleToUpPlates);
            return Plates;
        }
    }

    /// <summary>
    /// 展平后的结果
    /// </summary>
    public CombinedFixtureBoard CombinedFixtureBoard;

    /// <summary>
    /// 输入的工件
    /// </summary>
    private InputWorkpiece InputWorkpiece;

    /// <summary>
    /// 当前展示的AIS对象列表
    /// </summary>
    private List<WAIS_Shape> _CurrentAIS = new List<WAIS_Shape>();

    #region 创建横板参数
    /// <summary>
    /// 横板
    /// </summary>
    private BasePlate? BasePlate;

    public double BasePlateOffsetX { get; set; } = 5;

    public double BasePlateOffsetY { get; set; } = 5;

    public double BasePlateOffsetZ { get; set; } = 100.0;

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
    /// <summary>
    /// 避让高度
    /// </summary>
    public double AvoidanceHeightParameter { get; set; } = 20.0;

    //! 移除竖板连接高，用避让高度代替
    ///// <summary>
    ///// 竖板连接高
    ///// </summary>
    //public double ConnectionHeightParameter { get; set; } = 20;

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
    public double ConnectionThicknessParameter { get; set; } = 2.0;
    public double FilletRadiusParameter { get; set; } = 1.0;

    #endregion

    #region 单块板属性
    private VerticalPlate? _CurrentPlate = null;
    public VerticalPlate? CurrentPlate
    {
        get { return _CurrentPlate; }
        set
        {
            if (_CurrentPlate != value)
            {
                _CurrentPlate = value;
                CurrentPlateSelectedLocation_TextBox.Text =
                    $"{value.Pose.Point.X():F1}, {value.Pose.Point.Y():F1}, {value.Pose.Point.Z():F1}";
                CurrentPlateSelectedDirection_TextBox.Text =
                    $"{value.Pose.Dir.X():F1}, {value.Pose.Dir.Y():F1}, {value.Pose.Dir.Z():F1}";
                CurrentPlateConnectionThickness_TextBox.Text = $"{value.ConnectionThickness}";
                CurrentPlateNumberString_TextBox.Text = $"{value.NumberString}";
                CurrentPlateFilletRadius_TextBox.Text = $"{value.FilletRadius}";
                CurrentPlateAvoidanceHeight_TextBox.Text = $"{value.AvoidanceHeight}";
                CurrentPlateCuttingDistance_TextBox.Text = $"{value.CuttingDistance}";
                CurrentPlateMinSupportLen_TextBox.Text = $"{value.MinSupportLen}";
                CurrentPlateClearances_TextBox.Text = $"{value.Clearances}";
                if (value.Sutured)
                {
                    CurrentPlateSutured_Label.Content = $"已合并";
                    CurrentPlateSutured_Label.Background = System.Windows.Media.Brushes.LawnGreen;
                }
                else
                {
                    CurrentPlateSutured_Label.Content = $"未合并";
                    CurrentPlateSutured_Label.Background = System.Windows.Media.Brushes.Red;
                }
                ;
            }
        }
    }
    //
    #endregion
    //public MainViewModel ViewModel { get; set; }

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
        //< TextBox Text = "{Binding BasePlate.BasePlateOffsetX, UpdateSourceTrigger=PropertyChanged}"
        //ViewModel = new MainViewModel();
        //this.DataContext = ViewModel;
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
        ConnectionHeight_TextBox.Text = AvoidanceHeightParameter.ToString();
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
            AvoidanceHeightParameter = newValue;
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
            // 更新 CuttingDistanceParameter 的值
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
            // 更新 XNum 的值
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
            // 更新 YNum 的值
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
            // 更新 ConnectionThickness 的值
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
            // 更新 FilletRadius 的值
            FilletRadiusParameter = newValue;
        }
    }

    /// <summary>
    /// 选择查看的方向
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void CurrentPlateDirection_ComboBox_SelectionChanged(
        object sender,
        SelectionChangedEventArgs e
    )
    {
        //! 方向虽然用X、Y做区分，实际上还包括XY以外的方向，但只分为两种。
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

    /// <summary>
    /// 选中 X 方向下拉框元素
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
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
            //清空画布并重新显示
            EraseAll(false);
            DisplayInputWorkpiece(false);
            DisplayBasePlates(false);
            DisplaySinglePlate(CurrentPlate, false);
            Viewer.Update();
            for (global::System.Int32 i = 0; i < CurrentPlate.Pieces.Count; ++i)
            {
                CurrentPlate_StackPanel.Children.Add(MakeStackItem(CurrentPlate.Pieces[i], i));
            }
        }
    }

    /// <summary>
    /// 选中 Y 方向下拉框元素
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
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
            //清空画布并重新显示
            EraseAll(false);
            DisplayInputWorkpiece(false);
            DisplayBasePlates(false);
            DisplaySinglePlate(CurrentPlate, false);
            Viewer.Update();
            for (global::System.Int32 i = 0; i < CurrentPlate.Pieces.Count; ++i)
            {
                CurrentPlate_StackPanel.Children.Add(MakeStackItem(CurrentPlate.Pieces[i], i));
            }
        }
    }

    //单板
    private Grid MakeStackItem(VerticalPiece thePiece, int theNum)
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
        PieceLabel.Content = theNum.ToString();
        PieceLabel.HorizontalContentAlignment = HorizontalAlignment.Left; // 设置 Label 的水平内容对齐方式为 Left

        // 将子元素添加到 Grid 中，并指定列索引
        Grid.SetColumn(PieceLabel, 0);

        // 将子元素添加到 StackPanel 的 Children 集合中
        outerStackPanel.Children.Add(PieceLabel);

        return outerStackPanel;
    }

    #endregion

    #region 按钮

    //todo 更新当前板坐标
    private void UpdateCurrentPlateLocation_Button_Click(object sender, RoutedEventArgs e)
    {
        //if (CurrentPlate != null)
        //{
        //    // 更新CurrentPlateValueLable的值
        //    if (double.TryParse(CurrentPlateAddLocation_TextBox.Text, out newValue))
        //    {
        //        CurrentPlate = SimpleClampMaker.MakeVerticalPlate(
        //            InputWorkpiece.Shape,
        //            BasePlate,
        //            new PlatePose(new Wgp_Pnt(5, 0, 0), DirectionY),
        //            CurrentPlate.Clearances,
        //            CurrentPlate.MinSupportLen,
        //            CurrentPlate.CuttingDistance
        //        );
        //    }
        //    //更新stack中的元素
        //    CurrentPlate_StackPanel.Children.Clear();
        //    foreach (var item in CurrentPlate.Pieces)
        //    {
        //        CurrentPlate_StackPanel.Children.Add(MakeStackItem(item));
        //    }

        //    Viewer.Update();
        //}
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

    /// <summary>
    /// 创建底板按钮
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void MakeBasePlate_Button_Clcik(object sender, RoutedEventArgs e)
    {
        #region 测试用代码
        if (InputWorkpiece == null)
        {
            TestInput();
        }
        #endregion

        EraseAll(false);
        DisplayInputWorkpiece(true);
        MakeBasePlate();
        DisplayBasePlates(true);
        Viewer.Update();
    }

    /// <summary>
    /// 创建竖板按钮
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void MakeVerticalPlate_Button_Click(object sender, RoutedEventArgs e)
    {
        #region 测试用代码
        if (InputWorkpiece == null)
        {
            TestInput();
        }
        if (BasePlate == null)
        {
            MakeBasePlate();
        }
        #endregion

        // 清空原来的板
        MiddleToDownPlates.Clear();
        MiddleToUpPlates.Clear();

        if (InputWorkpiece != null && BasePlate != null)
        {
            EraseAll(false);
            DisplayInputWorkpiece(false);
            DisplayBasePlates(false);

            MakeVerticalPlates();
            UpdateComboBox();
            DisplayVerticalPlatesPieces(false);
        }
        Viewer.Update();
        Viewer.FitAll();
    }

    /// <summary>
    /// 连接竖板按钮
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void ConnectPlate_Button_Click(object sender, RoutedEventArgs e)
    {
        #region 测试用代码
        //todo DEBUG
        if (InputWorkpiece == null)
        {
            TestInput();
        }
        if (BasePlate == null)
        {
            MakeBasePlate();
        }
        if (VerticalPlates.Count == 0)
        {
            MakeVerticalPlates();
        }
        #endregion

        ConnectPlates();
        UpdateComboBox();

        EraseAll(false);
        DisplayInputWorkpiece(false);
        DisplayBasePlates(false);
        DisplayVerticalPlates(false);

        Viewer.Update();
        Viewer.FitAll();
    }

    /// <summary>
    /// 展平排列板按钮
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void PermutationPlate_Button_Click(object sender, RoutedEventArgs e)
    {
        EraseAll(false);
        CombinedDisplayBoard();
        Viewer.Update();
        Viewer.FitAll();
    }

    private void OutputSTEP_Button_Click(object sender, RoutedEventArgs e)
    {
        OutputSTEP();
    }

    //todo
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

    //todo
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
    #endregion

    /// <summary>
    /// 创建底板逻辑
    /// </summary>
    private void MakeBasePlate()
    {
        BasePlate = SimpleClampMaker.MakeBasePlate_NoInteract(
            InputWorkpiece.Shape,
            BasePlateOffsetZ,
            BasePlateOffsetX,
            BasePlateOffsetY
        );
        // 根据计算结果，设置推荐值
        InitialOffsetXParameter = Math.Round(BasePlate.DX * 0.1);
        InitialOffsetX_TextBox.Text = InitialOffsetXParameter.ToString("F1");
        InitialOffsetYParameter = Math.Round(BasePlate.DY * 0.1);
        InitialOffsetY_TextBox.Text = InitialOffsetYParameter.ToString("F1");
    }

    /// <summary>
    /// 创建竖板逻辑
    /// </summary>
    void MakeVerticalPlates()
    {
        double OffextX = 0.0;
        double OffextY = 0.0;
        double InitX = BasePlate.X + InitialOffsetXParameter;
        double tempX = BasePlate.X + InitialOffsetXParameter;
        double InitY = BasePlate.Y + InitialOffsetYParameter;
        double tempY = BasePlate.Y + InitialOffsetYParameter;
        switch (VPCMode)
        {
            case VPCMode.Num:
                // 计算偏移值
                OffextX = (BasePlate.DX - InitialOffsetXParameter * 2) / (float)(XNum - 2 + 1);
                OffextY = (BasePlate.DY - InitialOffsetYParameter * 2) / (float)(YNum - 2 + 1);
                break;
            case VPCMode.Offset:
                // 直接使用设定的偏移值
                OffextX = OffsetXParameter;
                OffextY = OffsetYParameter;
                break;
        }

        // 创建X方向竖板
        int num = 0;
        while (BasePlate.X < tempX && tempX < BasePlate.X + BasePlate.DX)
        {
            var tempPlate = SimpleClampMaker.MakeVerticalPlate(
                InputWorkpiece.Shape,
                BasePlate,
                new PlatePose(new Wgp_Pnt(tempX, BasePlate.Y, 0.0), DirectionX),
                ClearancesParameter,
                MinSupportingLenParameter,
                CuttingDistanceParameter
            );
            tempPlate.NumberString = $"X{num}";
            MiddleToDownPlates.Add(tempPlate);
            num += 1;
            tempX += OffextX;
            //! test
            return;
        }
        // 创建Y方向竖板
        num = 0;
        while (BasePlate.Y < tempY && tempY < BasePlate.Y + BasePlate.DY)
        {
            var tempPlate = SimpleClampMaker.MakeVerticalPlate(
                InputWorkpiece.Shape,
                BasePlate,
                new PlatePose(new Wgp_Pnt(BasePlate.X, tempY, 0.0), DirectionY),
                ClearancesParameter,
                MinSupportingLenParameter,
                CuttingDistanceParameter
            );
            tempPlate.NumberString = $"Y{num}";
            MiddleToUpPlates.Add(tempPlate);
            num += 1;
            tempY += OffextY;
        }

        //#region 多线程创建

        ////创建X方向竖板
        //// 使用并发集合以便在线程间安全地添加元素
        //ConcurrentBag<VerticalPlate> concurrentDownPlates = new ConcurrentBag<VerticalPlate>();
        //    Parallel.For(
        //        0,
        //        (int)Math.Ceiling((BasePlate.DX - InitX) / OffextX),
        //        i =>
        //        {
        //            double currentX = InitX + i * OffextX;

        //            if (BasePlate.X < currentX && currentX < BasePlate.X + BasePlate.DX)
        //            {
        //                var tempPlate = SimpleClampMaker.MakeVerticalPlate(
        //                    InputWorkpiece.Shape,
        //                    BasePlate,
        //                    new PlatePose(new Wgp_Pnt(currentX, BasePlate.Y, 0.0), DirectionX),
        //                    ClearancesParameter,
        //                    MinSupportingLenParameter,
        //                    CuttingDistanceParameter
        //                );
        //                tempPlate.NumberString = $"X{i}";
        //                concurrentDownPlates.Add(tempPlate);
        //            }
        //        }
        //    );
        //    // 将并发集合中的元素添加到最终的集合中
        //    MiddleToDownPlates.AddRange(concurrentDownPlates);

        //    //创建Y方向竖板
        //    // 使用并发集合以便在线程间安全地添加元素
        //    ConcurrentBag<VerticalPlate> concurrentUpPlates = new ConcurrentBag<VerticalPlate>();
        //    Parallel.For(
        //        0,
        //        (int)Math.Ceiling((BasePlate.DY - InitY) / OffextY),
        //        i =>
        //        {
        //            double currentY = InitY + i * OffextY;

        //            if (BasePlate.Y < currentY && currentY < BasePlate.Y + BasePlate.DY)
        //            {
        //                var tempPlate = SimpleClampMaker.MakeVerticalPlate(
        //                    InputWorkpiece.Shape,
        //                    BasePlate,
        //                    new PlatePose(new Wgp_Pnt(BasePlate.X, currentY, 0.0), DirectionY),
        //                    ClearancesParameter,
        //                    MinSupportingLenParameter,
        //                    CuttingDistanceParameter
        //                );
        //                tempPlate.NumberString = $"Y{i}";
        //                concurrentUpPlates.Add(tempPlate);
        //            }
        //        }
        //    );
        //    // 将并发集合中的元素添加到最终的集合中
        //    MiddleToUpPlates.AddRange(concurrentUpPlates);

        //#endregion
    }

    /// <summary>
    /// 连接竖板逻辑
    /// </summary>
    void ConnectPlates()
    {
        //连接板
        var temp = SimpleClampMaker.ConnectVerticalPLates(
            MiddleToDownPlates,
            MiddleToUpPlates,
            BasePlate,
            AvoidanceHeightParameter,
            ConnectionThicknessParameter,
            FilletRadiusParameter
        );
        MiddleToDownPlates = temp[0];
        MiddleToUpPlates = temp[1];

        // 给板打标签
        List<VerticalPlate> tempDown = new List<VerticalPlate>();
        foreach (var plate in MiddleToDownPlates)
        {
            tempDown.Add(SimpleClampMaker.BrandNumber(plate, 10.0));
        }
        MiddleToDownPlates = tempDown;

        List<VerticalPlate> tempUp = new List<VerticalPlate>();
        foreach (var plate in MiddleToUpPlates)
        {
            tempUp.Add(SimpleClampMaker.BrandNumber(plate, 10.0));
        }
        MiddleToUpPlates = tempUp;

        // 给底板开槽
        BasePlate = SimpleClampMaker.SlotBasePLates(
            BasePlate,
            MiddleToDownPlates,
            MiddleToUpPlates
        );
    }

    /// <summary>
    /// 展平竖板，并显示
    /// </summary>
    void CombinedDisplayBoard()
    {
        CombinedFixtureBoard = new CombinedFixtureBoard(
            SimpleClampMaker.DeployPlates(BasePlate, MiddleToDownPlates, MiddleToUpPlates)
        );
        Display(CombinedFixtureBoard.AIS, false);
    }

    /// <summary>
    /// 导出创建后的形状到文件中
    /// </summary>
    void OutputSTEP()
    {
        STEPExchange exchanger = new(CombinedFixtureBoard.Shape);
        exchanger.SaveFile("mods\\test_output.STEP");
    }

    /// <summary>
    /// 更新控件
    /// </summary>
    private void UpdateComboBox()
    {
        ClearPlatesInfo();
        foreach (var onePlate in MiddleToDownPlates)
        {
            CurrentPlateLocationX_ComboBox.Items.Add(onePlate);
        }
        foreach (var onePlate in MiddleToUpPlates)
        {
            CurrentPlateLocationY_ComboBox.Items.Add(onePlate);
        }
    }

    #region 视图显示

    /// <summary>
    /// 显示AIS，并加入当前AIS列表
    /// </summary>
    /// <param name="theAIS"></param>
    /// <param name="update"></param>
    private void Display(WAIS_Shape theAIS, bool update)
    {
        _CurrentAIS.Add(theAIS);
        AISContext.Display(theAIS, update);
    }

    /// <summary>
    /// 移除AIS列表中的所有AIS对象
    /// </summary>
    /// <param name="update"></param>
    private void EraseAll(bool update)
    {
        var aisToRemove = _CurrentAIS.ToList();
        foreach (var theAIS in aisToRemove)
        {
            _CurrentAIS.Remove(theAIS);
            AISContext.Erase(theAIS, update);
        }
    }

    /// <summary>
    /// 显示工件
    /// </summary>
    /// <param name="update"></param>
    private void DisplayInputWorkpiece(bool update)
    {
        if (InputWorkpiece != null)
        {
            Display(InputWorkpiece.AIS, update);
            AISContext.SetTransparency(InputWorkpiece.AIS, 0.3, false);
            AISContext.SetColor(InputWorkpiece.AIS, new WColor(255, 255, 0), false);
        }
    }

    /// <summary>
    /// 显示底板
    /// </summary>
    /// <param name="update"></param>
    private void DisplayBasePlates(bool update)
    {
        if (BasePlate != null)
        {
            Display(BasePlate.AIS, update);
            AISContext.SetTransparency(BasePlate.AIS, 0.5, false);
            AISContext.SetColor(BasePlate.AIS, new WColor(204, 102, 51), false);
        }
    }

    /// <summary>
    /// 显示竖板（分片）
    /// </summary>
    private void DisplayVerticalPlatesPieces(bool update)
    {
        foreach (var onePlate in MiddleToDownPlates)
        {
            foreach (var onePiece in onePlate.Pieces)
            {
                Display(onePiece.AIS, false);
                AISContext.SetTransparency(onePiece.AIS, 0.3, false);
                AISContext.SetColor(onePiece.AIS, new WColor(255, 0, 0), update);
            }
        }
        foreach (var onePlate in MiddleToUpPlates)
        {
            foreach (var onePiece in onePlate.Pieces)
            {
                Display(onePiece.AIS, false);
                AISContext.SetTransparency(onePiece.AIS, 0.3, false);
                AISContext.SetColor(onePiece.AIS, new(0, 255, 0), update);
            }
        }
    }

    /// <summary>
    /// 显示单块板的片
    /// </summary>
    /// <param name="thePlate"></param>
    /// <param name="update"></param>
    private void DisplaySinglePlate(VerticalPlate thePlate, bool update)
    {
        WColor theColor = new(125, 125, 125);
        if (MiddleToDownPlates.Contains(thePlate))
        {
            theColor = new(255, 0, 0);
        }
        else if (MiddleToUpPlates.Contains(thePlate))
        {
            theColor = new(0, 255, 0);
        }

        if (thePlate.Sutured)
        {
            Display(thePlate.AIS, false);
            AISContext.SetTransparency(thePlate.AIS, 0.3, false);
            AISContext.SetColor(thePlate.AIS, theColor, update);
        }
        else
        {
            foreach (var onePiece in thePlate.Pieces)
            {
                Display(onePiece.AIS, false);
                AISContext.SetTransparency(onePiece.AIS, 0.3, false);
                AISContext.SetColor(onePiece.AIS, theColor, update);
            }
        }
    }

    /// <summary>
    /// 显示竖板（整体）
    /// </summary>
    /// <param name="update"></param>
    private void DisplayVerticalPlates(bool update)
    {
        foreach (var onePlate in MiddleToDownPlates)
        {
            if (onePlate.Sutured)
            {
                Display(onePlate.AIS, false);
                AISContext.SetTransparency(onePlate.AIS, 0.3, false);
                AISContext.SetColor(onePlate.AIS, new WColor(255, 0, 0), update);
            }
        }
        foreach (var onePlate in MiddleToUpPlates)
        {
            if (onePlate.Sutured)
            {
                Display(onePlate.AIS, false);
                AISContext.SetTransparency(onePlate.AIS, 0.3, false);
                AISContext.SetColor(onePlate.AIS, new(0, 255, 0), update);
            }
        }
    }

    #endregion

    /// <summary>
    /// 删除控件中板的信息
    /// </summary>
    private void ClearPlatesInfo()
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

    //todo 改变位置
    private void ChangeCurrentLocation(object sender, RoutedEventArgs e)
    {
        Button button = (Button)sender;
        VerticalPlate thePlate = (VerticalPlate)button.Tag;
        //todo 改变PlateDirection的点
        CurrentPlateSelectedLocation_TextBox.Text = thePlate.NumberString;
    }

    private void TestInput()
    {
        InputWorkpiece = new(new STEPExchange("mods\\test1Small01.stp").Shape());
    }
}
