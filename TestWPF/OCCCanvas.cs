using System;
using System.Collections.Generic;
using System.Windows.Controls;
using System.Windows.Forms;
using OCCTK.OCC.AIS;
using OCCTK.Visualization;

namespace OCCViewForm;

public enum Action3d
{
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
        { Action3d.SingleSelect, Cursors.Hand },
        { Action3d.MultipleSelect, Cursors.Hand },
        { Action3d.XORSelect, Cursors.Default },
        { Action3d.AreaSelect, Cursors.Cross },
        { Action3d.MultipleAreaSelect, Cursors.Cross },
        { Action3d.XORAreaSelect, Cursors.Default },
        { Action3d.AreaZooming, Cursors.Default },
        { Action3d.DynamicRotation, new Cursor("rotation.cur") },
        { Action3d.DynamicPanning, Cursors.SizeAll },
    };

    /// <summary>
    /// C++委托的视图对象
    /// </summary>
    public CSharpViewer viewer { get; set; }

    /// <summary>
    /// 重设滚轮光标计时器
    /// </summary>
    private System.Windows.Forms.Timer cursorResetTimer { get; set; }

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
            viewer.DisplayGrid(_isActivateGrid);
        }
    }

    /// <summary>
    /// 当前选择模式
    /// </summary>
    private Action3d _currentAction3d;
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
    public bool degenerateMode { get; private set; }

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
    private int myCurrentX;
    private int myCurrentY;
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
        viewer = new CSharpViewer();
        viewer.InitOCCViewer();
        if (!viewer.InitViewer(this.Handle))
        {
            MessageBox.Show("图形初始化失败", "Error!");
        }
        viewer.SetSelectionMode(OCCTK.Visualization.SelectionMode.Shape);
        // 初始化计时器
        cursorResetTimer = new System.Windows.Forms.Timer();
        cursorResetTimer.Interval = 500; // 设置计时器间隔为100毫秒
        cursorResetTimer.Tick += CursorResetTimer_Tick; // 绑定计时器的Tick事件处理方法
        CurrentAction3d = Action3d.Nothing;
        IsActivateGrid = false;

        IsDrawRect = false;
        degenerateMode = true;
        viewer.DisplayViewCube(5);
        this.SetDisplayMode(DisplayMode.Shading);
        this.SetSelectionMode(OCCTK.Visualization.SelectionMode.Face);
    }

    private void InitializeComponent()
    {
        ControlBox = false;
        TopLevel = false;

        this.ImeMode = System.Windows.Forms.ImeMode.NoControl;

        SizeChanged += new System.EventHandler(OnSizeChanged);
        Paint += new System.Windows.Forms.PaintEventHandler(OnPaint);

        MouseDown += new System.Windows.Forms.MouseEventHandler(OnMouseDown);
        MouseUp += new System.Windows.Forms.MouseEventHandler(OnMouseUp);
        MouseMove += new System.Windows.Forms.MouseEventHandler(OnMouseMove);
        // Form基类中包含OnMouseWheel方法，不需要在此注册
        //KeyDown += new System.Windows.Forms.KeyEventHandler(OnKeyDown);
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
        cursorResetTimer.Stop();
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
        viewer.RedrawView();
        viewer.UpdateView();
    }

    private void OnSizeChanged(object sender, EventArgs e)
    {
        viewer.UpdateView();
    }

    private void OnMouseDown(object sender, MouseEventArgs e)
    {
        // 获取鼠标按下的按钮和修改键
        MouseButtons mouseButton = e.Button;
        Keys modifiers = ModifierKeys;
        mouseDownX = e.X;
        //myCurrentX = e.X;
        mouseDownY = e.Y;
        //myCurrentY = e.Y;
        // 开始旋转
        viewer.StartRotation(e.X, e.Y);
        //默认设置为点击，如果鼠标移动了会替换掉默认模式
        CurrentAction3d = Action3d.SingleSelect;
        #region 以下代码为了实现按钮切换鼠标交互模式，暂不考虑
        //    if (mouseButton == MouseButtons.Left)
        //    {

        //        if (modifiers == Keys.Control)
        //        {
        //            CurrentMode = CurrentAction3d.DynamicZooming;
        //        }
        //        else
        //        {
        //            switch (CurrentMode)
        //            {
        //                case CurrentAction3d.Nothing:
        //                    if (modifiers == Keys.Shift)
        //                    {
        //                        MultiDragEvent(myCurrentX, myCurrentY, -1);
        //                    }
        //                    else
        //                    {
        //                        DragEvent(myCurrentX, myCurrentY, -1);
        //                    }
        //                    break;
        //                case CurrentAction3d.DynamicRotation:
        //                    view.StartRotation(e.X, e.Y);
        //                    break;
        //                case CurrentAction3d.AreaZooming:
        //                    Cursor = Cursors.Cross;
        //                    break;
        //                default:
        //                    break;
        //            }
        //        }
        //}
        #endregion
    }

    private void OnMouseMove(object sender, MouseEventArgs e)
    {
        //! 使用if而不是switch判断，让点击事件具有更高优先级
        // 获取鼠标当前位置
        //Point pt = e.Location;
        // 获取鼠标按下的按钮和修改键
        MouseButtons mouseButton = e.Button;
        Keys modifiers = ModifierKeys;
        myCurrentX = e.X;
        myCurrentY = e.Y;

        var mouseState = System.Windows.Forms.Control.MouseButtons;
        if (
            (mouseState & (MouseButtons.Left | MouseButtons.Right))
            == (MouseButtons.Left | MouseButtons.Right)
        )
        {
            // 同时按下了左键和右键
        }
        else if (mouseButton == MouseButtons.Left)
        {
            if (modifiers == Keys.None)
            {
                // 框选
                CurrentAction3d = Action3d.AreaSelect;
            }
            else if (modifiers == Keys.Control)
            {
                // 多次框选
                CurrentAction3d = Action3d.MultipleAreaSelect;
            }
        }
        else if (mouseButton == MouseButtons.Right)
        {
            if (modifiers == Keys.Shift)
            {
                // 框选区域放大
                CurrentAction3d = Action3d.AreaZooming;
            }
        }
        else if (mouseButton == MouseButtons.Middle)
        {
            if (modifiers == Keys.None)
            {
                // 旋转
                viewer.Rotation(e.X, e.Y);
                viewer.RedrawView();
                CurrentAction3d = Action3d.DynamicRotation;
                Cursor = new Cursor("rotation.cur");
            }
            else if (modifiers == Keys.Control)
            {
                // Dynamic Panning 表示交互式的平移画面与非交互的平移 Global Panning 相区别
                // 平移
                myCurrentX = e.X;
                myCurrentY = e.Y;
                viewer.Pan(myCurrentX - mouseDownX, mouseDownY - myCurrentY);
                mouseDownX = myCurrentX;
                mouseDownY = myCurrentY;
                CurrentAction3d = Action3d.DynamicPanning;
                Cursor = Cursors.SizeAll;
            }
        }
        else
        {
            //将鼠标位置发送给OCC交互上下文管理器，用于获取该位置的所选对象
            //! 单选等操作均需要基于该位置进行
            viewer.MoveTo(myCurrentX, myCurrentY);
        }
        var interaction = CanvasInteractionFactory.CreateInteraction(this, CurrentAction3d);
        interaction.Interaction(myCurrentX, myCurrentY);
    }

    private void OnMouseUp(object sender, MouseEventArgs e)
    {
        // 获取鼠标按下的按钮和修改键
        MouseButtons mouseButton = e.Button;
        Keys modifiers = ModifierKeys;
        switch (CurrentAction3d)
        {
            case Action3d.SingleSelect:
                // 单选
                viewer.Select();
                break;
            case Action3d.MultipleSelect:
                viewer.XORSelect();
                break;
            case Action3d.AreaSelect:
                myCurrentX = e.X;
                myCurrentY = e.Y;
                if (
                    Math.Abs(myCurrentX - mouseDownX) > ZEROWIDTHMIN
                    && Math.Abs(myCurrentY - mouseDownY) > ZEROWIDTHMIN
                )
                {
                    // 移动了,结束单次框选
                    DrawRectangle(false);
                    DragEvent(myCurrentX, myCurrentY, 1);
                }
                else
                {
                    // 单选
                    viewer.Select();
                }
                break;
            case Action3d.MultipleAreaSelect:
                if (
                    Math.Abs(myCurrentX - mouseDownX) > ZEROWIDTHMIN
                    && Math.Abs(myCurrentY - mouseDownY) > ZEROWIDTHMIN
                )
                {
                    DrawRectangle(false);
                    // 移动了 结束连续框选
                    MultiDragEvent(myCurrentX, myCurrentY, 1);
                }
                else
                {
                    viewer.XORSelect();
                }
                break;
            case Action3d.AreaZooming:
                // 结束区域缩放
                myCurrentX = e.X;
                myCurrentY = e.Y;
                DrawRectangle(false);
                if (
                    Math.Abs(myCurrentX - mouseDownX) > ZEROWIDTHMIN
                    && Math.Abs(myCurrentX - myCurrentY) > ZEROWIDTHMIN
                )
                {
                    viewer.FitArea(mouseDownX, mouseDownY, myCurrentX, myCurrentY);
                }
                RaiseZoomingFinished();
                break;
            case Action3d.DynamicRotation:
                //结束旋转
                break;
            case Action3d.DynamicPanning:
                // 结束平移
                break;
            default:
                break;
        }
        CurrentAction3d = Action3d.Nothing;
        Cursor = Cursors.Default;
    }

    protected override void OnMouseWheel(MouseEventArgs e)
    {
        base.OnMouseWheel(e);
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
        viewer.Zoom(zoomFactor, true);
        // 启动计时器
        cursorResetTimer.Start();
    }

    //无效设置
    //private void OnKeyDown(object sender, System.Windows.Forms.KeyEventArgs e)
    //{
    //    if (e.KeyCode == Keys.F)
    //    {
    //        this.FitAll();
    //    }
    //}


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
                Action3d.Nothing
                    => new Nothing(canvas),
                Action3d.AreaSelect => new AreaSelect(canvas),
                Action3d.MultipleAreaSelect => new MultipleAreaSelect(canvas),
                Action3d.AreaZooming => new AreaZooming(canvas),
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
        viewer.ZoomAllView();
    }

    /// <summary>
    /// 循环设置相机视图方向
    /// </summary>
    public void SetViewOrientation()
    {
        viewer.SetViewOrientation(CycleEnum(CurrentViewOrientation));
    }

    /// <summary>
    /// 设置相机视图方向为theMode
    /// </summary>
    /// <param name="theMode"></param>
    public void SetViewOrientation(ViewOrientation theMode)
    {
        CurrentViewOrientation = theMode;
        viewer.SetViewOrientation((int)theMode);
    }

    /// <summary>
    /// 重置视图的居中和方向
    /// </summary>
    public void Reset()
    {
        viewer.Reset();
    }

    /// <summary>
    /// 设置隐藏线模式
    /// </summary>
    /// <param name="theMode"></param>
    public void SetHidden(bool theMode)
    {
        viewer.SetDegenerateMode(theMode);
        degenerateMode = theMode;
    }

    /// <summary>
    /// 依次切换显示模式
    /// </summary>
    public void SetDisplayMode()
    {
        viewer.SetDisplayMode(CycleEnum(CurrentDisplayMode));
        viewer.UpdateCurrentViewer();

        RaiseZoomingFinished();
    }

    /// <summary>
    /// 切换到指定显示模式
    /// </summary>
    /// <param name="theMode"></param>
    public void SetDisplayMode(DisplayMode theMode)
    {
        CurrentDisplayMode = theMode;
        viewer.SetDisplayMode((int)theMode);
        viewer.UpdateCurrentViewer();

        RaiseZoomingFinished();
    }
    #endregion
    #region 显示
    public void Display(WAIS_Shape theAIS, bool Toupdate)
    {
        viewer.Display(theAIS, Toupdate);
    }

    public void EraseSelect()
    {
        viewer.EraseObjects();
    }

    public void Erase(WAIS_Shape theAIS, bool Toupdate)
    {
        viewer.Erase(theAIS, Toupdate);
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
            viewer.MultipleAreaSelect(
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
            viewer.AreaSelect(
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
            viewer.UpdateView();
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
        int x = Math.Min(mouseDownX, myCurrentX);
        int y = Math.Min(mouseDownY, myCurrentY);
        gr.DrawRectangle(
            p,
            x,
            y,
            Math.Abs(myCurrentX - mouseDownX),
            Math.Abs(myCurrentY - mouseDownY)
        );
    }
    #endregion
    public void SetSelectionMode(OCCTK.Visualization.SelectionMode theMode)
    {
        viewer.SetSelectionMode(theMode);
    }

    public WAIS_Shape GetSelectedShape()
    {
        WAIS_Shape result = viewer.GetSelectedAIS();
        return result;
    }
    #endregion
}
