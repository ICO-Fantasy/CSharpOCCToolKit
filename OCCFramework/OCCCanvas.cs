using log4net;
using OCCTK.OCC.AIS;
using OCCTK.OCC.V3d;
using View = OCCTK.OCC.V3d.View;

namespace OCCFramework;

public partial class OCCCanvas : Form
{
    private static readonly ILog log = LogManager.GetLogger(typeof(OCCCanvas));

    public OCCCanvas(ThreeDimensionContext context)
    {
        ThreeDimensionContext = context;
        InitializeComponent();
        //! 删除From自带的边界
        FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
        //! ControlStyles.DoubleBuffer 双缓冲会导致画面绘制失效，不能启用
        SetStyle(ControlStyles.OptimizedDoubleBuffer, false); // 关闭优化双重缓冲
        //! 设置自定义绘制和防止擦除背景，避免拖动画布时画面闪动
        SetStyle(ControlStyles.AllPaintingInWmPaint, true); // 防止擦除背景。
        SetStyle(ControlStyles.UserPaint, true); // 使用自定义的绘制。
        //创建相机
        View = context.Viewer.CreateView();
        if (View == null)
        {
            //MessageBox.Show("图形初始化失败", "Error!");
            throw new Exception("图形初始化失败");
        }
        View.SetWindow(this.Handle);
        context.ViewList.Add(this);

        //创建操作器
        myManipulator = new();
        //创建选择框
        myBubberBand = new();
        //鼠标样式重置计时器
        cursorResetTimer = new System.Windows.Forms.Timer();
        //默认设置
        SetDefault();

        View.Redraw();
        View.MustBeResized();
    }

    #region 字段

    protected readonly ThreeDimensionContext ThreeDimensionContext;
    protected Viewer Viewer => ThreeDimensionContext.Viewer;
    protected InteractiveContext AISContext => ThreeDimensionContext.AISContext;

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

    #region 刻度坐标系

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
        if (_ShowGraduatedTrihedron)
        {
            View?.DisplayDefault_GraduatedTrihedron();
        }
        else
        {
            View?.Hide_GraduatedTrihedron();
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
    /// 主视图（第一个创建的视图）
    /// </summary>
    public View View { get; protected set; }

    #region 状态flag

    private bool _ShowGrid;

    /// <summary>
    /// 是否显示网格
    /// </summary>
    public bool ShowGrid
    {
        get { return _ShowGrid; }
        set { _ShowGrid = value; }
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
    public bool DegenerateMode { get; set; }

    /// <summary>
    /// 框线模式
    /// </summary>
    public bool IsWireframeEnabled { get; set; }

    /// <summary>
    /// 阴影模式
    /// </summary>
    public bool IsShadingEnabled { get; set; }

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
        if (View == null)
        {
            throw new Exception("View is null");
        }

        //设置相机默认值
        View.SetICORendering();
        //设置默认背景颜色
        View.SetDefaultBGColor();

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
        //显示刻度坐标系
        ShowGraduatedTrihedron = true;

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
        if (AISContext == null || View == null)
        {
            return;
        }

        View.Redraw();
        AISContext.UpdateCurrentViewer();
    }

    private void OnSizeChanged(object? sender, EventArgs e)
    {
        if (View == null)
        {
            return;
        }
        View.MustBeResized();
    }

    #endregion

    #endregion

    #region 视图控制

    public new void Update()
    {
        View.Redraw();
        AISContext.UpdateCurrentViewer();
    }

    /// <summary>
    /// 相机缩放至最佳尺寸
    /// </summary>
    public void FitAll()
    {
        View.FitAll(0.01, true);
    }

    /// <summary>
    /// 循环设置相机视图方向
    /// </summary>
    public void SetViewOrientation()
    {
        View.SetViewOrientation(CycleEnum(CurrentViewOrientation), true);
    }

    /// <summary>
    /// 设置相机视图方向为theMode
    /// </summary>
    /// <param name="theMode"></param>
    public void SetViewOrientation(ViewOrientation theMode)
    {
        CurrentViewOrientation = theMode;
        View.SetViewOrientation(theMode, true);
    }

    /// <summary>
    /// 重置视图的居中和方向
    /// </summary>
    public void Reset()
    {
        View.Reset();
    }

    /// <summary>
    /// 设置隐藏线模式
    /// </summary>
    /// <param name="theMode"></param>
    public void SetHidden(bool toOpen)
    {
        View.SetDegenerateMode(toOpen);
        DegenerateMode = toOpen;
    }

    #endregion
}
