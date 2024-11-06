using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq.Expressions;
using System.Windows.Forms;
using System.Windows.Input;
using log4net;
using OCCTK.Extension;
using OCCTK.OCC;
using OCCTK.OCC.AIS;
using OCCTK.OCC.gp;
using OCCTK.OCC.OpenGL;
using OCCTK.OCC.Topo;
using OCCTK.OCC.V3d;
using OCCTK.Visualization;
using TestWPF;
using Color = OCCTK.Extension.Color;
using View = OCCTK.OCC.V3d.View;

namespace OCCViewForm;

public partial class OCCCanvas : Form
{
    private static readonly ILog log = LogManager.GetLogger(typeof(App));

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

        //创建引擎
        myGraphicDriver = new();
        //创建视图对象
        myViewer = new(myGraphicDriver);
        //创建上下文管理器
        myAISContext = new InteractiveContext(myViewer);
        //创建相机
        ViewList.Add(myViewer.CreateView());
        MainView?.SetWindow(this.Handle);
        if (MainView == null)
        {
            //MessageBox.Show("图形初始化失败", "Error!");
            throw new Exception("图形初始化失败");
        }
        //创建操作器
        myManipulator = new();
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

    private readonly GraphicDriver myGraphicDriver;
    private readonly Viewer myViewer;
    private readonly InteractiveContext myAISContext;

    /// <summary>
    /// 框选零宽度的最小值
    /// </summary>
    private const int ZEROWIDTHMIN = 1;

    /// <summary>
    /// 设备的像素宽高比
    /// </summary>
    private float devicePixelRatioX;
    private float devicePixelRatioY;

    private List<AShape> _CurrentAIS = [];

    public List<AShape> SelectedAISList = [];

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

    #region 原点坐标系

    private bool _ShowGraduatedTrihedron;
    public bool ShowGraduatedTrihedron
    {
        get { return _ShowGraduatedTrihedron; }
        set
        {
            _ShowGraduatedTrihedron = value;
            DisplayGraduatedTrihedron();
        }
    }

    private void DisplayGraduatedTrihedron()
    {
        if (_ShowOriginTrihedron)
        {
            MainView.DisplayDefault_GraduatedTrihedron();
        }
        else
        {
            MainView.Hide_GraduatedTrihedron();
        }
    }

    #endregion

    #region 操作器

    private Manipulator myManipulator;

    private ManipulatorMode? _manipulatorMode;
    private ManipulatorAxisIndex? _manipulatorAxis;

    #endregion

    #endregion

    #region 属性

    /// <summary>
    /// AIS操作器
    /// </summary>
    public Manipulator Manipulator => myManipulator;

    /// <summary>
    /// 画布对象(设置灯光等)
    /// </summary>
    public Viewer Viewer => myViewer;

    /// <summary>
    /// 视图列表
    /// </summary>
    public List<View> ViewList { get; set; } = [];

    /// <summary>
    /// 主视图（第一个创建的视图）
    /// </summary>
    public View? MainView => ViewList.Count > 0 ? ViewList[0] : null;

    /// <summary>
    /// DebugC++委托的交互对象管理器
    /// </summary>
    public InteractiveContext AISContext => myAISContext;

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
                AISContext.SetDefaultSelectionMode(value);
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
            throw new Exception("OCC OCCCanvas is null");
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
        _currentAction3d = Action3d.None;
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
    private static T CycleEnum<T>(T currentEnum)
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
    public void SetDefaultSelectionMode(OCCTK.OCC.AIS.SelectionMode theMode)
    {
        AISContext.SetDefaultSelectionMode(theMode);
    }

    /// <summary>
    /// 获取选中的AIS对象
    /// </summary>
    /// <returns></returns>
    public AShape GetSelectedShape()
    {
        return AISContext.SelectedAIS();
    }

    #endregion

    #region 其它设置

    #endregion
}
