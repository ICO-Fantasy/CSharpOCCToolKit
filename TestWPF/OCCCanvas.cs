using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq.Expressions;
using System.Windows.Forms;
using OCCTK.Extension;
using OCCTK.OCC;
using OCCTK.OCC.AIS;
using OCCTK.OCC.gp;
using OCCTK.OCC.Topo;
using OCCTK.OCC.V3d;
using OCCTK.Visualization;
using Color = OCCTK.Extension.Color;

namespace OCCViewForm;

#region Enum

public enum Action3d
{
    // Normal
    Nothing,
    SingleSelect,
    MultipleSelect,
    XORSelect,
    AreaSelect,
    MultipleAreaSelect,
    XORAreaSelect,
    AreaZooming,
    DynamicRotation,
    DynamicPanning,
    Prohibition,

    // Manipulator
    Manipulator_Translation
}

#endregion

public class SingleManipulator
{
    // 私有静态变量用于保存单例实例
    private static Manipulator? _instance;
    private static readonly object _lock = new();

    // 私有构造函数，防止外部实例化
    private SingleManipulator() { }

    // 公共静态属性，用于访问单例实例
    public static Manipulator Instance
    {
        get
        {
            // 确保线程安全
            lock (_lock)
            {
                _instance ??= new Manipulator();
                return _instance;
            }
        }
    }
}

public partial class OCCCanvas : Form
{
    public OCCCanvas()
    {
        InitializeComponent();
        //! 删除From自带的边界
        FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
        //! ControlStyles.DoubleBuffer 双缓冲会导致画面绘制失效，不能启用
        SetStyle(ControlStyles.OptimizedDoubleBuffer, false); // 关闭优化双重缓冲
        //! 设置自定义绘制和防止擦除背景，避免拖动画布时画面闪动
        SetStyle(ControlStyles.AllPaintingInWmPaint, true); // 防止擦除背景。
        SetStyle(ControlStyles.UserPaint, true); // 使用自定义的绘制。

        Canvas = new CSharpViewer();
        //初始化画布，创建上下文
        Canvas.InitViewer();
        //创建相机
        MainView = Canvas.CreateView(this.Handle);
        if (MainView == null)
        {
            //MessageBox.Show("图形初始化失败", "Error!");
            throw new Exception("图形初始化失败");
        }

        //获取视图对象
        Viewer = Canvas.GetViewer();
        //获取上下文管理器对象
        AISContext = Canvas.GetContext();
        //创建选择框
        myBubberBand = new();
        //鼠标样式重置计时器
        cursorResetTimer = new System.Windows.Forms.Timer();
        //默认设置
        SetDefault();

        MainView.Redraw();
        MainView.MustBeResized();
    }

    #region 字段

    /// <summary>
    /// 框选零宽度的最小值
    /// </summary>
    private const int ZEROWIDTHMIN = 1;

    float devicePixelRatioX;
    float devicePixelRatioY;

    private List<AShape> _CurrentAIS = new List<AShape>();

    public List<AShape> SelectedAISList = new List<AShape>();

    #region 视图立方

    private ViewCube? _ViewCube;
    private bool _ShowViewCube;
    public bool ShowViewCube
    {
        get { return _ShowViewCube; }
        set
        {
            _ShowViewCube = value;
            DisplayViewCube(false);
        }
    }

    private void DisplayViewCube(bool update)
    {
        _ViewCube ??= new(3.0);
        if (_ShowViewCube)
        {
            Display(_ViewCube, update);
        }
        else
        {
            if (AISContext.IsDisplayed(_ViewCube))
            {
                Erase(_ViewCube, update);
            }
        }
    }

    #endregion

    #region 视图坐标系

    private Trihedron? _ViewTrihedron;
    private bool _ShowViewTrihedron;
    public bool ShowViewTrihedron
    {
        get { return _ShowViewTrihedron; }
        set
        {
            _ShowViewTrihedron = value;
            DisplayViewTrihedron(false);
        }
    }

    private void DisplayViewTrihedron(bool update)
    {
        if (_ViewTrihedron == null)
        {
            _ViewTrihedron = new(100);
            _ViewTrihedron.SetAspect(10, 10);
        }
        if (_ShowViewTrihedron)
        {
            Display(_ViewTrihedron, update);
        }
        else
        {
            if (AISContext.IsDisplayed(_ViewTrihedron))
            {
                Erase(_ViewTrihedron, update);
            }
        }
    }

    #endregion

    #region 原点坐标系

    private Trihedron? _OriginTrihedron;
    private bool _ShowOriginTrihedron;
    public bool ShowOriginTrihedron
    {
        get { return _ShowOriginTrihedron; }
        set
        {
            _ShowOriginTrihedron = value;
            DisplayOriginTrihedron(false);
        }
    }

    private void DisplayOriginTrihedron(bool update)
    {
        _OriginTrihedron ??= new(100);
        if (_ShowOriginTrihedron)
        {
            Display(_OriginTrihedron, update);
        }
        else
        {
            Erase(_OriginTrihedron, update);
        }
    }

