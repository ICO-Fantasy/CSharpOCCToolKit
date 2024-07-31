using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Windows.Controls;
using System.Windows.Forms;
using OCCTK.OCC.AIS;
using OCCTK.Visualization;

namespace OCCViewForm;

public enum ActionMode
{
    Normal,
    Manipulator
}

public enum Action3d
{
    // Normal
    Normal_Nothing,
    Normal_SingleSelect,
    Normal_MultipleSelect,
    Normal_XORSelect,
    Normal_AreaSelect,
    Normal_MultipleAreaSelect,
    Normal_XORAreaSelect,
    Normal_AreaZooming,
    Normal_DynamicRotation,
    Normal_DynamicPanning,

    // Manipulator
    Manipulator_Nothing,
    Manipulator_SingleSelect,
    Manipulator_Translation
}

public enum DisplayMode
{
    Wireframe,
    Shading
}

public enum ViewOrientation
{
    Axo,
    Front,
    Back,
    Top,
    Bottom,
    Left,
    Right,
}

public class SingleManipulator
{
    // 私有静态变量用于保存单例实例
    private static WAIS_Manipulator _instance;
    private static readonly object _lock = new object();

    // 私有构造函数，防止外部实例化
    private SingleManipulator() { }

    // 公共静态属性，用于访问单例实例
    public static WAIS_Manipulator Instance
    {
        get
        {
            // 确保线程安全
            lock (_lock)
            {
                if (_instance == null)
                {
                    _instance = new WAIS_Manipulator();
                }
                return _instance;
            }
        }
    }
}

