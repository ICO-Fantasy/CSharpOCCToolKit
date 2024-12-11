using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Forms.Integration;
using System.Windows.Input;
using System.Windows.Media.TextFormatting;
using log4net;
using Microsoft.Win32;
using OCCTK.Extension;
using OCCTK.IO;
using OCCTK.Laser;
using OCCTK.OCC.AIS;
using OCCTK.OCC.gp;
using OCCTK.OCC.Topo;
using OCCTK.Tool;
using OCCTK.Utils;
using OCCViewForm;
using Windows.Media.Core;
//设置别名
using Brushes = System.Windows.Media.Brushes;
using Color = OCCTK.Extension.Color;
using SelectionMode = OCCTK.OCC.AIS.SelectionMode;
using ViewOrientation = OCCTK.OCC.V3d.ViewOrientation;

namespace TestWPF;

public class InputWorkpiece
{
    public InputWorkpiece(TShape theShape)
    {
        _InputWorkpiece = theShape;

        _AISInputWorkpiece = new AShape(theShape);
    }

    private TShape _InputWorkpiece;
    private AShape _AISInputWorkpiece;
    public TShape Shape
    {
        get { return _InputWorkpiece; }
    }
    public InteractiveObject AIS
    {
        get { return _AISInputWorkpiece; }
    }

    public void Transform()
    {
        var trans = new OCCTK.OCC.BRepBuilderAPI.Transform(
            _InputWorkpiece,
            _AISInputWorkpiece.LocalTransformation()
        );
        _InputWorkpiece = trans.Shape();
        _AISInputWorkpiece.RemoveSelf(false);
        _AISInputWorkpiece = new AShape(_InputWorkpiece);
    }
}

public class CombinedFixtureBoard
{
    public CombinedFixtureBoard(TShape theShape)
    {
        _InputShape = theShape;
        _AIS = new AShape(theShape);
    }

    private TShape _InputShape;
    private AShape _AIS;
    public TShape Shape
    {
        get { return _InputShape; }
    }
    public AShape AIS
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

/// <summary>
/// 交互逻辑 SimpleClamp.xaml
/// </summary>
public partial class SimpleClamp : Window
{
    private static readonly ILog log = LogManager.GetLogger(typeof(App));

    #region TEST

    private List<TShape> inputShapes = new();

    #endregion
    public SimpleClamp()
    {
        InitializeComponent();
        // 创建 Windows Forms 控件和 WindowsFormsHost
        WindowsFormsHost aHost = new WindowsFormsHost();
        // 创建 Windows Forms 控件和 WindowsFormsHost
        //CanvasHost.Child = ((App)Application.Current).canva;
        if (App.Current.ContextManager.MainContext == null)
        {
            var context = App.Current.ContextManager.CreateContext();
        }
        Canvas = new OCCCanvas(App.Current.ContextManager.MainContext);
        aHost.Child = Canvas;
        canvas_grid.Children.Add(aHost);
        Canvas.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
        Canvas.Show();
        //< TextBox Text = "{Binding BasePlate.BasePlateOffsetX, UpdateSourceTrigger=PropertyChanged}"
        //ViewModel = new MainViewModel();
        //this.DataContext = ViewModel;
        Canvas.OnAISSelectionEvent += OnSelectPiece;
        Canvas.OnMouseMovedEvent += MousePosition;
        Canvas.OnKeyDownEvent += KeyDown;
        Canvas.OnKeyUpEvent += KeyUp;

        TextChangeSetting();
        //! test
        //TestInput();
        OCCFunctionTest();
        DisplayEraseInputWorkpiece(true);
    }

    const double LINEAR_TOL = 1e-1;

    // 定义颜色数组
    private readonly System.Windows.Media.SolidColorBrush[] colorSequence =
    {
        Brushes.LightPink,
        Brushes.LightCoral,
        Brushes.LightYellow,
        Brushes.LightGreen,
        Brushes.LightCyan
    };

    private readonly OCCCanvas Canvas;
    private ThreeDimensionContext ThreeDContext => App.Current.ContextManager.MainContext;
    private InteractiveContext AISContext => App.Current.ContextManager.MainContext?.AISContext;

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
    /// 当前展示的AIS对象列表
    /// </summary>
    private List<AShape> _CurrentAIS = new List<AShape>();

    /// <summary>
    /// 展平后的结果
    /// </summary>
    public CombinedFixtureBoard? CombinedFixtureBoard;

    /// <summary>
    /// 输入的工件
    /// </summary>
    private InputWorkpiece? InputWorkpiece;
    private bool ShowInputWorkpiece = true;

    #region 底板参数

    /// <summary>
    /// 底板
    /// </summary>
    private BasePlate? BasePlate;
    private bool ShowBasePlate = true;

    public double BasePlateOffsetX { get; set; } = 5;

    public double BasePlateOffsetY { get; set; } = 5;

    public double BasePlateOffsetZ { get; set; } = 100.0;

    #endregion

    #region 创建竖板参数

    //private static readonly Dir DirectionX = new(1.0, 0.0, 0.0);
    //private static readonly Dir DirectionY = new(0.0, 1.0, 0.0);
    private static readonly Dir DirectionX = DirMaker.AngleWithZ(0);
    private static readonly Dir DirectionY = DirMaker.AngleWithZ(90);

    /// <summary>
    /// 创建竖板所用模式
    /// </summary>
    public VPCMode VPCMode { get; set; } = 0;

    public int XNum { get; set; } = 4;

    public int YNum { get; set; } = 5;

    //X方向
    /// <summary>
    /// 竖板横向初始偏移
    /// </summary>
    public double InitialOffsetXParameter { get; set; } = 30.0;

    /// <summary>
    /// 竖板横向偏移
    /// </summary>
    public double OffsetXParameter { get; set; } = 20.0;

    //Y方向
    /// <summary>
    /// 竖板纵向初始偏移
    /// </summary>
    public double InitialOffsetYParameter { get; set; } = 40.0;

    /// <summary>
    /// 竖板纵向偏移
    /// </summary>
    public double OffsetYParameter { get; set; } = 20;

    private string AddPlateDirection = "";

    #endregion

    #region 竖板全局参数

    /// <summary>
    /// 避让高度
    /// </summary>
    public double AvoidanceHeightParameter { get; set; } = 20.0;

    /// <summary>
    /// 竖板最小支撑长度
    /// </summary>
    public double MinSupportingLenParameter { get; set; } = 0;

    //XY方向
    /// <summary>
    /// 竖板避让间隙
    /// </summary>
    public double ClearancesParameter { get; set; } = 0;