    #endregion

    #region 操作器

    /// <summary>
    /// AIS操作器
    /// </summary>
    private readonly Manipulator myManipulator = SingleManipulator.Instance;

    private Trsf? myManipulatorTransfrom;
    private ManipulatorMode? _manipulatorMode;
    private ManipulatorAxisIndex? _manipulatorAxis;

    #endregion

    #endregion

    #region 属性

    /// <summary>
    /// C++委托的视图对象
    /// </summary>
    public CSharpViewer Canvas { get; set; }

    /// <summary>
    /// 画布对象
    /// </summary>
    public Viewer Viewer { get; set; }

    /// <summary>
    /// 主视图
    /// </summary>
    public OCCTK.OCC.V3d.View MainView { get; set; }

    /// <summary>
    /// C++委托的交互对象管理器
    /// </summary>
    public InteractiveContext AISContext { get; set; }

    #region 状态flag

    private bool _ShowGrid;

    /// <summary>
    /// 是否显示网格
    /// </summary>
    public bool ShowGrid
    {
        get { return _ShowGrid; }
        set
        {
            _ShowGrid = value;
            //todo
        }
    }

    /// <summary>
    /// 当前显示模式
    /// </summary>
    public DisplayMode CurrentDisplayMode { get; private set; }

    public OCCTK.OCC.AIS.SelectionMode _currentSelectionMode;
    public OCCTK.OCC.AIS.SelectionMode CurrentSelectionMode
    {
        get { return _currentSelectionMode; }
        private set
        {
            if (AISContext != null)
            {
                AISContext.SetSelectionMode(value);
                _currentSelectionMode = value;
            }
        }
    }

    /// <summary>
    /// 当前视图方向
    /// </summary>
    public OCCTK.OCC.V3d.ViewOrientation CurrentViewOrientation { get; private set; }

    #endregion

    #region 显示模式
    /// <summary>
    /// 隐藏线显示模式
    /// </summary>
    public bool DegenerateMode { get; private set; }

    /// <summary>
    /// 框线模式
    /// </summary>
    public bool IsWireframeEnabled { get; private set; }

    /// <summary>
    /// 阴影模式
    /// </summary>
    public bool IsShadingEnabled { get; private set; }

    #endregion

    #endregion

    private void InitializeComponent()
    {
        // 标题栏中不显示控制框的值
        ControlBox = false;
        // 设置不为顶层窗口
        TopLevel = false;
        this.ImeMode = System.Windows.Forms.ImeMode.NoControl;

        SizeChanged += new System.EventHandler(OnSizeChanged);
        Paint += new System.Windows.Forms.PaintEventHandler(OnPaint);

        RegisterMouseAction();
        RegisterKeyBoardAction();
    }

    private void SetDefault()
    {
        if (Viewer == null)
        {
            throw new Exception("Canvas is null");
        }
        if (AISContext == null)
        {
            throw new Exception("AISContext is null");
        }
        if (MainView == null)
        {
            throw new Exception("MainView is null");
        }

        //设置默认灯光
        Viewer.SetICOLight();
        //设置交互默认值
        AISContext.SetDefault();
        //设置相机默认值
        MainView.SetICORendering();
        MainView.SetDefaultBGColor();
        //设置选择模式
        CurrentSelectionMode = OCCTK.OCC.AIS.SelectionMode.Shape;

        float dpiX = CreateGraphics().DpiX;
        float dpiY = CreateGraphics().DpiY;

        // 计算设备像素比
        devicePixelRatioX = dpiX / 96.0f;
        devicePixelRatioY = dpiY / 96.0f;

        #region 设置画布

        //默认操作
        _currentAction3d = Action3d.Nothing;
        //隐藏线模式
        DegenerateMode = true;
        //显示网格
        ShowGrid = false;
        //显示视图立方
        ShowViewCube = true;
        //显示原点坐标轴
        ShowOriginTrihedron = false;
        //显示视图坐标轴
        ShowViewTrihedron = false;
        //显示刻度坐标系
        MainView.DisplayDefault_GraduatedTrihedron();

        #endregion

        _manipulatorMode = null;
        _manipulatorAxis = null;
    }

    /// <summary>
    /// 循环选择下一个枚举值
    /// </summary>
    /// <param name="currentEnum"></param>
    /// <returns></returns>
    private T CycleEnum<T>(T currentEnum)
        where T : Enum
    {
        // 获取枚举类型的所有值
        T[] enumValues = (T[])Enum.GetValues(typeof(T));

        // 获取当前枚举值的索引
        int currentIndex = Array.IndexOf(enumValues, currentEnum);

        // 获取下一个枚举值的索引，如果当前值是最后一个，则循环到第一个
        int nextIndex = (currentIndex + 1) % enumValues.Length;

        // 返回下一个枚举值
        return enumValues[nextIndex];
    }

    #region 重写虚方法

