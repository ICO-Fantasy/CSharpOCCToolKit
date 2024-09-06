using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq.Expressions;
using System.Windows.Forms;
using OCCTK.OCC;
using OCCTK.OCC.AIS;
using OCCTK.OCC.gp;
using OCCTK.OCC.TopoDS;
using OCCTK.OCC.V3d;
using OCCTK.Visualization;

namespace OCCViewForm;

#region Enum

public enum ActionMode
{
    Normal,
    Manipulator
}

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

public enum DisplayMode
{
    Wireframe,
    Shading
}

#endregion

#region 接口

//public interface IAISSelectionHandler
//{
//    void OnAISSelectionMade(AShape theAIS);
//}

public delegate void AISSelectionMadeHandler(AShape theAIS);

//public interface IMouseMoveHandler
//{
//    void OnMouseMoved(int X, int Y);
//}

public delegate void MouseMovedHandler(int X, int Y);

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

public class OCCCanvas : Form
{
    // 定义事件
    public event AISSelectionMadeHandler OnAISSelected;
    public event MouseMovedHandler OnMouseMoved;

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
            MessageBox.Show("图形初始化失败", "Error!");
        }

        //获取视图对象
        Viewer = Canvas.GetViewer();
        //获取上下文管理器对象
        AISContext = Canvas.GetContext();

        SetDefault();

        MainView.Redraw();
        MainView.MustBeResized();
    }

    #region 字段

    /// <summary>
    /// 框选零宽度的最小值
    /// </summary>
    private const int ZEROWIDTHMIN = 1;

    /// <summary>
    /// 交互动作对应的鼠标指针
    /// </summary>
    public Dictionary<Action3d, Cursor> CURSORS = new Dictionary<Action3d, Cursor>
    {
        // Manipulator
        { Action3d.Nothing, Cursors.Default },
        { Action3d.SingleSelect, Cursors.Hand },
        { Action3d.MultipleSelect, Cursors.Hand },
        { Action3d.XORSelect, Cursors.Default },
        { Action3d.AreaSelect, Cursors.Cross },
        { Action3d.MultipleAreaSelect, Cursors.Cross },
        { Action3d.XORAreaSelect, Cursors.Default },
        { Action3d.AreaZooming, Cursors.Default },
        { Action3d.DynamicRotation, Cursors.NoMove2D },
        { Action3d.DynamicPanning, Cursors.SizeAll },
        { Action3d.Prohibition, Cursors.No },
        // Manipulator
        { Action3d.Manipulator_Translation, Cursors.SizeAll },
    };

    float devicePixelRatioX;
    float devicePixelRatioY;

    private List<AShape> _CurrentAIS = new List<AShape>();

    public List<AShape> SelectedAISList = new List<AShape>();

    #region 视图立方

    private ViewCube _ViewCube;
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
        if (_ViewCube == null)
        {
            _ViewCube = new(5.0);
        }
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

    private Trihedron _ViewTrihedron;
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

    private Trihedron _OriginTrihedron;
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
        if (_OriginTrihedron == null)
        {
            _OriginTrihedron = new(100);
        }
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

    private bool _Detected = false;

    public bool ScrollMode = true; //true表示根据鼠标点缩放，false表示根据中心点缩放

    /// <summary>
    /// 选择框
    /// </summary>
    private RubberBand myBubberBand;

    /// <summary>
    /// AIS操作器
    /// </summary>
    private readonly Manipulator myManipulator = SingleManipulator.Instance;

    private Trsf? myManipulatorTransfrom;
    private ManipulatorMode _manipulatorMode = ManipulatorMode.None;
    private ManipulatorAxisIndex _ManipulatorAxis = ManipulatorAxisIndex.None;

    private int mouseDownX;
    private int mouseDownY;
    private int mouseCurrentX;
    private int mouseCurrentY;
    private int mouseUpX;
    private int mouseUpY;

    //todo
    private int PanX;
    private int PanY;

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

    /// <summary>
    /// 重设滚轮光标计时器
    /// </summary>
    private System.Windows.Forms.Timer CursorResetTimer { get; set; }

    #region 状态flag

    private bool _isActivateGrid;

    /// <summary>
    /// 是否显示网格
    /// </summary>
    public bool IsActivateGrid
    {
        get { return _isActivateGrid; }
        set
        {
            _isActivateGrid = value;
            //todo
        }
    }

    private Action3d _currentAction3d;

    /// <summary>
    /// 当前交互动作
    /// </summary>
    public Action3d CurrentAction3d
    {
        get => _currentAction3d;
        set
        {
            _currentAction3d = value;
            if (CURSORS.TryGetValue(value, out Cursor cursor))
            {
                Cursor = cursor;
            }
        }
    }

    /// <summary>
    /// 当前显示模式
    /// </summary>
    public DisplayMode CurrentDisplayMode { get; private set; }

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

    //无效定义
    //public bool IsTransparencyEnabled { get; private set; }
    //public bool IsColorEnabled { get; private set; }
    //public bool IsMaterialEnabled { get; private set; }
    //public bool IsDeleteEnabled { get; private set; }

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

        MouseDown += new System.Windows.Forms.MouseEventHandler(OnMouseDown);
        MouseUp += new System.Windows.Forms.MouseEventHandler(OnMouseUp);
        MouseMove += new System.Windows.Forms.MouseEventHandler(OnMouseMove);
        MouseWheel += new System.Windows.Forms.MouseEventHandler(OnMouseWheel);

        //todo 键盘事件未生效
        KeyPreview = true;
        KeyDown += new System.Windows.Forms.KeyEventHandler(OnKeyDown);
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
        Viewer.SetDefaultLight();
        //设置交互默认值
        AISContext.SetDefault();
        //设置选择模式
        AISContext.SetSelectionMode(OCCTK.OCC.AIS.SelectionMode.Shape);
        //设置相机默认值
        MainView.SetDefault();

        float dpiX = CreateGraphics().DpiX;
        float dpiY = CreateGraphics().DpiY;

        // 计算设备像素比
        devicePixelRatioX = dpiX / 96.0f;
        devicePixelRatioY = dpiY / 96.0f;

        //设置选择框样式
        myBubberBand = new();
        myBubberBand.SetLineType(OCCTK.OCC.Aspect.Line.Solid);
        myBubberBand.SetLineColor(new OCCTK.Extension.Color(125, 0, 0));
        myBubberBand.SetFilling(true);
        myBubberBand.SetFillTransparency(0.9);
        myBubberBand.SetFillColor(new OCCTK.Extension.Color(0, 0, 125));

        #region 设置鼠标样式

        CursorResetTimer = new System.Windows.Forms.Timer();
        CursorResetTimer.Interval = 500; // 设置计时器间隔为100毫秒
        CursorResetTimer.Tick += CursorResetTimer_Tick; // 绑定计时器的Tick事件处理方法
        #endregion

        CurrentAction3d = Action3d.Nothing;
        IsActivateGrid = false;

        DegenerateMode = true;

        ShowViewCube = true;
        ShowOriginTrihedron = false;
        ShowViewTrihedron = false;

        MainView.DisplayDefault_GraduatedTrihedron();
    }

    /// <summary>
    /// 计时器触发后将光标恢复为默认光标
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void CursorResetTimer_Tick(object? sender, EventArgs e)
    {
        Cursor = System.Windows.Forms.Cursors.Default;
        // 停止计时器
        CursorResetTimer.Stop();
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
        if (AISContext == null)
        {
            return;
        }
        if (MainView == null)
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

    #region 鼠标

    private void OnMouseDown(object? sender, MouseEventArgs e)
    {
        // 获取鼠标按下的按钮和修改键
        //MouseButtons mouseButton = e.Button;
        //Keys modifiers = ModifierKeys;
        // 记录鼠标按下的位置
        mouseDownX = e.X;
        mouseDownY = e.Y;
        PanX = mouseDownX;
        PanY = mouseDownY;
        //默认为点击操作
        CurrentAction3d = Action3d.SingleSelect;
        //鼠标按下的时候执行检测
        AISContext.InitDetected();
        _Detected = AISContext.MoreDetected();
        myBubberBand.SetRectangle(0, 0, 0, 0);
        //! 记录旋转开始点
        MainView.StartRotation(mouseDownX, mouseDownY);
        //! 记录变换开始点
        if (myManipulator.HasActiveMode())
        {
            myManipulator.StartTransform(mouseDownX, mouseDownY, Canvas.GetMainView());
        }
    }

    private void OnMouseMove(object? sender, MouseEventArgs e)
    {
        //! 键盘响应一定要获得焦点
        this.Focus();
        //! 使用if而不是switch判断，让点击事件具有更高优先级
        // 获取鼠标当前位置
        //Point pt = e.Location;
        // 获取鼠标按下的按钮和修改键
        MouseButtons mouseButton = e.Button;
        Keys modifiers = ModifierKeys;
        mouseCurrentX = e.X;
        mouseCurrentY = e.Y;
        OnMouseMoved?.Invoke(e.X, e.Y);

        var mouseState = System.Windows.Forms.Control.MouseButtons;
        AISContext.InitSelected();

        //! 同时按下了左键和右键
        if (
            (mouseState & (MouseButtons.Left | MouseButtons.Right))
            == (MouseButtons.Left | MouseButtons.Right)
        ) { }
        //! 左键
        else if (mouseButton == MouseButtons.Left)
        {
            if (_Detected)
            {
                // 操作器执行移动
                if (myManipulator.HasActiveMode())
                {
                    if (_manipulatorMode == ManipulatorMode.None)
                    {
                        _manipulatorMode = myManipulator.ActiveMode();
                    }
                    if (_ManipulatorAxis == ManipulatorAxisIndex.None)
                    {
                        _ManipulatorAxis = myManipulator.ActiveAxisIndex();
                    }
                    if (
                        myManipulator.ActiveMode() == _manipulatorMode
                        && myManipulator.ActiveAxisIndex() == _ManipulatorAxis
                    )
                    {
                        myManipulatorTransfrom = myManipulator.Transform(
                            mouseCurrentX,
                            mouseCurrentY,
                            MainView
                        );
                        MainView.Redraw();
                        CurrentAction3d = Action3d.Manipulator_Translation;
                    }
                }
                //不执行动作
                else if (AISContext.IsSelected())
                {
                    CurrentAction3d = Action3d.Prohibition;
                }
                else
                {
                    CurrentAction3d = Action3d.AreaSelect;
                }
            }
            // 框选
            else if (modifiers == Keys.None)
            {
                CurrentAction3d = Action3d.AreaSelect;
            }
            // 多次框选
            else if (modifiers == Keys.Control)
            {
                CurrentAction3d = Action3d.MultipleAreaSelect;
            }
        }
        //! 右键
        else if (mouseButton == MouseButtons.Right)
        {
            // 框选区域放大
            if (modifiers == Keys.Shift)
            {
                CurrentAction3d = Action3d.AreaZooming;
            }
        }
        //! 中键
        else if (mouseButton == MouseButtons.Middle)
        {
            // 旋转
            if (modifiers == Keys.None)
            {
                MainView.Rotation(e.X, e.Y);
                MainView.Redraw();
                CurrentAction3d = Action3d.DynamicRotation;
            }
            // Dynamic Panning 表示交互式的平移画面与非交互的平移 Global Panning 相区别
            // 平移
            else if (modifiers == Keys.Control)
            {
                MainView.Pan(mouseCurrentX - PanX, PanY - mouseCurrentY);
                PanX = mouseCurrentX;
                PanY = mouseCurrentY;
                CurrentAction3d = Action3d.DynamicPanning;
            }
        }
        else
        {
            //将鼠标位置发送给OCC交互上下文管理器，用于获取该位置的所选对象
            //! 单选等操作均需要基于该位置进行
            //! moveto的坐标是基于pix的，不需要做转换
            AISContext.MoveTo(mouseCurrentX, mouseCurrentY, MainView);
        }
        // 工厂类构建
        var interaction = MouseMoveFactory.CreateInteraction(this, CurrentAction3d);
        // 执行操作
        interaction.Execute(mouseCurrentX, mouseCurrentY);
    }

    private void OnMouseUp(object? sender, MouseEventArgs e)
    {
        // 获取鼠标按下的按钮和修改键
        MouseButtons mouseButton = e.Button;
        Keys modifiers = ModifierKeys;
        mouseUpX = e.X;
        mouseUpY = e.Y;

        switch (CurrentAction3d)
        {
            case Action3d.SingleSelect:
                // 单选
                AISContext.Select();
                if (AISContext.IsSelected())
                {
                    // 触发事件，调用所有订阅的方法
                    OnAISSelected?.Invoke(AISContext.SelectedAIS());
                }
                break;
            case Action3d.MultipleSelect:
                AISContext.XORSelect();
                break;
            case Action3d.AreaSelect:
                if (
                    Math.Abs(mouseCurrentX - mouseDownX) > ZEROWIDTHMIN
                    && Math.Abs(mouseCurrentY - mouseDownY) > ZEROWIDTHMIN
                )
                {
                    // 移动了,结束单次框选
                    //! 框选是基于pix，不需要做处理
                    AISContext.AreaSelect(
                        Math.Min(mouseDownX, mouseUpX),
                        Math.Min(mouseDownY, mouseUpY),
                        Math.Max(mouseDownX, mouseUpX),
                        Math.Max(mouseDownY, mouseUpY),
                        MainView
                    );
                }
                else
                {
                    // 单选
                    AISContext.Select();
                }
                break;
            case Action3d.MultipleAreaSelect:
                if (
                    Math.Abs(mouseCurrentX - mouseDownX) > ZEROWIDTHMIN
                    && Math.Abs(mouseCurrentY - mouseDownY) > ZEROWIDTHMIN
                )
                {
                    // 移动了 结束连续框选
                    AISContext.MultipleAreaSelect(
                        Math.Min(mouseDownX, mouseUpX),
                        Math.Min(mouseDownY, mouseUpY),
                        Math.Max(mouseDownX, mouseUpX),
                        Math.Max(mouseDownY, mouseUpY),
                        MainView
                    ); //连续选择，只添加
                }
                else
                {
                    AISContext.XORSelect();
                }
                break;
            case Action3d.AreaZooming:
                // 结束区域缩放
                AISContext.Erase(myBubberBand, true);
                if (
                    Math.Abs(mouseCurrentX - mouseDownX) > ZEROWIDTHMIN
                    && Math.Abs(mouseCurrentX - mouseCurrentY) > ZEROWIDTHMIN
                )
                {
                    MainView.WindowFitAll(mouseDownX, mouseDownY, mouseCurrentX, mouseCurrentY);
                }
                break;
            case Action3d.DynamicRotation:
                //结束旋转
                break;
            case Action3d.DynamicPanning:
                // 结束平移
                break;
            case Action3d.Manipulator_Translation:
                // 结束拖动
                myManipulator.StopTransform();
                break;
            default:
                break;
        }

        // 结束操作
        if (AISContext.IsDisplayed(myBubberBand))
        {
            Erase(myBubberBand, true);
        }
        CurrentAction3d = Action3d.Nothing;
        _manipulatorMode = ManipulatorMode.None;
        _ManipulatorAxis = ManipulatorAxisIndex.None;
        _Detected = false;

        // 恢复光标
        Cursor = Cursors.Default;
    }

    private void OnMouseWheel(object? sender, MouseEventArgs e)
    {
        Keys modifiers = ModifierKeys;
        // 处理鼠标滚轮事件
        int delta = e.Delta; // 获取鼠标滚轮滚动的增量
        int zoomDistance = 10;
        mouseCurrentX = e.X;
        mouseCurrentY = e.Y;
        int endX = e.X;
        int endY = e.Y;

        double zoomFactor = 1.0;
        // 根据需要执行相应操作

        if (delta > 0)
        {
            // 向上滚动
            Cursor = Cursors.PanNorth;
            zoomFactor = 2.0;
            endX += zoomDistance;
            endY += zoomDistance;
            if (modifiers == Keys.Shift)
            {
                // Shift键被按下时进行不同的缩放操作
                zoomFactor = 1.1;
                endX += (int)(zoomDistance * 0.1);
                endY += (int)(zoomDistance * 0.1);
            }
        }
        else
        {
            // 向下滚动
            Cursor = Cursors.PanSouth;
            zoomFactor = 0.5;
            endX -= zoomDistance;
            endY -= zoomDistance;
            if (modifiers == Keys.Shift)
            {
                // Shift键被按下时进行不同的缩放操作
                zoomFactor = 0.9;
                endX -= (int)(zoomDistance * 0.1);
                endY -= (int)(zoomDistance * 0.1);
            }
        }
        if (ScrollMode)
        {
            MainView.StartZoomAtPoint(
                devicePixelRatioX * mouseCurrentX,
                devicePixelRatioY * mouseCurrentY
            );
            MainView.ZoomAtPoint(mouseCurrentX, mouseCurrentY, endX, endY);
        }
        else
        {
            MainView.SetZoom(zoomFactor, true);
        }
        // 启动计时器
        CursorResetTimer.Start();
    }

    #endregion

    /// <summary>
    /// 键盘事件
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void OnKeyDown(object? sender, KeyEventArgs e)
    {
        if (e.KeyCode == Keys.F)
        {
            this.FitAll();
        }
    }

    #region 执行逻辑

    public interface IMouseMove
    {
        void Execute(int currentX, int currentY);
    }

    public class Nothing : IMouseMove
    {
        public readonly OCCCanvas _C;

        public Nothing(OCCCanvas canvas) => _C = canvas;

        public void Execute(int currentX, int currentY) { }
    }

    public class AreaSelect : IMouseMove
    {
        public readonly OCCCanvas _C;

        public AreaSelect(OCCCanvas canvas) => _C = canvas;

        public void Execute(int currentX, int currentY)
        {
            _C.DrawRectangle();
            _C.Cursor = Cursors.Cross;
        }
    }

    public class MultipleAreaSelect : IMouseMove
    {
        public readonly OCCCanvas _C;

        public MultipleAreaSelect(OCCCanvas canvas) => _C = canvas;

        public void Execute(int currentX, int currentY)
        {
            _C.DrawRectangle();
            _C.Cursor = Cursors.Cross;
        }
    }

    public class AreaZooming : IMouseMove
    {
        public readonly OCCCanvas _C;

        public AreaZooming(OCCCanvas canvas) => _C = canvas;

        public void Execute(int currentX, int currentY)
        {
            _C.DrawRectangle();
            _C.Cursor = Cursors.Cross;
        }
    }

    public static class MouseMoveFactory
    {
        public static IMouseMove CreateInteraction(OCCCanvas canvas, Action3d mode)
        {
            return mode switch
            {
                //Action3d.SingleSelect => new SingleSelect(canvas),
                Action3d.Nothing
                    => new Nothing(canvas),
                Action3d.AreaSelect => new AreaSelect(canvas),
                Action3d.MultipleAreaSelect => new MultipleAreaSelect(canvas),
                Action3d.AreaZooming => new AreaZooming(canvas),
                _ => new Nothing(canvas),
            };
        }

        //public static IMouseMove CreateInteraction(OCCCanvas canvas, string mode)
        //{
        //    return mode switch
        //    {
        //        "SingleSelect" => new SingleSelect(canvas),
        //        "AreaSelect" => new AreaSelect(canvas),
        //        "MultipleAreaSelect" => new MultipleAreaSelect(canvas),
        //        _ => throw new ArgumentException("Invalid interaction mode", nameof(mode)),
        //    };
        //}
    }

    #endregion

    #endregion

    #region 设置按键
    //设置未成功
    //private void OCCViewer_KeyDown(object sender, KeyEventArgs e)
    //{
    //    if (e.KeyCode == Keys.F) // 如果按下 F 键
    //    {
    //        this.FitAll();
    //    }
    //}
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

    public void Display(InteractiveObject theAIS, bool Toupdate)
    {
        AISContext.Display(theAIS, Toupdate);
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
        AISContext.EraseAll(Toupdate);
        DisplayViewCube(Toupdate);
        DisplayOriginTrihedron(Toupdate);
        DisplayViewTrihedron(Toupdate);
    }

    public void Remove(InteractiveObject theAIS, bool Toupdate)
    {
        AISContext.Remove(theAIS, Toupdate);
    }

    #endregion

    #region 对象交互

    #region 选择框

    /// <summary>
    /// 绘制选择框
    /// </summary>
    /// <param name="draw"></param>
    private void DrawRectangle()
    {
        myBubberBand.SetRectangle(
            Math.Min(mouseDownX, mouseCurrentX),
            Math.Min(Height - mouseDownY, Height - mouseCurrentY),
            Math.Max(mouseDownX, mouseCurrentX),
            Math.Max(Height - mouseDownY, Height - mouseCurrentY)
        );
        if (AISContext.IsDisplayed(myBubberBand))
        {
            AISContext.Redisplay(myBubberBand, true);
        }
        else
        {
            AISContext.Display(myBubberBand, true);
        }
    }

    #endregion

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

    public void Attach(InteractiveObject theAIS)
    {
        myManipulator.Attach(theAIS, true, true, true);
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

    public void ResetManipulatorPart()
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