    /// <summary>
    /// 竖板切断距离
    /// </summary>
    public double CuttingDistanceParameter { get; set; } = 999;

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
            if (value == null)
            {
                return;
            }
            if (_CurrentPlate != value)
            {
                _CurrentPlate = value;
                CurrentPlateLocationX_TextBox.Text = $"{value.Pose.Location.X:F1}";
                CurrentPlateLocationY_TextBox.Text = $"{value.Pose.Location.Y:F1}";
                CurrentPlateDirection_TextBox.Text =
                    $"{DirMaker.GetDirAngleWithZ(value.Pose.Direction):F1}";
                CurrentPlateAuxiliaryHight_TextBox.Text = $"{value.AuxiliaryHight}";
                CurrentPlateAuxiliaryWidth_TextBox.Text = $"{value.AuxiliaryWidth}";
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
                    CurrentPlateSutured_Label.Background = Brushes.LawnGreen;
                }
                else
                {
                    CurrentPlateSutured_Label.Content = $"未合并";
                    CurrentPlateSutured_Label.Background = Brushes.Red;
                }
                ;
            }
        }
    }
    //
    #endregion

    /// <summary>
    /// 全局属性绑定
    /// </summary>
    private void TextChangeSetting()
    {
        BasePlateOffsetX_TextBox.Text = BasePlateOffsetX.ToString("F1");
        BasePlateOffsetX_TextBox.TextChanged += BasePlateOffsetX_TextChanged;

        BasePlateOffsetY_TextBox.Text = BasePlateOffsetY.ToString("F1");
        BasePlateOffsetY_TextBox.TextChanged += BasePlateOffsetY_TextChanged;

        BasePlateHight_TextBox.Text = BasePlateOffsetZ.ToString("F1");
        BasePlateOffsetY_TextBox.TextChanged += BasePlateOffsetZ_TextChanged;

        BasePlateXWidth_TextBox.TextChanged += BasePlateXWidth_TextChanged;
        BasePlateYWidth_TextBox.TextChanged += BasePlateYWidth_TextChanged;

        InitialOffsetX_TextBox.Text = InitialOffsetXParameter.ToString("F1");
        InitialOffsetX_TextBox.TextChanged += InitialOffsetX_TextChanged;

        // 设置OffsetXLabel的内容和TextBox的初始值，并添加TextChanged事件处理程序
        OffsetX_TextBox.Text = OffsetXParameter.ToString("F1");
        OffsetX_TextBox.TextChanged += OffsetX_TextChanged;

        // 设置InitialOffsetYLabel的内容和TextBox的初始值，并添加TextChanged事件处理程序
        InitialOffsetY_TextBox.Text = InitialOffsetYParameter.ToString("F1");
        InitialOffsetY_TextBox.TextChanged += InitialOffsetY_TextChanged;

        // 设置OffsetYLabel的内容和TextBox的初始值，并添加TextChanged事件处理程序
        OffsetY_TextBox.Text = OffsetYParameter.ToString("F1");
        OffsetY_TextBox.TextChanged += OffsetY_TextChanged;

        // 设置ConnectionHeightLabel的内容和TextBox的初始值，并添加TextChanged事件处理程序
        ConnectionHeight_TextBox.Text = AvoidanceHeightParameter.ToString("F1");
        ConnectionHeight_TextBox.TextChanged += ConnectionHeight_TextChanged;

        // 设置MinSupportingLenLabel的内容和TextBox的初始值，并添加TextChanged事件处理程序
        MinSupportingLen_TextBox.Text = MinSupportingLenParameter.ToString("F1");
        MinSupportingLen_TextBox.TextChanged += MinSupportingLen_TextChanged;

        // 设置ClearancesLabel的内容和TextBox的初始值，并添加TextChanged事件处理程序
        Clearances_TextBox.Text = ClearancesParameter.ToString("F1");
        Clearances_TextBox.TextChanged += Clearances_TextChanged;

        // 设置CuttingDistanceLabel的内容和TextBox的初始值，并添加TextChanged事件处理程序
        CuttingDistance_TextBox.Text = CuttingDistanceParameter.ToString("F1");
        CuttingDistance_TextBox.TextChanged += CuttingDistance_TextChanged;

        ConnectionThickness_TextBox.Text = ConnectionThicknessParameter.ToString("F1");
        ConnectionThickness_TextBox.TextChanged += ConnectionThickness_TextChanged;

        FilletRadius_TextBox.Text = FilletRadiusParameter.ToString("F1");
        FilletRadius_TextBox.TextChanged += FilletRadius_TextChanged;

        #region 创建竖板
        XNum_TextBox.Text = XNum.ToString();
        XNum_TextBox.TextChanged += XNum_TextChanged;
        YNum_TextBox.Text = YNum.ToString();
        YNum_TextBox.TextChanged += YNum_TextChanged;
        #endregion
    }

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

    private void Wireframe_Button_Click(object sender, RoutedEventArgs e)
    {
        //OCCCanvas.SetDisplayMode(DisplayMode.Wireframe);
    }

    private void Shading_Button_Click(object sender, RoutedEventArgs e)
    {
        //OCCCanvas.SetDisplayMode(DisplayMode.Shading);
    }
    #endregion

    #region 导入
    private void InputBrep_Button_Click(object sender, RoutedEventArgs e)
    {
        // 创建文件选择对话框
        OpenFileDialog openFileDialog = new OpenFileDialog
        {
            Filter = "Brep Files (*.brep)|*.brep|All Files (*.*)|*.*", // 设置文件过滤器
            InitialDirectory = System.IO.Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "mods") // 设置初始目录为指定的路径
        };

        // 如果用户选择了文件并点击了“打开”按钮
        if (openFileDialog.ShowDialog() == true)
        {
            string selectedFilePath = openFileDialog.FileName; // 获取选择的文件路径

            EraseAll(false);
            InputWorkpiece = new(new BrepExchange(selectedFilePath).Shape()); // 使用选择的文件路径
            BasePlate = null;
            MiddleToDownPlates.Clear();
            MiddleToUpPlates.Clear();
            UpdateComboBox();
            DisplayEraseInputWorkpiece(true);
            Canvas.FitAll();
        }
    }

    private void InputSTEP_Button_Click(object sender, RoutedEventArgs e)
    {
        // 创建文件选择对话框
        OpenFileDialog openFileDialog = new OpenFileDialog
        {
            Filter = "STEP Files (*.STEP;*.step;*.stp)|*.STEP;*.step;*.stp|All Files (*.*)|*.*", // 设置文件过滤器
            InitialDirectory = System.IO.Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "mods") // 设置初始目录为指定的路径
        };

        // 如果用户选择了文件并点击了“打开”按钮
        if (openFileDialog.ShowDialog() == true)
        {
            string selectedFilePath = openFileDialog.FileName; // 获取选择的文件路径

            EraseAll(false);
            InputWorkpiece = new(new STEPExchange(selectedFilePath).Shape().TopoShape); // 使用选择的文件路径
            BasePlate = null;
            MiddleToDownPlates.Clear();
            MiddleToUpPlates.Clear();
            UpdateComboBox();
            DisplayEraseInputWorkpiece(true);
            Canvas.FitAll();
        }
    }

    private void SaveBrep_Button_Click(object sender, RoutedEventArgs e)
    {
        // 创建保存文件对话框
        SaveFileDialog saveFileDialog = new SaveFileDialog
        {
            Filter = "Brep Files (*.brep)|*.brep|All Files (*.*)|*.*", // 设置默认的文件类型过滤器
            InitialDirectory = System.IO.Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "mods") // 设置初始目录为指定的路径
        };

        // 显示保存文件对话框并检查用户是否选择了保存路径
        if (saveFileDialog.ShowDialog() == true)
        {
            // 获取用户选择的文件路径
            string filePath = saveFileDialog.FileName;
            string tempFilePath = System.IO.Path.Combine(System.IO.Path.GetTempPath(), "temp.brep");

            // 创建 BrepExchange 对象并保存文件
            BrepExchange BE = new BrepExchange(InputWorkpiece.Shape);
            BE.SaveFile(tempFilePath);
            try
            {
                // 剪切文件
                System.IO.File.Move(tempFilePath, filePath);

                Debug.WriteLine($"文件已从 {tempFilePath} 剪切并重命名为 {filePath}");
            }
            catch (Exception ex)
            {
                System.IO.File.Delete(tempFilePath);
                Debug.WriteLine($"文件剪切失败: {ex.Message}");
            }
        }
    }

    private void Test_input_test_1_Click(object sender, RoutedEventArgs e)
    {
        EraseAll(false);
        InputWorkpiece = new(new STEPExchange("mods\\test1.stp").Shape().TopoShape);
        BasePlate = null;
        Display(InputWorkpiece.AIS, true);
        FitAll();
    }

    private void Test_input_test_2_Click(object sender, RoutedEventArgs e)
    {
        // 创建文件选择对话框
        OpenFileDialog openFileDialog = new OpenFileDialog
        {
            Filter = "Brep Files (*.brep)|*.brep|All Files (*.*)|*.*", // 设置文件过滤器
            InitialDirectory = System.IO.Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "mods") // 设置初始目录为指定的路径
        };

        // 如果用户选择了文件并点击了“打开”按钮
        if (openFileDialog.ShowDialog() == true)
        {
            string selectedFilePath = openFileDialog.FileName; // 获取选择的文件路径
            inputShapes.Add(new BrepExchange(selectedFilePath).Shape()); // 使用选择的文件路径
            CompoundNum_Label.Content = $"CompoundNum: {inputShapes.Count}";
        }
    }

    private void Test_input_test_3_Click(object sender, RoutedEventArgs e)
    {
        // 创建保存文件对话框
        SaveFileDialog saveFileDialog = new SaveFileDialog
        {
            Filter = "Brep Files (*.brep)|*.brep|All Files (*.*)|*.*", // 设置默认的文件类型过滤器
            InitialDirectory = System.IO.Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "mods") // 设置初始目录为指定的路径
        };

        // 显示保存文件对话框并检查用户是否选择了保存路径
        if (saveFileDialog.ShowDialog() == true)
        {
            // 获取用户选择的文件路径
            string filePath = saveFileDialog.FileName;
            string tempFilePath = System.IO.Path.Combine(System.IO.Path.GetTempPath(), "temp.brep");
            CompoundMaker maker = new();
            foreach (TShape s in inputShapes)
            {
                maker.Add(s);
            }

            // 创建 BrepExchange 对象并保存文件
            BrepExchange BE = new BrepExchange(maker.Shape());
            BE.SaveFile(tempFilePath);
            try
            {
                // 剪切文件
                System.IO.File.Move(tempFilePath, filePath);

                Debug.WriteLine($"文件已从 {tempFilePath} 剪切并重命名为 {filePath}");
            }
            catch (Exception ex)
            {
                System.IO.File.Delete(tempFilePath);
                Debug.WriteLine($"文件剪切失败: {ex.Message}");
            }
        }
    }

    private void Test_input_test_4_Click(object sender, RoutedEventArgs e)
    {
        EraseAll(false);
        InputWorkpiece = new(new STEPExchange("mods\\test4.stp").Shape().TopoShape);
        BasePlate = null;
        Display(InputWorkpiece.AIS, true);
        Canvas.FitAll();
    }

    private void Test_input_test_5_Click(object sender, RoutedEventArgs e)
    {
        EraseAll(false);
        InputWorkpiece = new(new STEPExchange("mods\\test5.stp").Shape().TopoShape);
        BasePlate = null;
        Display(InputWorkpiece.AIS, true);
        FitAll();
    }

    #endregion

    #region 选择模式

    private void Select_Shape_Button_Click(object sender, RoutedEventArgs e)
    {
        Debug.WriteLine(SelectionMode.Shape.ToString());
    }

    private void Select_Face_Button_Click(object sender, RoutedEventArgs e)
    {
        Debug.WriteLine(SelectionMode.Face.ToString());
    }

    private void Select_Wire_Button_Click(object sender, RoutedEventArgs e)
    {
        Debug.WriteLine(SelectionMode.Edge.ToString());
    }

    private void Select_Vertex_Button_Click(object sender, RoutedEventArgs e)
    {
        Debug.WriteLine(SelectionMode.Vertex.ToString());
    }

    private void Select_Shell_Button_Click(object sender, RoutedEventArgs e)
    {
        Debug.WriteLine(SelectionMode.Shell.ToString());
    }

    #endregion

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

    private void BasePlateXWidth_TextChanged(object sender, EventArgs e)
    {
        // 获取TextBox的新值
        TextBox textBox = (TextBox)sender;
        double newValue;
        if (double.TryParse(textBox.Text, out newValue))
        {
            // 更新 BasePlate本身
            if (BasePlate == null)
            {
                return;
            }
            BasePlate.OffsetX = (newValue - BasePlate.DX) / 2;
            BasePlate.UpdateAIS();
            DisplayEraseBasePlates(true);
            Canvas.Update();
        }
    }

    private void BasePlateYWidth_TextChanged(object sender, EventArgs e)
    {
        // 获取TextBox的新值
        TextBox textBox = (TextBox)sender;
        double newValue;
        if (double.TryParse(textBox.Text, out newValue))
        {
            // 更新 BasePlate本身
            if (BasePlate == null)
            {
                return;
            }
            BasePlate.OffsetY = (newValue - BasePlate.DY) / 2;
            BasePlate.UpdateAIS();
            DisplayEraseBasePlates(true);
            Canvas.Update();
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
            selectedDirection = seletedItem.Content.ToString() ?? string.Empty;
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
        object? sender,
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
            DisplayEraseInputWorkpiece(false);
            DisplayEraseBasePlates(false);
            if (CurrentPlate.Sutured)
            {
                DisplayCurrentPlate(false);
            }
            else
            {
                DisplayCurrentPieces(false);
            }
            Update();
            CurrentPlate
                .Pieces.OrderBy(item =>
                {
                    var plate = item;
                    return plate.Order;
                })
                .ToList();
            for (int i = 0; i < CurrentPlate.Pieces.Count; ++i)
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
        object? sender,
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
            DisplayEraseInputWorkpiece(false);
            DisplayEraseBasePlates(false);
            if (CurrentPlate.Sutured)
            {
                DisplayCurrentPlate(false);
            }
            else
            {
                DisplayCurrentPieces(false);
            }
            Canvas.Update();
            for (global::System.Int32 i = 0; i < CurrentPlate.Pieces.Count; ++i)
            {
                CurrentPlate_StackPanel.Children.Add(MakeStackItem(CurrentPlate.Pieces[i], i));
            }
        }
    }

    /// <summary>
    /// 选择添加的方向
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void AddPlateDirection_ComboBox_SelectionChanged(
        object? sender,
        SelectionChangedEventArgs e
    )
    {
        if (BasePlate == null)
        {
            return;
        }
        ComboBoxItem seletedItem = (ComboBoxItem)AddPlateDirection_ComboBox.SelectedItem;
        if (seletedItem != null)
        {
            AddPlateDirection = seletedItem.Content.ToString() ?? string.Empty;
        }

        //! 方向虽然用X、Y做区分，实际上还包括XY以外的方向，但只分为两种。
        if (AddPlateDirection == "X")
        {
            AddPlateDirection_TextBox.Text = DirMaker.GetDirAngleWithZ(DirectionX).ToString("F1");
            AddPlateLocationY_TextBox.Text = (BasePlate.Y + BasePlate.DY / 2).ToString("F1");
            AddPlateLocationY_TextBox.IsReadOnly = true;
            AddPlateLocationX_TextBox.IsReadOnly = false;
            AddPlateNumberString_TextBox.Text = "X";
        }
        else if (AddPlateDirection == "Y")
        {
            AddPlateDirection_TextBox.Text = DirMaker.GetDirAngleWithZ(DirectionY).ToString("F1");
            AddPlateLocationX_TextBox.Text = (BasePlate.X + BasePlate.DX / 2).ToString("F1");
            AddPlateLocationX_TextBox.IsReadOnly = true;
            AddPlateLocationY_TextBox.IsReadOnly = false;
            AddPlateNumberString_TextBox.Text = "Y";
        }
    }

    //单片
    private Grid? MakeStackItem(VerticalPiece thePiece, int theNum)
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
            ($"{thePiece.Order:F1}", 0),
            ($"L: {thePiece.Length:F1}", 1),
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
        //增加删除按钮
        Button deleteButton = new Button { Content = "删除" };
        Grid.SetColumn(deleteButton, labelDefinitions.Length);
        outerStackPanel.Children.Add(deleteButton);
        deleteButton.Click += (sender, e) =>
        {
            // 删除对应的 Piece
            CurrentPlate?.Pieces?.Remove(thePiece);
            CurrentPlate?.RemovePiece(thePiece);
            // 删除显示对象
            AISContext.Remove(thePiece.AIS, false);
            //删除对应的grid
            if (outerStackPanel.Parent is StackPanel theS)
            {
                theS.Children.Remove(outerStackPanel);
            }
            Canvas.Update();
        };
        // 绑定 thePiece 对象到第一个 Label 的 Tag 属性
        if (outerStackPanel.Children[0] is Label firstLabel)
        {
            firstLabel.Tag = thePiece;
            // 添加鼠标左键点击事件处理程序
            firstLabel.MouseLeftButtonDown += PieceLabel_MouseLeftButtonDown;
        }

        return outerStackPanel;
    }

    #endregion

    #region 按钮

    /// <summary>
    /// 更新选中竖板
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void UpdateCurrentPlate_Button_Click(object sender, RoutedEventArgs e)
    {
        if (CurrentPlate == null)
        {
            return;
        }
        string oldNumberString = CurrentPlate.NumberString;
        // 更新CurrentPlateValueLable的值
        double theX,
            theY,
            theAngle,
            theClearances,
            theMinSupportLen,
            theCuttingDistance;

        // 转换 X 位置
        if (!double.TryParse(CurrentPlateLocationX_TextBox.Text, out theX))
        {
            MessageBox.Show("X 位置输入的值不是有效的数字。");
            return;
        }
        // 转换 Y 位置
        if (!double.TryParse(CurrentPlateLocationY_TextBox.Text, out theY))
        {
            MessageBox.Show("Y 位置输入的值不是有效的数字。");
            return;
        }

        // 转换方向
        if (!double.TryParse(CurrentPlateDirection_TextBox.Text, out theAngle))
        {
            MessageBox.Show("X 方向输入的值不是有效的数字。");
            return;
        }
        // 转换 板两侧偏置
        if (!double.TryParse(CurrentPlateClearances_TextBox.Text, out theClearances))
        {
            MessageBox.Show("两侧偏置量输入的值不是有效的数字。");
            return;
        }
        // 转换 最小支撑长度
        if (!double.TryParse(CurrentPlateMinSupportLen_TextBox.Text, out theMinSupportLen))
        {
            MessageBox.Show("两侧偏置量输入的值不是有效的数字。");
            return;
        }
        // 转换 切断距离
        if (!double.TryParse(CurrentPlateCuttingDistance_TextBox.Text, out theCuttingDistance))
        {
            MessageBox.Show("两侧偏置量输入的值不是有效的数字。");
            return;
        }

        var newPlate = SimpleClampMaker.MakeVerticalPlate(
            InputWorkpiece.Shape,
            BasePlate,
            new PlatePose(
                new Pnt(theX, theY, CurrentPlate.Pose.Location.Z),
                DirMaker.AngleWithZ(theAngle)
            ),
            theClearances,
            theMinSupportLen,
            theCuttingDistance
        );
        //更新编号值
        newPlate.NumberString = CurrentPlateNumberString_TextBox.Text;
        MiddleToDownPlates.RemoveAll(item => item.NumberString == oldNumberString);
        if (oldNumberString[0] == 'X')
        {
            MiddleToDownPlates.Add(newPlate);
        }
        MiddleToUpPlates.RemoveAll(item => item.NumberString == oldNumberString);
        if (oldNumberString[0] == 'Y')
        {
            MiddleToUpPlates.Add(newPlate);
        }
        //删除旧的板的显示
        if (CurrentPlate.AIS != null)
        {
            ThreeDContext.Remove(CurrentPlate.AIS, false);
        }
        if (CurrentPlate.Pieces.Count() != 0)
        {
            foreach (var onePiece in CurrentPlate.Pieces)
            {
                ThreeDContext.Remove(onePiece.AIS, false);
            }
        }
        //替换为新的
        CurrentPlate = newPlate;
        //更新控件
        UpdateComboBox();
        //更新stack中的元素
        CurrentPlate_StackPanel.Children.Clear();
        for (global::System.Int32 i = 0; i < CurrentPlate.Pieces.Count; ++i)
        {
            CurrentPlate_StackPanel.Children.Add(MakeStackItem(CurrentPlate.Pieces[i], i));
        }
        if (CurrentPlate.Sutured)
        {
            DisplayCurrentPlate(false);
        }
        else
        {
            DisplayCurrentPieces(false);
        }
        Canvas.Update();
    }

    /// <summary>
    /// 更新板中储存的数值，但是不生成新板
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void UpdateCurrentPlatePara_Button_Click(object sender, RoutedEventArgs e)
    {
        if (CurrentPlate == null)
        {
            return;
        }
        double AH,
            AW;
        if (!double.TryParse(CurrentPlateAuxiliaryHight_TextBox.Text, out AH))
        {
            MessageBox.Show("辅助高 输入的值不是有效的数字。");
            return;
        }
        if (!double.TryParse(CurrentPlateAuxiliaryWidth_TextBox.Text, out AW))
        {
            MessageBox.Show("辅助宽 输入的值不是有效的数字。");
            return;
        }
        CurrentPlate.NumberString = CurrentPlateNumberString_TextBox.Text;
        CurrentPlate.AuxiliaryHight = AH;
        CurrentPlate.AuxiliaryWidth = AW;
    }

    /// <summary>
    /// 添加板
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void AddPlate_Button_Click(object sender, RoutedEventArgs e)
    {
        if (InputWorkpiece == null)
        {
            return;
        }
        if (BasePlate == null)
        {
            return;
        }

        #region 读取数字

        double theX,
            theY,
            theAngle;
        // 转换 X 位置
        if (!double.TryParse(AddPlateLocationX_TextBox.Text, out theX))
        {
            MessageBox.Show("X 位置输入的值不是有效的数字。");
            return;
        }

        // 转换 Y 位置
        if (!double.TryParse(AddPlateLocationY_TextBox.Text, out theY))
        {
            MessageBox.Show("Y 位置输入的值不是有效的数字。");
            return;
        }
        // 转换 X 方向
        if (!double.TryParse(AddPlateDirection_TextBox.Text, out theAngle))
        {
            MessageBox.Show("X 方向输入的值不是有效的数字。");
            return;
        }
        string theNumberString = AddPlateNumberString_TextBox.Text;

        #endregion

        //按照选择的方向添加
        VerticalPlate? newPlate = null;
        if (AddPlateDirection == "X")
        {
            newPlate = SimpleClampMaker.MakeVerticalPlate(
                InputWorkpiece.Shape,
                BasePlate,
                new PlatePose(
                    new Pnt(theX, BasePlate.Y + BasePlate.DY / 2, BasePlate.Z),
                    DirMaker.AngleWithZ(theAngle)
                ),
                ClearancesParameter,
                MinSupportingLenParameter,
                CuttingDistanceParameter
            );
            newPlate.NumberString = theNumberString;
            MiddleToDownPlates.Add(newPlate);
        }
        if (AddPlateDirection == "Y")
        {
            newPlate = SimpleClampMaker.MakeVerticalPlate(
                InputWorkpiece.Shape,
                BasePlate,
                new PlatePose(
                    new Pnt(BasePlate.X + BasePlate.DX / 2, theY, BasePlate.Z),
                    DirMaker.AngleWithZ(theAngle)
                ),
                ClearancesParameter,
                MinSupportingLenParameter,
                CuttingDistanceParameter
            );
            newPlate.NumberString = theNumberString;
            MiddleToUpPlates.Add(newPlate);
        }
        if (newPlate != null)
        {
            UpdateComboBox();
            DisplaySinglePlate(newPlate, false);
        }
        Canvas.Update();
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
        MakeBasePlate();
        DisplayEraseInputWorkpiece(true);
        DisplayEraseBasePlates(true);
        Canvas.Update();
    }

    /// <summary>
    /// 重置底板按钮
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void ResetBasePlate_Button_Clcik(object sender, RoutedEventArgs e)
    {
        if (BasePlate == null)
        {
            return;
        }
        BasePlate.OffsetX = 0.0;
        BasePlate.OffsetY = 0.0;
        BasePlate.UpdateAIS();
        BasePlateXWidth_TextBox.Text = (BasePlate.DX + BasePlate.OffsetX * 2).ToString("F1");
        BasePlateYWidth_TextBox.Text = (BasePlate.DY + BasePlate.OffsetY * 2).ToString("F1");
        DisplayEraseBasePlates(false);
        Canvas.Update();
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
            DisplayEraseInputWorkpiece(false);
            DisplayEraseBasePlates(false);

            MakeVerticalPlates();
            UpdateComboBox();
            DisplayVerticalPlatesPieces(false);

            //! debug
            //foreach (var item in MiddleToDownPlates)
            //{
            //    Display(item.AIS, false);
            //    OCCCanvas.AISContext.SetTransparency(item.AIS, 0.9, false);
            //}
            //foreach (var item in MiddleToUpPlates)
            //{
            //    Display(item.AIS, false);
            //    OCCCanvas.AISContext.SetTransparency(item.AIS, 0.9, false);
            //}
        }
        Update();
        FitAll();
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
        DisplayEraseInputWorkpiece(false);
        DisplayEraseBasePlates(false);
        DisplayVerticalPlates(false);

        Canvas.Update();
        Canvas.FitAll();
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
        Canvas.Update();
        Canvas.FitAll();
    }

    /// <summary>
    /// 导出展平后的板到STEP模型
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void OutputSTEP_Button_Click(object sender, RoutedEventArgs e)
    {
        OutputSTEP();
    }

    #region 操作器

    private InteractiveObject? manipulatedObject = null;

    /// <summary>
    /// 操作器
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void Manipulator_Button_Click(object sender, RoutedEventArgs e)
    {
        if (InputWorkpiece == null)
        {
            return;
        }
        if (manipulatedObject == null)
        {
            //关闭缩放、平移
            Canvas.Manipulator.SetPart(OCCTK.OCC.AIS.ManipulatorMode.Scaling, false);
            Canvas.Manipulator.SetPart(OCCTK.OCC.AIS.ManipulatorMode.TranslationPlane, false);
            Canvas.Manipulator.SetPart(OCCTK.OCC.AIS.ManipulatorMode.Translation, false);
            //绑定AIS对象
            manipulatedObject = InputWorkpiece.AIS;
            Canvas.Manipulator.Attach(manipulatedObject);
        }
        else
        {
            //! 必须先detach操作器，否则会因为AIS对象消失而出错
            Canvas.Manipulator.Detach();
            ThreeDContext.Remove(manipulatedObject, false);
            InputWorkpiece.Transform();
            manipulatedObject = null;
            DisplayEraseInputWorkpiece(false);
        }
        Canvas.Update();
    }

    #endregion

    /// <summary>
    /// 擦除选中的视图对象
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void Erase_Button_Click(object sender, RoutedEventArgs e) =>
        ThreeDContext.EraseSelected();

    /// <summary>
    /// 显示当前片
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void DisplayCurrentPieces_Button_Click(object sender, RoutedEventArgs e)
    {
        if (CurrentPlate == null)
        {
            return;
        }
        EraseAll(false);
        DisplayEraseInputWorkpiece(false);
        DisplayEraseBasePlates(false);
        DisplayCurrentPieces(false);

        Canvas.Update();
        Canvas.FitAll();
    }

    /// <summary>
    /// 显示当前板
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void DisplayCurrentPlate_Button_Click(object sender, RoutedEventArgs e)
    {
        if (CurrentPlate == null)
        {
            return;
        }
        if (!CurrentPlate.Sutured)
        {
            MessageBox.Show("板未连接");
            return;
        }
        EraseAll(false);
        DisplayEraseInputWorkpiece(false);
        DisplayEraseBasePlates(false);
        DisplayCurrentPlate(false);

        Canvas.Update();
        Canvas.FitAll();
    }

    /// <summary>
    /// 显示或隐藏工件
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void Display_Input_Button_Click(object sender, RoutedEventArgs e)
    {
        ShowInputWorkpiece = !ShowInputWorkpiece;
        DisplayEraseInputWorkpiece(ShowInputWorkpiece);
        Canvas.Update();
    }

    /// <summary>
    /// 显示或隐藏底板
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void Display_Base_Button_Click(object sender, RoutedEventArgs e)
    {
        ShowBasePlate = !ShowBasePlate;
        DisplayEraseBasePlates(ShowBasePlate);
        Canvas.Update();
    }

    #endregion

    #region 算法逻辑

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
        //将工件平移到左下角点为0的位置
        AISContext.Remove(InputWorkpiece.AIS, false);
        InputWorkpiece = new(
            new OCCTK.OCC.BRepBuilderAPI.Transform(
                InputWorkpiece.Shape,
                BasePlate.Translation
            ).Shape()
        );

        BasePlateXWidth_TextBox.Text = (BasePlate.DX + BasePlate.OffsetX * 2).ToString("F1");
        BasePlateYWidth_TextBox.Text = (BasePlate.DY + BasePlate.OffsetY * 2).ToString("F1");
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
                OffextX = (BasePlate.DX - InitialOffsetXParameter * 2) / (float)(XNum - 1);
                OffextY = (BasePlate.DY - InitialOffsetYParameter * 2) / (float)(YNum - 1);
                break;
            case VPCMode.Offset:
                // 直接使用设定的偏移值
                OffextX = OffsetXParameter;
                OffextY = OffsetYParameter;
                break;
        }
        if (false)
        {
            // 创建X方向竖板
            int num = 0;
            while (
                BasePlate.X + InitialOffsetXParameter - LINEAR_TOL <= tempX
                && tempX <= BasePlate.X + BasePlate.DX - InitialOffsetXParameter + LINEAR_TOL
            )
            {
                var tempPlate = SimpleClampMaker.MakeVerticalPlate(
                    InputWorkpiece.Shape,
                    BasePlate,
                    new PlatePose(new Pnt(tempX, BasePlate.Y, 0.0), DirectionX),
                    ClearancesParameter,
                    MinSupportingLenParameter,
                    CuttingDistanceParameter
                );
                tempPlate.NumberString = $"X{num}";
                //! debug
                foreach (var onePiece in tempPlate.Pieces)
                {
                    Debug.WriteLine($"{tempPlate.NumberString}: {onePiece.Order:F1}");
                }
                MiddleToDownPlates.Add(tempPlate);
                num += 1;
                tempX += OffextX;
            }
            // 创建Y方向竖板
            num = 0;
            while (
                BasePlate.Y + InitialOffsetYParameter - LINEAR_TOL <= tempY
                && tempY <= BasePlate.Y + BasePlate.DY - InitialOffsetYParameter + LINEAR_TOL
            )
            {
                var tempPlate = SimpleClampMaker.MakeVerticalPlate(
                    InputWorkpiece.Shape,
                    BasePlate,
                    new PlatePose(new Pnt(BasePlate.X, tempY, 0.0), DirectionY),
                    ClearancesParameter,
                    MinSupportingLenParameter,
                    CuttingDistanceParameter
                );
                tempPlate.NumberString = $"Y{num}";
                //! debug
                foreach (var onePiece in tempPlate.Pieces)
                {
                    Debug.WriteLine($"{tempPlate.NumberString}: {onePiece.Order:F1}");
                }
                MiddleToUpPlates.Add(tempPlate);
                num += 1;
                tempY += OffextY;
            }
        }
        else
        {
            #region 多线程创建

            //创建X方向竖板
            // 使用并发集合以便在线程间安全地添加元素
            Debug.WriteLine("Create X");
            ConcurrentBag<VerticalPlate> concurrentDownPlates = new ConcurrentBag<VerticalPlate>();
            // 创建并行任务
            var createXTask = Task.Run(() =>
            {
                Parallel.For(
                    0,
                    (int)
                        Math.Ceiling(
                            (BasePlate.DX - (InitialOffsetXParameter - LINEAR_TOL) * 2) / OffextX
                        ),
                    i =>
                    {
                        double currentX = InitX + i * OffextX;
                        Debug.WriteLine(
                            $"{BasePlate.X + InitialOffsetXParameter}, {currentX}, {BasePlate.X + BasePlate.DX - InitialOffsetXParameter}"
                        );
                        if (
                            BasePlate.X + InitialOffsetXParameter - LINEAR_TOL <= currentX
                            && currentX
                                <= BasePlate.X + BasePlate.DX - InitialOffsetXParameter + LINEAR_TOL
                        )
                        {
                            var tempPlate = SimpleClampMaker.MakeVerticalPlate(
                                InputWorkpiece.Shape,
                                BasePlate,
                                new PlatePose(
                                    new Pnt(currentX, (BasePlate.Y + BasePlate.DY / 2), 0.0),
                                    DirectionX
                                ),
                                ClearancesParameter,
                                MinSupportingLenParameter,
                                CuttingDistanceParameter
                            );
                            tempPlate.NumberString = $"X{i}";
                            concurrentDownPlates.Add(tempPlate);
                        }
                    }
                );
                // 将并发集合中的元素添加到最终的集合中
                MiddleToDownPlates.AddRange(concurrentDownPlates);
            });

            //创建Y方向竖板
            // 使用并发集合以便在线程间安全地添加元素
            Debug.WriteLine("Create Y");
            var createYTask = Task.Run(() =>
            {
                ConcurrentBag<VerticalPlate> concurrentUpPlates =
                    new ConcurrentBag<VerticalPlate>();
                Parallel.For(
                    0,
                    (int)
                        Math.Ceiling(
                            (BasePlate.DY - (InitialOffsetYParameter - LINEAR_TOL) * 2) / OffextY
                        ),
                    i =>
                    {
                        double currentY = InitY + i * OffextY;
                        Debug.WriteLine(
                            $"{BasePlate.Y + InitialOffsetYParameter}, {currentY}, {BasePlate.Y + BasePlate.DY - InitialOffsetYParameter}"
                        );
                        if (
                            BasePlate.Y + InitialOffsetYParameter - LINEAR_TOL <= currentY
                            && currentY
                                <= BasePlate.Y + BasePlate.DY - InitialOffsetYParameter + LINEAR_TOL
                        )
                        {
                            var tempPlate = SimpleClampMaker.MakeVerticalPlate(
                                InputWorkpiece.Shape,
                                BasePlate,
                                new PlatePose(
                                    new Pnt((BasePlate.X + BasePlate.DX / 2), currentY, 0.0),
                                    DirectionY
                                ),
                                ClearancesParameter,
                                MinSupportingLenParameter,
                                CuttingDistanceParameter
                            );
                            tempPlate.NumberString = $"Y{i}";
                            concurrentUpPlates.Add(tempPlate);
                        }
                    }
                );
                // 将并发集合中的元素添加到最终的集合中
                MiddleToUpPlates.AddRange(concurrentUpPlates);
            });
            // 等待两个任务完成
            Task.WaitAll(createXTask, createYTask);
            // 输出完成信息
            Debug.WriteLine("Both X and Y plates have been created.");
            #endregion
        }
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
        //! debug
        //foreach (var tempPlate in VerticalPlates)
        //{
        //    Debug.WriteLine(
        //        $"{tempPlate.NumberString}_start: {tempPlate.Start.X():F1}, {tempPlate.Start.Y():F1}"
        //    );
        //    Debug.WriteLine(
        //        $"{tempPlate.NumberString}_end: {tempPlate.End.X():F1}, {tempPlate.End.Y():F1}"
        //    );
        //    Debug.Write($"{tempPlate.NumberString}: ");
        //    foreach (var onePiece in tempPlate.Pieces)
        //    {
        //        Debug.Write($"{onePiece.Order:F1}, ");
        //    }
        //    Debug.WriteLine("");
        //}


        // 给板打标签
        List<VerticalPlate> tempDown = new List<VerticalPlate>();
        foreach (var plate in MiddleToDownPlates)
        {
            tempDown.Add(SimpleClampMaker.BrandNumberVerticalPlate(plate, 10.0));
        }
        MiddleToDownPlates = tempDown;
        List<VerticalPlate> tempUp = new List<VerticalPlate>();
        foreach (var plate in MiddleToUpPlates)
        {
            tempUp.Add(SimpleClampMaker.BrandNumberVerticalPlate(plate, 10.0));
        }
        MiddleToUpPlates = tempUp;

        // 给底板开槽
        try
        {
            BasePlate = SimpleClampMaker.SlotBasePLates(
                BasePlate,
                MiddleToDownPlates,
                MiddleToUpPlates
            );
        }
        catch (Exception ex)
        {
            Debug.WriteLine(ex);
        }
        //给底板烙印标记
        BasePlate = SimpleClampMaker.BrandNumberBasePlate(BasePlate, 10.0);
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
        // 构造组合体
        CompoundMaker theMaker = new CompoundMaker();
        theMaker.Add(BasePlate.Shape);
        foreach (var onePlate in VerticalPlates)
        {
            if (onePlate.Shape != null)
            {
                theMaker.Add(onePlate.Shape);
            }
        }
        STEPExchange exchanger3D = new(theMaker.Shape());
        exchanger3D.SaveSingleShape("mods\\test_output_3d.STEP");
        //保存排列后的板
        STEPExchange exchanger = new(CombinedFixtureBoard.Shape);
        exchanger.SaveSingleShape("mods\\test_output_Fixtured.STEP");
    }

    #endregion

    #region 视图显示

    //简化函数调用
    private Action<InteractiveObject, bool> Display => AISContext.Display;
    private Action<InteractiveObject, Color, bool> SetColor => AISContext.SetColor;
    private Action<InteractiveObject, double, bool> SetTransparency => AISContext.SetTransparency;
    private Action<InteractiveObject, bool> Erase => AISContext.Erase;
    private Action<bool> EraseAll => ThreeDContext.EraseAll;
    private Action Update => Canvas.Update;
    private Action FitAll => Canvas.FitAll;

    /// <summary>
    /// 显示或隐藏工件
    /// </summary>
    /// <param name="update"></param>
    private void DisplayEraseInputWorkpiece(bool update)
    {
        if (InputWorkpiece != null)
        {
            if (ShowInputWorkpiece)
            {
                Display(InputWorkpiece.AIS, false);
                //AISContext.SetTransparency(InputWorkpiece.AIS, 0.9, false);
                SetColor(InputWorkpiece.AIS, new Color(125, 125, 125), update);
            }
            else
            {
                Erase(InputWorkpiece.AIS, update);
            }
        }
    }

    /// <summary>
    /// 显示或隐藏底板
    /// </summary>
    /// <param name="update"></param>
    private void DisplayEraseBasePlates(bool update)
    {
        if (BasePlate != null)
        {
            if (ShowBasePlate)
            {
                Display(BasePlate.AIS, false);
                //AISContext.SetTransparency(BasePlate.AIS, 0.8, false);
                SetColor(BasePlate.AIS, new Color(204, 102, 51), false);
            }
            else
            {
                Erase(BasePlate.AIS, update);
            }
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
                //AISContext.SetTransparency(onePiece.AIS, 0.3, false);
                SetColor(onePiece.AIS, new Color(255, 0, 0), update);
            }
        }
        foreach (var onePlate in MiddleToUpPlates)
        {
            foreach (var onePiece in onePlate.Pieces)
            {
                Display(onePiece.AIS, false);
                //AISContext.SetTransparency(onePiece.AIS, 0.3, false);
                SetColor(onePiece.AIS, new(0, 255, 0), update);
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
                //AISContext.SetTransparency(onePlate.AIS, 0.3, false);
                SetColor(onePlate.AIS, new Color(255, 0, 0), update);
            }
        }
        foreach (var onePlate in MiddleToUpPlates)
        {
            if (onePlate.Sutured)
            {
                Display(onePlate.AIS, false);
                //AISContext.SetTransparency(onePlate.AIS, 0.3, false);
                SetColor(onePlate.AIS, new(0, 255, 0), update);
            }
        }
    }

    /// <summary>
    /// 显示对应的竖板（若合并，则显示整体，否则显示分片）
    /// </summary>
    /// <param name="thePlate"></param>
    /// <param name="update"></param>
    private void DisplaySinglePlate(VerticalPlate thePlate, bool update, double transparency = 0.0)
    {
        Color theColor = new(125, 125, 125);
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
            AISContext.SetTransparency(thePlate.AIS, transparency, false);
            SetColor(thePlate.AIS, theColor, update);
        }
        else
        {
            foreach (var onePiece in thePlate.Pieces)
            {
                Display(onePiece.AIS, false);
                AISContext.SetTransparency(onePiece.AIS, transparency, false);
                SetColor(onePiece.AIS, theColor, update);
            }
        }
    }

    /// <summary>
    /// 显示当前选中的竖板（分片）
    /// </summary>
    /// <param name="update"></param>
    private void DisplayCurrentPieces(bool update)
    {
        if (CurrentPlate != null)
        {
            Color theColor = new(125, 125, 125);
            if (MiddleToDownPlates.Contains(CurrentPlate))
            {
                theColor = new(255, 0, 0);
                //其它板按半透明显示
                foreach (
                    var otherPlate in MiddleToDownPlates
                        .Where(plate => !plate.Equals(CurrentPlate))
                        .ToList()
                )
                {
                    DisplaySinglePlate(otherPlate, update, 0.6);
                }
            }
            else if (MiddleToUpPlates.Contains(CurrentPlate))
            {
                theColor = new(0, 255, 0);
                //其它板按半透明显示
                foreach (
                    var otherPlate in MiddleToUpPlates
                        .Where(plate => !plate.Equals(CurrentPlate))
                        .ToList()
                )
                {
                    DisplaySinglePlate(otherPlate, update, 0.6);
                }
            }
            foreach (var Piece in CurrentPlate.Pieces)
            {
                Display(Piece.AIS, update);
                AISContext.SetTransparency(Piece.AIS, 0.0, update);
                SetColor(Piece.AIS, theColor, update);
            }
        }
    }

    /// <summary>
    /// 显示当前选中的竖板（整体）
    /// </summary>
    /// <param name="update"></param>
    private void DisplayCurrentPlate(bool update)
    {
        if (CurrentPlate != null)
        {
            Color theColor = new(125, 125, 125);
            if (MiddleToDownPlates.Contains(CurrentPlate))
            {
                theColor = new(255, 0, 0);
                //其它板按半透明显示
                foreach (
                    var otherPlate in MiddleToDownPlates
                        .Where(plate => !plate.Equals(CurrentPlate))
                        .ToList()
                )
                {
                    DisplaySinglePlate(otherPlate, update, 0.6);
                }
            }
            else if (MiddleToUpPlates.Contains(CurrentPlate))
            {
                theColor = new(0, 255, 0);
                //其它板按半透明显示
                foreach (
                    var otherPlate in MiddleToUpPlates
                        .Where(plate => !plate.Equals(CurrentPlate))
                        .ToList()
                )
                {
                    DisplaySinglePlate(otherPlate, update, 0.6);
                }
            }
            if (CurrentPlate.AIS != null)
            {
                Display(CurrentPlate.AIS, update);
                SetColor(CurrentPlate.AIS, theColor, update);
                AISContext.SetTransparency(CurrentPlate.AIS, 0.0, update);
            }
        }
    }

    // 选中单个片的交互
    private void PieceLabel_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
    {
        // 获取点击的 Label
        Label clickedLabel = sender as Label;

        // 确保 Label 不为空且 Tag 不为空
        if (clickedLabel == null || clickedLabel.Tag == null)
            return;
        // 获取 Grid（假设 Label 是直接添加到 Grid 中的）
        Grid parentGrid = clickedLabel.Parent as Grid;
        StackPanel parentStack = parentGrid.Parent as StackPanel;

        if (parentGrid == null || parentStack == null)
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
                if (row != clickedRow)
                    continue;
                if (column >= 0 && column < colorSequence.Length)
                {
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
        var piece = clickedLabel.Tag as VerticalPiece;
        if (piece != null && piece.AIS != null)
        {
            AISContext.SelectAIS(piece.AIS, true);
        }
    }

    #endregion

    #region 实现接口
    public void OnSelectPiece(AShape theAIS)
    {
        if (CurrentPlate_StackPanel.Children.Count == 0)
        {
            return;
        }
        if (theAIS == null)
        {
            return;
        }
        int clickedRow = 999;
        //获取选中的对象列
        foreach (Grid theGrid in CurrentPlate_StackPanel.Children)
        {
            StackPanel parentStack = theGrid.Parent as StackPanel;
            // 遍历每个 Grid 中的子元素
            var tl = theGrid.Children[0];
            if (tl is Label thelabel)
            {
                if (thelabel.Tag is VerticalPiece onePiece)
                {
                    if (onePiece.AIS.Equals(theAIS))
                    {
                        clickedRow = parentStack.Children.IndexOf(theGrid);
                    }
                }
            }
        }
        //高亮显示获取的列
        foreach (Grid theGrid in CurrentPlate_StackPanel.Children)
        {
            StackPanel parentStack = theGrid.Parent as StackPanel;
            int row = parentStack.Children.IndexOf(theGrid);
            foreach (var child in theGrid.Children)
            {
                if (child is Label label)
                {
                    int column = Grid.GetColumn(label);
                    // 如果是点击的行，按照颜色数组设置背景颜色
                    if (row == clickedRow)
                    {
                        if (column >= 0 && column < colorSequence.Length)
                        {
                            label.Background = colorSequence[column];
                        }
                    }
                    else
                    {
                        // 其他行设置为透明
                        label.Background = Brushes.Transparent;
                    }
                }
            }
        }
    }

    public void MousePosition(int X, int Y)
    {
        currentMouse_Label.Content = $"鼠标坐标: {X}, {Y}";
    }

    public void KeyDown(System.Windows.Forms.Keys keys)
    {
        currentPressKey_Label.Content = $"按下的按键: {keys}";
    }

    public void KeyUp(System.Windows.Forms.Keys keys)
    {
        // 清空显示
        currentPressKey_Label.Content = "按下的按键: ";
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
    }

    /// <summary>
    /// 提取NumberString中的数字
    /// </summary>
    /// <param name="numberString"></param>
    /// <returns></returns>
    private static int ExtractNumber(string numberString)
    {
        // 去掉开头的字母部分
        // 假设 NumberString 以字母 X 开头，数字在字母后面
        if (numberString.Length > 1 && numberString[0] == 'X')
        {
            // 提取数字部分并转换为整数
            if (int.TryParse(numberString.Substring(1), out int result))
            {
                return result;
            }
        }
        return 0; // 默认值，如果无法提取数字
    }

    /// <summary>
    /// 更新控件
    /// </summary>
    private void UpdateComboBox()
    {
        ClearPlatesInfo();
        // 按照 NumberString 中的数字进行排序
        var sortedDownPlates = MiddleToDownPlates
            .OrderBy(p => ExtractNumber(p.NumberString))
            .ToList();
        foreach (var onePlate in sortedDownPlates)
        {
            CurrentPlateLocationX_ComboBox.Items.Add(onePlate);
        }
        var sortedUpPlates = MiddleToUpPlates.OrderBy(p => ExtractNumber(p.NumberString)).ToList();
        foreach (var onePlate in sortedUpPlates)
        {
            CurrentPlateLocationY_ComboBox.Items.Add(onePlate);
        }
    }

    #region 测试
    private void FitAll_Button_Click(object sender, RoutedEventArgs e)
    {
        Canvas.FitAll();
    }

    private void TestInput()
    {
        InputWorkpiece = new(new STEPExchange("mods\\mytest.stp").Shape().TopoShape);
    }

    #endregion
}