    #region 渲染

    protected override void OnPaintBackground(PaintEventArgs e)
    {
        // 不调用基类方法，避免系统背景绘制
    }

    private void OnPaint(object? sender, PaintEventArgs e)
    {
        if (AISContext == null || MainView == null)
        {
            return;
        }

        MainView.Redraw();
        AISContext.UpdateCurrentViewer();
    }

    private void OnSizeChanged(object? sender, EventArgs e)
    {
        if (MainView == null)
        {
            return;
        }
        MainView.MustBeResized();
    }

    #endregion

    #endregion

    #region 视图控制

    public new void Update()
    {
        MainView.Redraw();
        AISContext.UpdateCurrentViewer();
    }

    /// <summary>
    /// 相机缩放至最佳尺寸
    /// </summary>
    public void FitAll()
    {
        MainView.FitAll(0.01, true);
    }

    /// <summary>
    /// 循环设置相机视图方向
    /// </summary>
    public void SetViewOrientation()
    {
        MainView.SetViewOrientation(CycleEnum(CurrentViewOrientation), true);
    }

    /// <summary>
    /// 设置相机视图方向为theMode
    /// </summary>
    /// <param name="theMode"></param>
    public void SetViewOrientation(ViewOrientation theMode)
    {
        CurrentViewOrientation = theMode;
        MainView.SetViewOrientation(theMode, true);
    }

    /// <summary>
    /// 重置视图的居中和方向
    /// </summary>
    public void Reset()
    {
        MainView.Reset();
    }

    /// <summary>
    /// 设置隐藏线模式
    /// </summary>
    /// <param name="theMode"></param>
    public void SetHidden(bool toOpen)
    {
        MainView.SetDegenerateMode(toOpen);
        DegenerateMode = toOpen;
    }

    #endregion

    #region 显示

    public void Display(InteractiveObject theAIS, bool Toupdate = true)
    {
        AISContext.Display(theAIS, false);
        //默认颜色为灰色
        AISContext.SetColor(theAIS, new Color(125, 125, 125), Toupdate);
    }

    public void EraseSelected()
    {
        AISContext.EraseSelected();
    }

    public void Erase(InteractiveObject theAIS, bool Toupdate)
    {
        AISContext.Erase(theAIS, Toupdate);
    }

    public void EraseAll(bool Toupdate)
    {
        AISContext.EraseAll(false);
        DisplayViewCube(false);
        DisplayOriginTrihedron(false);
        DisplayViewTrihedron(Toupdate);
    }

    public void Remove(InteractiveObject theAIS, bool Toupdate)
    {
        AISContext.Remove(theAIS, Toupdate);
    }

    #endregion

    #region 对象交互

    /// <summary>
    /// 设置选择模式
    /// </summary>
    /// <param name="theMode"></param>
    public void SetSelectionMode(OCCTK.OCC.AIS.SelectionMode theMode)
    {
        AISContext.SetSelectionMode(theMode);
    }

    /// <summary>
    /// 获取选中的AIS对象
    /// </summary>
    /// <returns></returns>
    public AShape GetSelectedShape()
    {
        return AISContext.SelectedAIS();
    }

    #region 操作器

    //Ax2 debugAX2;
    //Trsf debugT;

    public void Attach(InteractiveObject theAIS)
    {
        myManipulator.Attach(theAIS, true, true, true);
        //if (theAIS is AShape) {
        //AShape a = (AShape)theAIS;
        //debugT = a.Shape().Location();
        //myManipulatorTransfrom = debugT;
        //debugAX2 = myManipulator.Position();
        //Debug.WriteLine($"\n\n\nManipulator_ax2: {debugAX2}");
        //Debug.WriteLine($"debugT: {debugT}");
        //}
    }

    public void Dettach()
    {
        myManipulator.Detach();
    }

    public void SetManipulatorPart(ManipulatorMode theMode, bool enable)
    {
        myManipulator.SetPart(theMode, enable);
    }

    /// <summary>
    /// 设置对应轴的可视部分
    /// </summary>
    /// <param name="axisIndex"></param>
    /// <param name="theMode"></param>
    /// <param name="enable"></param>
    public void SetManipulatorPart(
        ManipulatorAxisIndex axisIndex,
        ManipulatorMode theMode,
        bool enable
    )
    {
        myManipulator.SetPart(axisIndex, theMode, enable);
    }

    public void Detach()
    {
        //myManipulator.DeactivateCurrentMode();
        myManipulator.Detach();
        //myManipulator.SetPart(ManipulatorMode.TranslationPlane, true);
        //myManipulator.SetPart(ManipulatorMode.Rotation, true);
        //myManipulator.SetPart(ManipulatorMode.Translation, true);
        //myManipulator.SetPart(ManipulatorMode.Scaling, true);
    }

    public Trsf? GetManipulatorTransformation()
    {
        return myManipulatorTransfrom;
    }

    #endregion

    #endregion

    #region 其它设置

    #endregion
}