public class OCCCanvas : Form
{
    #region 属性

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
        { Action3d.Normal_Nothing, Cursors.Default },
        { Action3d.Normal_SingleSelect, Cursors.Hand },
        { Action3d.Normal_MultipleSelect, Cursors.Hand },
        { Action3d.Normal_XORSelect, Cursors.Default },
        { Action3d.Normal_AreaSelect, Cursors.Cross },
        { Action3d.Normal_MultipleAreaSelect, Cursors.Cross },
        { Action3d.Normal_XORAreaSelect, Cursors.Default },
        { Action3d.Normal_AreaZooming, Cursors.Default },
        { Action3d.Normal_DynamicRotation, new Cursor("rotation.cur") },
        { Action3d.Normal_DynamicPanning, Cursors.SizeAll },
        // Manipulator
        { Action3d.Manipulator_Nothing, Cursors.Cross },
        { Action3d.Manipulator_SingleSelect, Cursors.Hand },
        { Action3d.Manipulator_Translation, Cursors.SizeAll },
    };

    /// <summary>
    /// C++委托的视图对象
    /// </summary>
    public CSharpViewer Viewer { get; set; }

    /// <summary>
    /// C++委托的交互对象管理器
    /// </summary>
    public WAIS_InteractiveContext InteractiveContext { get; set; }

    /// <summary>
    /// AIS操作器
    /// </summary>
    private WAIS_Manipulator myManipulator = SingleManipulator.Instance;

    /// <summary>
    /// 重设滚轮光标计时器
    /// </summary>
    private System.Windows.Forms.Timer CursorResetTimer { get; set; }

    #region 状态flag
    // 缩放结束
    public event EventHandler ZoomingFinished;

    protected void RaiseZoomingFinished()
    {
        if (ZoomingFinished != null)
        {
            ZoomingFinished(this, EventArgs.Empty);
        }
    }

    //未知
    public event EventHandler AvaliabiltyOfOperationsChanged;

    protected void RaiseAvaliabiltyOfOperationsChanged()
    {
        if (AvaliabiltyOfOperationsChanged != null)
        {
            AvaliabiltyOfOperationsChanged(this, EventArgs.Empty);
        }
    }

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
            Viewer.DisplayGrid(_isActivateGrid);
        }
    }

    /// <summary>
    /// 当前操作模式
    /// </summary>
    public ActionMode CurrentActionMode;

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
    public ViewOrientation CurrentViewOrientation { get; private set; }

    /// <summary>
    /// 是否正在绘制选择框
    /// </summary>
    private bool IsDrawRect { get; set; }
    private bool IsRectVisible { get; set; }
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

    private int mouseDownX;
    private int mouseDownY;
    private int mouseCurrentX;
    private int mouseCurrentY;
    private int myButtonDownX;
    private int myButtonDownY;
    #endregion

    #endregion

    /// <summary>
    /// 构造函数
    /// </summary>
    public OCCCanvas()
    {
        InitializeComponent();
        //# ControlStyles.DoubleBuffer 双缓冲会导致画面绘制失效，不能启用
        //! 设置自定义绘制和防止擦除背景，避免拖动画布时画面闪动
        SetStyle(
            ControlStyles.AllPaintingInWmPaint // 防止擦除背景。
                | ControlStyles.UserPaint // 使用自定义的绘制。
            ,
            true
        );
        Viewer = new CSharpViewer();
        Viewer.InitOCCViewer();
        if (!Viewer.InitViewer(this.Handle))
        {
            MessageBox.Show("图形初始化失败", "Error!");
        }
        InteractiveContext = Viewer.CSharpAISContext;
        Viewer.SetSelectionMode(OCCTK.Visualization.SelectionMode.Shape);
        #region 设置鼠标样式
        CursorResetTimer = new System.Windows.Forms.Timer();
        CursorResetTimer.Interval = 500; // 设置计时器间隔为100毫秒
        CursorResetTimer.Tick += CursorResetTimer_Tick; // 绑定计时器的Tick事件处理方法
        #endregion
        CurrentActionMode = ActionMode.Normal;
        CurrentAction3d = Action3d.Normal_Nothing;
        IsActivateGrid = false;

        IsDrawRect = false;
        DegenerateMode = true;
        Viewer.DisplayViewCube(5);
        this.SetDisplayMode(DisplayMode.Shading);
        this.SetSelectionMode(OCCTK.Visualization.SelectionMode.Face);
    }

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

    /// <summary>
    /// 计时器触发后将光标恢复为默认光标
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void CursorResetTimer_Tick(object sender, EventArgs e)
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
    private int CycleEnum(Enum currentEnum)
    {
        int enumLength = Enum.GetNames(currentEnum.GetType()).Length;
        int intValue = Convert.ToInt16(currentEnum);

        if (intValue + 1 == enumLength)
        {
            return 0;
        }
        else
        {
            return intValue + 1;
        }
    }

    #region 重写虚方法
    private void OnPaint(object sender, PaintEventArgs e)
    {
        Viewer.RedrawView();
        Viewer.UpdateView();
    }

    private void OnSizeChanged(object sender, EventArgs e)
    {
        Viewer.UpdateView();
    }

    private void OnMouseDown(object sender, MouseEventArgs e)
    {
        // 获取鼠标按下的按钮和修改键
        //MouseButtons mouseButton = e.Button;
        //Keys modifiers = ModifierKeys;
        // 记录鼠标按下的位置
        mouseDownX = e.X;
        mouseDownY = e.Y;
        //! 开始旋转
        Viewer.StartRotation(e.X, e.Y);
        switch (CurrentActionMode)
        {
            case ActionMode.Normal:
            {
                // 默认设置为点击，如果鼠标移动了会替换掉默认模式
                CurrentAction3d = Action3d.Normal_SingleSelect;
                break;
            }
            case ActionMode.Manipulator:
            {
                if (myManipulator.HasActiveMode())
                {
                    // 记录开始变换的点
                    myManipulator.StartTransform(mouseDownX, mouseDownY, Viewer.GetMainView());
                    CurrentAction3d = Action3d.Manipulator_SingleSelect;
                }
                break;
            }
        }
    }

    private void OnMouseMove(object sender, MouseEventArgs e)
    {
        //! 使用if而不是switch判断，让点击事件具有更高优先级
        // 获取鼠标当前位置
        //Point pt = e.Location;
        // 获取鼠标按下的按钮和修改键
        MouseButtons mouseButton = e.Button;
        Keys modifiers = ModifierKeys;
        mouseCurrentX = e.X;
        mouseCurrentY = e.Y;

        var mouseState = System.Windows.Forms.Control.MouseButtons;
        switch (CurrentActionMode)
        {
            case ActionMode.Normal:
            {
                if (
                    (mouseState & (MouseButtons.Left | MouseButtons.Right))
                    == (MouseButtons.Left | MouseButtons.Right)
                ) // 同时按下了左键和右键
                { }
                else if (mouseButton == MouseButtons.Left)
                {
                    if (modifiers == Keys.None)
                    {
                        // 框选
                        CurrentAction3d = Action3d.Normal_AreaSelect;
                    }
                    else if (modifiers == Keys.Control)
                    {
                        // 多次框选
                        CurrentAction3d = Action3d.Normal_MultipleAreaSelect;
                    }
                }
                else if (mouseButton == MouseButtons.Right)
                {
                    if (modifiers == Keys.Shift)
                    {
                        // 框选区域放大
                        CurrentAction3d = Action3d.Normal_AreaZooming;
                    }
                }
                else if (mouseButton == MouseButtons.Middle)
                {
                    if (modifiers == Keys.None)
                    {
                        // 旋转
                        Viewer.Rotation(e.X, e.Y);
                        Viewer.RedrawView();
                        CurrentAction3d = Action3d.Normal_DynamicRotation;
                    }
                    else if (modifiers == Keys.Control)
                    {
                        // Dynamic Panning 表示交互式的平移画面与非交互的平移 Global Panning 相区别
                        // 平移
                        Viewer.Pan(mouseCurrentX - mouseDownX, mouseDownY - mouseCurrentY);
                        mouseDownX = mouseCurrentX;
                        mouseDownY = mouseCurrentY;
                        CurrentAction3d = Action3d.Normal_DynamicPanning;
                    }
                }
                else
                {
                    //将鼠标位置发送给OCC交互上下文管理器，用于获取该位置的所选对象
                    //! 单选等操作均需要基于该位置进行
                    Viewer.MoveTo(mouseCurrentX, mouseCurrentY);
                }
                break;
            }
            case ActionMode.Manipulator:
            {
                if (
                    (mouseState & (MouseButtons.Left | MouseButtons.Right))
                    == (MouseButtons.Left | MouseButtons.Right)
                ) // 同时按下了左键和右键
                { }
                else if (mouseButton == MouseButtons.Left)
                {
                    if (myManipulator.HasActiveMode())
                    {
                        // 操作器执行移动
                        myManipulator.Transform(mouseCurrentX, mouseCurrentY, Viewer.GetMainView());
                        Viewer.RedrawView();
                        CurrentAction3d = Action3d.Manipulator_Translation;
                    }
                }
                else
                {
                    //将鼠标位置发送给OCC交互上下文管理器，用于获取该位置的所选对象
                    //! 单选等操作均需要基于该位置进行
                    Viewer.MoveTo(mouseCurrentX, mouseCurrentY);
                }
                break;
            }
        }
        // 工厂类构建
        var interaction = CanvasInteractionFactory.CreateInteraction(this, CurrentAction3d);
        // 执行操作
        interaction.Interaction(mouseCurrentX, mouseCurrentY);
    }

    private void OnMouseUp(object sender, MouseEventArgs e)
    {
        // 获取鼠标按下的按钮和修改键
        MouseButtons mouseButton = e.Button;
        Keys modifiers = ModifierKeys;
        mouseCurrentX = e.X;
        mouseCurrentY = e.Y;

        switch (CurrentActionMode)
        {
            case ActionMode.Normal:
            {
                switch (CurrentAction3d)
                {
                    case Action3d.Normal_SingleSelect:
                        // 单选
                        Viewer.Select();
                        break;
                    case Action3d.Normal_MultipleSelect:
                        Viewer.XORSelect();
                        break;
                    case Action3d.Normal_AreaSelect:
                        if (
                            Math.Abs(mouseCurrentX - mouseDownX) > ZEROWIDTHMIN
                            && Math.Abs(mouseCurrentY - mouseDownY) > ZEROWIDTHMIN
                        )
                        {
                            // 移动了,结束单次框选
                            DrawRectangle(false);
                            DragEvent(mouseCurrentX, mouseCurrentY, 1);
                        }
                        else
                        {
                            // 单选
                            Viewer.Select();
                        }
                        break;
                    case Action3d.Normal_MultipleAreaSelect:
                        if (
                            Math.Abs(mouseCurrentX - mouseDownX) > ZEROWIDTHMIN
                            && Math.Abs(mouseCurrentY - mouseDownY) > ZEROWIDTHMIN
                        )
                        {
                            DrawRectangle(false);
                            // 移动了 结束连续框选
                            MultiDragEvent(mouseCurrentX, mouseCurrentY, 1);
                        }
                        else
                        {
                            Viewer.XORSelect();
                        }
                        break;
                    case Action3d.Normal_AreaZooming:
                        // 结束区域缩放
                        DrawRectangle(false);
                        if (
                            Math.Abs(mouseCurrentX - mouseDownX) > ZEROWIDTHMIN
                            && Math.Abs(mouseCurrentX - mouseCurrentY) > ZEROWIDTHMIN
                        )
                        {
                            Viewer.FitArea(mouseDownX, mouseDownY, mouseCurrentX, mouseCurrentY);
                        }
                        RaiseZoomingFinished();
                        break;
                    case Action3d.Normal_DynamicRotation:
                        //结束旋转
                        break;
                    case Action3d.Normal_DynamicPanning:
                        // 结束平移
                        break;
                    default:
                        break;
                }

                // 结束操作
                CurrentAction3d = Action3d.Normal_Nothing;
                break;
            }
            case ActionMode.Manipulator:
            {
                switch (CurrentAction3d)
                {
                    case Action3d.Manipulator_SingleSelect:
                        // 单选并将操作器到添加到选中
                        Viewer.Select();
                        InteractiveContext.InitSelected();
                        while (InteractiveContext.MoreSelected())
                        {
                            var a = InteractiveContext.SelectedInteractive();
                        }
                        break;
                    case Action3d.Manipulator_Translation:
                        // 结束拖动
                        myManipulator.StopTransform();
                        break;
                    case Action3d.Manipulator_Nothing:
                        break;
                    default:
                        break;
                }
                // 结束操作
                CurrentAction3d = Action3d.Manipulator_Nothing;
                break;
            }
        }

        // 恢复光标
        Cursor = Cursors.Default;
    }

    private void OnMouseWheel(object sender, MouseEventArgs e)
    {
        Keys modifiers = ModifierKeys;
        // 处理鼠标滚轮事件
        int delta = e.Delta; // 获取鼠标滚轮滚动的增量
        bool isScrollingUp = delta > 0;
        bool isScrollingDown = delta < 0;
        double zoomFactor = 1.0;
        // 根据需要执行相应操作
        if (isScrollingUp)
        {
            // 向上滚动
            Cursor = Cursors.PanNorth;
            zoomFactor = 2.0;
            if (modifiers == Keys.Shift)
            {
                // Shift键被按下时进行不同的缩放操作
                zoomFactor = 1.1;
            }
        }
        else if (isScrollingDown)
        {
            // 向下滚动
            Cursor = Cursors.PanSouth;
            zoomFactor = 0.5;
            if (modifiers == Keys.Shift)
            {
                // Shift键被按下时进行不同的缩放操作
                zoomFactor = 0.9;
            }
        }
        Viewer.Zoom(zoomFactor, true);
        // 启动计时器
        CursorResetTimer.Start();
    }

    private void OnKeyDown(object sender, KeyEventArgs e)
    {
        if (e.KeyCode == Keys.F)
        {
            this.FitAll();
        }
    }

    public interface ICanvasInteraction
    {
        void Interaction(int currentX, int currentY);
    }

    public class Nothing : ICanvasInteraction
    {
        public readonly OCCCanvas _C;

        public Nothing(OCCCanvas canvas) => _C = canvas;

        public void Interaction(int currentX, int currentY) { }
    }

    public class AreaSelect : ICanvasInteraction
    {
        public readonly OCCCanvas _C;

        public AreaSelect(OCCCanvas canvas) => _C = canvas;

        public void Interaction(int currentX, int currentY)
        {
            _C.DrawRectangle(false);
            _C.DrawRectangle(true);
            _C.Cursor = Cursors.Cross;
        }
    }

    public class MultipleAreaSelect : ICanvasInteraction
    {
        public readonly OCCCanvas _C;

        public MultipleAreaSelect(OCCCanvas canvas) => _C = canvas;

        public void Interaction(int currentX, int currentY)
        {
            _C.DrawRectangle(false);
            _C.DrawRectangle(true);
            _C.Cursor = Cursors.Cross;
        }
    }

    public class AreaZooming : ICanvasInteraction
    {
        public readonly OCCCanvas _C;

        public AreaZooming(OCCCanvas canvas) => _C = canvas;

        public void Interaction(int currentX, int currentY)
        {
            _C.DrawRectangle(false);
            _C.DrawRectangle(true);
            _C.Cursor = Cursors.Cross;
        }
    }

    public static class CanvasInteractionFactory
    {
        public static ICanvasInteraction CreateInteraction(OCCCanvas canvas, Action3d mode)
        {
            return mode switch
            {
                //Action3d.SingleSelect => new SingleSelect(canvas),
                Action3d.Normal_Nothing
                    => new Nothing(canvas),
                Action3d.Normal_AreaSelect => new AreaSelect(canvas),
                Action3d.Normal_MultipleAreaSelect => new MultipleAreaSelect(canvas),
                Action3d.Normal_AreaZooming => new AreaZooming(canvas),
                _ => new Nothing(canvas),
            };
        }

        //public static ICanvasInteraction CreateInteraction(OCCCanvas canvas, string mode)
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
    /// <summary>
    /// 相机缩放至最佳尺寸
    /// </summary>
    public void FitAll()
    {
        Viewer.ZoomAllView();
    }

    /// <summary>
    /// 循环设置相机视图方向
    /// </summary>
    public void SetViewOrientation()
    {
        Viewer.SetViewOrientation(CycleEnum(CurrentViewOrientation));
    }

    /// <summary>
    /// 设置相机视图方向为theMode
    /// </summary>
    /// <param name="theMode"></param>
    public void SetViewOrientation(ViewOrientation theMode)
    {
        CurrentViewOrientation = theMode;
        Viewer.SetViewOrientation((int)theMode);
    }

    /// <summary>
    /// 重置视图的居中和方向
    /// </summary>
    public void Reset()
    {
        Viewer.Reset();
    }

    /// <summary>
    /// 设置隐藏线模式
    /// </summary>
    /// <param name="theMode"></param>
    public void SetHidden(bool theMode)
    {
        Viewer.SetDegenerateMode(theMode);
        DegenerateMode = theMode;
    }

    /// <summary>
    /// 依次切换显示模式
    /// </summary>
    public void SetDisplayMode()
    {
        Viewer.SetDisplayMode(CycleEnum(CurrentDisplayMode));
        Viewer.UpdateCurrentViewer();

        RaiseZoomingFinished();
    }

    /// <summary>
    /// 切换到指定显示模式
    /// </summary>
    /// <param name="theMode"></param>
    public void SetDisplayMode(DisplayMode theMode)
    {
        CurrentDisplayMode = theMode;
        Viewer.SetDisplayMode((int)theMode);
        Viewer.UpdateCurrentViewer();

        RaiseZoomingFinished();
    }
    #endregion
    #region 显示
    public void Display(WAIS_Shape theAIS, bool Toupdate)
    {
        InteractiveContext.Display(theAIS, Toupdate);
    }

    public void EraseSelect()
    {
        InteractiveContext.EraseObjects();
    }

    public void Erase(WAIS_Shape theAIS, bool Toupdate)
    {
        InteractiveContext.Erase(theAIS, Toupdate);
    }

    public void EraseAll()
    {
        InteractiveContext.EraseAll();
    }

    #endregion
    #region 对象交互
    #region 选择框
    /// <summary>
    /// 连续框选
    /// </summary>
    /// <param name="x"></param>
    /// <param name="y"></param>
    /// <param name="theState"></param>
    protected void MultiDragEvent(int x, int y, int theState)
    {
        if (theState == -1) //mouse is down
        {
            myButtonDownX = x;
            myButtonDownY = y;
        }
        else if (theState == 1) //mouse is up
        {
            Viewer.MultipleAreaSelect(
                Math.Min(myButtonDownX, x),
                Math.Min(myButtonDownY, y),
                Math.Max(myButtonDownX, x),
                Math.Max(myButtonDownY, y)
            ); //连续选择，只添加
        }
    }

    /// <summary>
    /// 框选
    /// </summary>
    /// <param name="x"></param>
    /// <param name="y"></param>
    /// <param name="theState"></param>
    protected void DragEvent(int x, int y, int theState)
    {
        if (theState == -1) // mouse is down
        {
            myButtonDownX = x;
            myButtonDownY = y;
        }
        else if (theState == 1) // mouse is up
        {
            Viewer.AreaSelect(
                Math.Min(myButtonDownX, x),
                Math.Min(myButtonDownY, y),
                Math.Max(myButtonDownX, x),
                Math.Max(myButtonDownY, y)
            );
        }
    }

    /// <summary>
    /// 绘制选择框
    /// </summary>
    /// <param name="draw"></param>
    private void DrawRectangle(bool draw)
    {
        System.Drawing.Graphics gr = System.Drawing.Graphics.FromHwnd(Handle);
        System.Drawing.Pen? p = null;
        if (IsRectVisible || !draw) //erase the rect
        {
            //p = new System.Drawing.Pen(System.Drawing.Color.Black);
            IsRectVisible = false;
            Viewer.UpdateView();
        }
        else if (draw)
        {
            p = new System.Drawing.Pen(System.Drawing.Color.White);
            IsRectVisible = true;
        }
        if (p == null)
        {
            return;
        }
        int x = Math.Min(mouseDownX, mouseCurrentX);
        int y = Math.Min(mouseDownY, mouseCurrentY);
        gr.DrawRectangle(
            p,
            x,
            y,
            Math.Abs(mouseCurrentX - mouseDownX),
            Math.Abs(mouseCurrentY - mouseDownY)
        );
    }
    #endregion
    public void SetSelectionMode(OCCTK.Visualization.SelectionMode theMode)
    {
        Viewer.SetSelectionMode(theMode);
    }

    public WAIS_Shape GetSelectedShape()
    {
        WAIS_Shape result = Viewer.GetSelectedAIS();
        return result;
    }

    public void Attach(WAIS_Shape theAIS)
    {
        myManipulator.Attach(theAIS);
        CurrentActionMode = ActionMode.Manipulator;
    }

    #endregion
    #region 其它设置

    #endregion
}
