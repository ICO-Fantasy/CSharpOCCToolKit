using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using OCCTK.Visualization;
using Windows.Storage.FileProperties;


namespace OCCViewForm
{
    public enum CurrentAction3d
    {
        Nothing,
        DynamicZooming,
        AreaZooming,
        DynamicPanning,
        GlobalPanning,
        DynamicRotation,
        AreaSelection
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
    public class OCCViewer : Form
    {
        //框选零宽度的最小值
        private const int ZEROWIDTHMIN = 1;
        /// <summary>
        /// 构造函数
        /// </summary>
        public OCCViewer()
        {
            InitializeComponent();

            view = new OCCView();
            view.InitOCCView();
            if (!view.InitViewer(this.Handle))
            {
                MessageBox.Show("图形初始化失败", "Error!");
            }
            // 初始化计时器
            cursorResetTimer = new System.Windows.Forms.Timer();
            cursorResetTimer.Interval = 500; // 设置计时器间隔为100毫秒
            cursorResetTimer.Tick += CursorResetTimer_Tick; // 绑定计时器的Tick事件处理方法
            CurrentMode = CurrentAction3d.Nothing;
            IsDrawRect = false;
            DegenerateMode = true;
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
        }
        /// <summary>
        /// 计时器触发后将光标恢复为默认光标
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void CursorResetTimer_Tick(object sender, EventArgs e)
        {
            Cursor = Cursors.Default;
            // 停止计时器
            cursorResetTimer.Stop();
        }
        #region 属性
        public OCCView view { get; set; } // 视图
        private System.Windows.Forms.Timer cursorResetTimer { get; set; } // 重设滚轮光标计时器
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
        /// <summary>
        /// 当前选择模式
        /// </summary>
        public CurrentAction3d CurrentMode { get; private set; }
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
        private int myCurrentX;
        private int myCurrentY;
        private int myButtonDownX;
        private int myButtonDownY;
        #endregion
        #endregion
        #region 重写虚方法
        private void OnPaint(object sender, PaintEventArgs e)
        {
            view.RedrawView();
            view.UpdateView();
        }
        private void OnSizeChanged(object sender, EventArgs e)
        {
            view.UpdateView();
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
            view.StartRotation(e.X, e.Y);
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
            //!使用if而不是switch判断，让点击事件具有更高优先级
            // 获取鼠标当前位置
            //Point pt = e.Location;
            // 获取鼠标按下的按钮和修改键
            MouseButtons mouseButton = e.Button;
            Keys modifiers = ModifierKeys;

            // 判断鼠标按钮和修改键的组合
            if (mouseButton == MouseButtons.Left)
            {
                if (modifiers == Keys.None || modifiers == Keys.Control)
                {
                    // 框选或多次框选
                    DrawRectangle(false);
                    myCurrentX = e.X;
                    myCurrentY = e.Y;
                    DrawRectangle(true);
                    CurrentMode = CurrentAction3d.AreaSelection;
                }
            }
            else if (mouseButton == MouseButtons.Right)
            {
                if (modifiers == Keys.Shift)
                {
                    // 框选区域放大
                    DrawRectangle(false);
                    myCurrentX = e.X;
                    myCurrentY = e.Y;
                    DrawRectangle(true);
                    CurrentMode = CurrentAction3d.AreaZooming;
                }
            }
            else if (mouseButton == MouseButtons.Middle)
            {
                if (modifiers == Keys.None)
                {
                    // 旋转
                    view.Rotation(e.X, e.Y);
                    view.RedrawView();
                    CurrentMode = CurrentAction3d.DynamicRotation;
                }
                else if (modifiers == Keys.Control)
                {
                    // Dynamic Panning 表示交互式的平移画面与非交互的平移 Global Panning 相区别
                    // 平移
                    view.Pan(e.X - myCurrentX, myCurrentY - e.Y);
                    myCurrentX = e.X;
                    myCurrentY = e.Y;
                    CurrentMode = CurrentAction3d.DynamicPanning;
                }
            }
        }
        private void OnMouseUp(object sender, MouseEventArgs e)
        {
            // 获取鼠标按下的按钮和修改键
            MouseButtons mouseButton = e.Button;
            Keys modifiers = ModifierKeys;
            if (mouseButton == MouseButtons.Left)
            {
                switch (CurrentMode)
                {
                    case CurrentAction3d.Nothing:
                        myCurrentX = e.X;
                        myCurrentY = e.Y;
                        if (myCurrentX == mouseDownX && myCurrentY == mouseDownY)
                        {
                            if (modifiers == Keys.Control)
                            {
                                // 连续选择
                                view.ShiftSelect();
                            }
                            else
                            {
                                // 单选
                                view.Select();
                            }
                        }
                        break;
                    case CurrentAction3d.AreaSelection:
                        myCurrentX = e.X;
                        myCurrentY = e.Y;
                        if (Math.Abs(myCurrentX - mouseDownX) > ZEROWIDTHMIN && Math.Abs(myCurrentY - mouseDownY) > ZEROWIDTHMIN) //移动了
                        {
                            DrawRectangle(false);
                            if (modifiers == Keys.Control)
                            {
                                // 结束连续框选
                                MultiDragEvent(myCurrentX, myCurrentY, 1);
                            }
                            else
                            {
                                // 结束单次框选
                                DragEvent(myCurrentX, myCurrentY, 1);
                            }

                        }
                        break;
                    //case CurrentAction3d.DynamicZooming:
                    //    // 结束缩放
                    //    Cursor = Cursors.Arrow;
                    //    RaiseZoomingFinished();
                    //    CurrentMode = CurrentAction3d.Nothing;
                    //    break;
                    case CurrentAction3d.DynamicRotation:
                        // 结束旋转
                        CurrentMode = CurrentAction3d.Nothing;
                        break;
                    case CurrentAction3d.DynamicPanning:
                        // 结束平移
                        CurrentMode = CurrentAction3d.Nothing;
                        break;
                    //case CurrentAction3d.GlobalPanning:
                    //    // 暂不支持非交互的平移
                    //    view.Place(e.X, e.Y, myCurZoom);
                    //    CurrentMode = CurrentAction3d.Nothing;
                    //    break;

                    default:
                        break;
                }
            }
            else if (mouseButton == MouseButtons.Right)
            {
                //view.SetDegenerateMode(DegenerateMode);//不需要设置隐藏线模式
                // Do Nothing
                switch (CurrentMode)
                {
                    case CurrentAction3d.AreaZooming:
                        // 结束区域缩放
                        myCurrentX = e.X;
                        myCurrentY = e.Y;
                        DrawRectangle(false);
                        if (Math.Abs(myCurrentX - mouseDownX) > ZEROWIDTHMIN &&
                             Math.Abs(myCurrentX - myCurrentY) > ZEROWIDTHMIN)
                        {
                            view.FitArea(mouseDownX, mouseDownY, myCurrentX, myCurrentY);
                        }
                        Cursor = Cursors.Arrow;
                        RaiseZoomingFinished();
                        CurrentMode = CurrentAction3d.Nothing;
                        break;
                    default:
                        break;
                }
            }
            else if (mouseButton == MouseButtons.Middle)
            {
                switch (CurrentMode)
                {
                    case CurrentAction3d.DynamicRotation:
                        //结束旋转
                        CurrentMode = CurrentAction3d.Nothing;
                        break;
                    default:
                        break;
                }
            }

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
            view.Zoom(zoomFactor, true);
            // 启动计时器
            cursorResetTimer.Start();
        }
        #endregion
        #region 视图控制
        public void FitAll()
        {
            view.ZoomAllView();
        }
        public void SetViewOrientation(ViewOrientation theMode)
        {
            view.SetViewOrientation((int)theMode);
        }
        public void Reset()
        {
            view.Reset();
        }
        /// <summary>
        /// 设置隐藏线模式
        /// </summary>
        /// <param name="theMode"></param>
        public void Hidden(bool theMode)
        {
            view.SetDegenerateMode(theMode);
            DegenerateMode = theMode;
        }

