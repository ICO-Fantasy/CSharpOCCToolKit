using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq.Expressions;
using System.Reflection;
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

#region 接口和事件

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

public partial class OCCCanvas
{
    // 定义事件
    public event AISSelectionMadeHandler? OnAISSelected;
    public event MouseMovedHandler? OnMouseMoved;

    /// <summary>
    /// 注册鼠标事件
    /// </summary>
    private void RegisterMouseAction()
    {
        MouseDown += new System.Windows.Forms.MouseEventHandler(OnMouseDown);
        MouseUp += new System.Windows.Forms.MouseEventHandler(OnMouseUp);
        MouseMove += new System.Windows.Forms.MouseEventHandler(OnMouseMove);
        MouseWheel += new System.Windows.Forms.MouseEventHandler(OnMouseWheel);
    }

    /// <summary>
    /// 注册键盘事件
    /// </summary>
    private void RegisterKeyBoardAction()
    {
        KeyPreview = true;
        KeyDown += new System.Windows.Forms.KeyEventHandler(OnKeyDown);
    }

    #region 字段

    /// <summary>
    /// 交互动作对应的鼠标指针
    /// </summary>
    public Dictionary<Action3d, Cursor> CURSORS =
        new()
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

    private bool _Detected = false;

    public bool isMousePointScroll = true; //true表示根据鼠标点缩放，false表示根据中心点缩放

    /// <summary>
    /// 选择框
    /// </summary>
    private RubberBand myBubberBand;

    private int mouseDownX;
    private int mouseDownY;
    private int mouseCurrentX;
    private int mouseCurrentY;
    private int mouseUpX;
    private int mouseUpY;

    #endregion

    #region 属性

    /// <summary>
    /// 重设滚轮光标计时器
    /// </summary>
    private readonly System.Windows.Forms.Timer cursorResetTimer;

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
            if (CURSORS.TryGetValue(value, out Cursor? cursor))
            {
                Cursor = cursor;
            }

            // 使用反射调用对应的方法
            string methodName = value.ToString();
            MethodInfo? methodInfo = GetType()
                .GetMethod(methodName, BindingFlags.NonPublic | BindingFlags.Instance);
            if (methodInfo != null)
            {
                methodInfo.Invoke(this, null); // 调用方法
            }
            else
            {
                // 尝试获取对应的属性
                var propertyInfo = GetType()
                    .GetProperty(methodName, BindingFlags.NonPublic | BindingFlags.Instance);
                if (propertyInfo != null)
                {
                    Action? action = propertyInfo.GetValue(this) as Action;
                    action.Invoke(); // 调用委托
                }
            }
        }
    }

    #endregion

    /// <summary>
    /// 计时器触发后将光标恢复为默认光标
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void CursorResetTimer_Tick(object? sender, EventArgs e)
    {
        Cursor = System.Windows.Forms.Cursors.Default;
        // 停止计时器
        cursorResetTimer.Stop();
    }

    #region 重写虚方法

    #region 鼠标

    private void OnMouseDown(object? sender, MouseEventArgs e)
    {
        // 获取鼠标按下的按钮和修改键
        //MouseButtons mouseButton = e.Button;
        //Keys modifiers = ModifierKeys;
        // 记录鼠标按下的位置
        mouseDownX = e.X;
        mouseDownY = e.Y;
        //默认为点击事件
        CurrentAction3d = Action3d.SingleSelect;
        //鼠标按下的时候执行检测
        AISContext.InitDetected();
        //是否检测到AIS
        _Detected = AISContext.MoreDetected();
        //! 记录旋转开始点
        MainView.StartRotation(mouseDownX, mouseDownY);
        //! 记录变换开始点
        if (myManipulator.HasActiveMode())
        {
            myManipulator.StartTransform(mouseDownX, mouseDownY, Canvas.GetMainView());
            //myManipulator.StartTransform(mouseDownX, mouseDownY, MainView);
        }
        MainView.StartPan();
    }

    //test 鼠标交互隐射
    public Dictionary<Tuple<MouseButtons, Keys>, Action3d> ActionMap =
        new()
        {
            { new(MouseButtons.Left, Keys.None), Action3d.AreaSelect },
            { new(MouseButtons.Left, Keys.Control), Action3d.MultipleAreaSelect }
        };

    private void OnMouseMove(object? sender, MouseEventArgs e)
    {
        //! 键盘响应一定要获得焦点
        this.Focus();
        //! 使用if而不是switch判断，让点击事件具有更高优先级
        // 获取鼠标当前位置
        //Point pt = e.Location;
        // 获取鼠标按下的按钮和修改键
        MouseButtons mouseButton = e.Button;
        MouseButtons mouseState = System.Windows.Forms.Control.MouseButtons;
        Keys modifiers = ModifierKeys;
        mouseCurrentX = e.X;
        mouseCurrentY = e.Y;
        OnMouseMoved?.Invoke(e.X, e.Y);

        AISContext.InitSelected();

        //! 同时按下了左键和右键
        if (
            (mouseState & (MouseButtons.Left | MouseButtons.Right))
            == (MouseButtons.Left | MouseButtons.Right)
        ) { }
        CurrentAction3d = ActionMap[new(mouseButton, modifiers)];
        //! 左键
        if (mouseButton == MouseButtons.Left)
        {
            if (_Detected)
            {
                // 操作器执行移动
                if (myManipulator.HasActiveMode())
                {
                    _manipulatorMode ??= myManipulator.ActiveMode();
                    _manipulatorAxis ??= myManipulator.ActiveAxisIndex();
                    if (
                        myManipulator.ActiveMode() == _manipulatorMode
                        && myManipulator.ActiveAxisIndex() == _manipulatorAxis
                    )
                    {
                        //移动
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
            if (modifiers == Keys.None)
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
                CurrentAction3d = Action3d.DynamicRotation;
            }
            // Dynamic Panning 表示交互式的平移画面与非交互的平移 Global Panning 相区别
            // 平移
            else if (modifiers == Keys.Control)
            {
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
        _currentAction3d = Action3d.Nothing;
        _manipulatorMode = null;
        _manipulatorAxis = null;
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

        double zoomFactor;
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
        if (isMousePointScroll)
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
        cursorResetTimer.Start();
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
            FitAll();
        }
    }

    #region Mouse Moved Action
    private Action AreaSelect => DrawRectangle;
    private Action MultipleAreaSelect => DrawRectangle;
    private Action AreaZooming => DrawRectangle;

    private void DynamicRotation()
    {
        MainView.Rotation(mouseCurrentX, mouseCurrentY);
    }

    private void DynamicPanning()
    {
        MainView.Pan(mouseCurrentX - mouseDownX, -(mouseCurrentY - mouseDownY));
    }

    private void Manipulator_Translation()
    {
        myManipulator.Transform(mouseCurrentX, mouseCurrentY, MainView);
    }

    #endregion

    #endregion

    #region 对象交互

    #region 选择框

    /// <summary>
    /// 绘制选择框
    /// </summary>
    /// <param name="draw"></param>
    private void DrawRectangle()
    {
        Debug.WriteLine(
            $"{Math.Min(mouseDownX, mouseCurrentX)},{Math.Min(Height - mouseDownY, Height - mouseCurrentY)},{Math.Max(mouseDownX, mouseCurrentX)},{Math.Max(Height - mouseDownY, Height - mouseCurrentY)}"
        );
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
            //! 不能即时刷新，否则上一次设置的框会闪动
            AISContext.Display(myBubberBand, false);
        }
    }

    #endregion

    #endregion
}