        /// <summary>
        /// 依次切换显示模式或切换到指定显示模式
        /// </summary>
        public void SwitchDisplayMode()
        {
            view.SetDisplayMode((int)DisplayMode.Wireframe);
            view.UpdateCurrentViewer();

            RaiseZoomingFinished();
        }
        public void SwitchDisplayMode(DisplayMode mode)
        {
            view.SetDisplayMode((int)DisplayMode.Wireframe);
            view.UpdateCurrentViewer();

            RaiseZoomingFinished();
        }
        #endregion
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
                view.ShiftSelect(Math.Min(myButtonDownX, x), Math.Min(myButtonDownY, y),
                                  Math.Max(myButtonDownX, x), Math.Max(myButtonDownY, y));
            }
        }
        protected void DragEvent(int x, int y, int theState)
        {
            if (theState == -1) //mouse is down
            {
                myButtonDownX = x;
                myButtonDownY = y;
            }
            else if (theState == 1) //mouse is up
            {
                view.Select(Math.Min(myButtonDownX, x), Math.Min(myButtonDownY, y),
                             Math.Max(myButtonDownX, x), Math.Max(myButtonDownY, y));
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
            if (IsRectVisible || !draw)//erase the rect
            {
                //p = new System.Drawing.Pen(System.Drawing.Color.Black);
                IsRectVisible = false;
                view.UpdateView();
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
            gr.DrawRectangle(p, x, y, Math.Abs(myCurrentX - mouseDownX), Math.Abs(myCurrentY - mouseDownY));
        }
        #endregion
    }
}
