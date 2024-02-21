using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Controls;
using System.Windows.Forms;
using OCCTK.Visualization;
using Windows.Storage.FileProperties;



namespace OCCViewForm
{
    public enum CurrentAction3d
    {
        CurAction3d_Nothing,
        CurAction3d_DynamicZooming,
        CurAction3d_AreaZooming,
        CurAction3d_DynamicPanning,
        CurAction3d_GlobalPanning,
        CurAction3d_DynamicRotation,
        CurAction3d_AreaSelection
    }
    public enum CurrentPressedKey
    {
        CurPressedKey_Nothing,
        CurPressedKey_Ctrl,
        CurPressedKey_Shift
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
        public OCCTK.Visualization.OCCView View { get; set; }
        #region 私有对象
        //视图
        private Pen RubberBandPen { get; set; } // 设置 RubberBand 的颜色和宽度
        #region 状态flag
        //缩放结束
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
        //当前选择模式
        public CurrentAction3d CurrentMode { get; private set; }
        //当前按下的按键
        private CurrentPressedKey CurrentPressedKey { get; set; }
        //零宽度的最小值
        private const int ZEROWIDTHMIN = 1;
        //是否正在绘制选择框
        private bool IsDrawRect { get; set; }
        private bool IsRectVisible { get; set; }
        #endregion
        #endregion
        #region 显示模式
        //隐藏线显示模式
        public bool DegenerateMode { get; private set; }
        //框线模式
        public bool IsWireframeEnabled { get; private set; }
        //阴影模式
        public bool IsShadingEnabled { get; private set; }

        //无效定义
        //public bool IsTransparencyEnabled { get; private set; }
        //public bool IsColorEnabled { get; private set; }
        //public bool IsMaterialEnabled { get; private set; }
        //public bool IsDeleteEnabled { get; private set; }

        private float myCurZoom;
        private int myXmin;
        private int myYmin;
        private int myXmax;
        private int myYmax;
        private int myRectDownX;
        private int myRectDownY;
        private int myButtonDownX;
        private int myButtonDownY;

        #endregion
        public OCCViewer()
        {
            InitializeComponent();

            View = new OCCView();
            View.InitOCCView();
            if (!View.InitViewer(this.Handle))
            {
                MessageBox.Show("图形初始化失败", "Error!");
            }
            RubberBandPen = new Pen(Color.White);
            CurrentMode = CurrentAction3d.CurAction3d_Nothing;
            CurrentPressedKey = CurrentPressedKey.CurPressedKey_Nothing;
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
        }
        #region 重写虚方法
        private void OnPaint(object sender, PaintEventArgs e)
        {
            View.RedrawView();
            View.UpdateView();
        }
        private void OnSizeChanged(object sender, EventArgs e)
        {
            View.UpdateView();
        }
        public void OnKeyUp()
        {
            CurrentPressedKey = CurrentPressedKey.CurPressedKey_Nothing;
        }

        private void OnMouseDown(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Left)
            {
                myXmin = e.X;
                myXmax = e.X;
                myYmin = e.Y;
                myYmax = e.Y;
                if (CurrentPressedKey == CurrentPressedKey.CurPressedKey_Ctrl)
                {
                    // start the dynamic zooming....
                    CurrentMode = CurrentAction3d.CurAction3d_DynamicZooming;
                }
                else
                {
                    switch (CurrentMode)
                    {
                        case CurrentAction3d.CurAction3d_Nothing:
                            if (CurrentPressedKey == CurrentPressedKey.CurPressedKey_Shift)
                            {
                                MultiDragEvent(myXmax, myYmax, -1);
                            }
                            else
                            {
                                DragEvent(myXmax, myYmax, -1);
                            }
                            break;
                        case CurrentAction3d.CurAction3d_DynamicRotation:
                            //View.SetDegenerateMode(DegenerateMode);
                            View.StartRotation(e.X, e.Y);
                            break;
                        case CurrentAction3d.CurAction3d_AreaZooming:
                            Cursor = Cursors.Hand;
                            break;
                        default:
                            break;
                    }
                }
            }
        }
        private void OnMouseMove(object sender, MouseEventArgs e)
        {
            //!使用if而不是switch判断，让点击事件具有更高优先级
            // 获取鼠标当前位置
            //Point pt = e.Location;
            // 获取鼠标按下的按钮和修改键
            MouseButtons buttons = e.Button;
            Keys modifiers = ModifierKeys;

            // 判断鼠标按钮和修改键的组合
            if (buttons == MouseButtons.Left)
            {
                if (modifiers == Keys.None)
                {
                    // 区域选择
                    DrawRectangle(false);
                    myXmax = e.X;
                    myYmax = e.Y;
                    DrawRectangle(true);
                    CurrentMode = CurrentAction3d.CurAction3d_AreaSelection;
                }
            }
            else if (buttons == MouseButtons.Right)
            {
                if (modifiers == Keys.Shift)
                {
                    // 框选区域放大
                    DrawRectangle(false);
                    myXmax = e.X;
                    myYmax = e.Y;
                    DrawRectangle(true);
                    CurrentMode = CurrentAction3d.CurAction3d_AreaZooming;
                }
            }
            else if (buttons == MouseButtons.Middle)
            {
                if (modifiers == Keys.None)
                {
                    // 旋转
                    View.Rotation(e.X, e.Y);
                    View.RedrawView();
                    CurrentMode = CurrentAction3d.CurAction3d_DynamicRotation;
                }
                else if (modifiers == Keys.Control)
                {
                    // Dynamic Panning 表示交互式的平移画面与非交互的平移 Global Panning 相区别
                    // 平移
                    View.Pan(e.X - myXmax, myYmax - e.Y);
                    myXmax = e.X;
                    myYmax = e.Y;
                    CurrentMode = CurrentAction3d.CurAction3d_DynamicPanning;
                }
            }
        }
        private void OnMouseUp(object sender, MouseEventArgs e)
        {
            // 获取鼠标按下的按钮和修改键
            MouseButtons buttons = e.Button;
            Keys modifiers = ModifierKeys;
            if (buttons == MouseButtons.Left)
            {
                //if (modifiers == Keys.Shift)
                //{
                //    return;
                //}
                switch (CurrentMode)
                {
                    case CurrentAction3d.CurAction3d_Nothing:
                        myXmax = e.X;
                        myYmax = e.Y;
                        if (e.X == myXmin && e.Y == myYmin)
                        {
                            if (modifiers == Keys.Control)
                            {
                                // 连续选择
                                View.ShiftSelect();
                            }
                            else
                            {
                                //单选
                                View.Select();
                            }
                        }
                        else
                        {
                            DrawRectangle(false);
                            if (modifiers == Keys.Control)
                            {
                                // 连续框选
                                MultiDragEvent(myXmax, myYmax, 1);
                            }
                            else
                            {
                                //单选
                                DragEvent(myXmax, myYmax, 1);
                            }
                        }
                        break;
                    case CurrentAction3d.CurAction3d_DynamicZooming:
                        // 结束缩放
                        Cursor = Cursors.Arrow;
                        RaiseZoomingFinished();
                        CurrentMode = CurrentAction3d.CurAction3d_Nothing;
                        break;
                    case CurrentAction3d.CurAction3d_DynamicPanning:
                        // 结束平移
                        CurrentMode = CurrentAction3d.CurAction3d_Nothing;
                        break;
                    //case CurrentAction3d.CurAction3d_GlobalPanning:
                    //    // 暂不支持非交互的平移
                    //    View.Place(e.X, e.Y, myCurZoom);
                    //    CurrentMode = CurrentAction3d.CurAction3d_Nothing;
                    //    break;
                    case CurrentAction3d.CurAction3d_DynamicRotation:
                        //结束旋转
                        CurrentMode = CurrentAction3d.CurAction3d_Nothing;
                        //View.SetDegenerateMode(DegenerateMode); //不需要设置隐藏线模式
                        break;
                    default:
                        break;
                }
            }
            else if (buttons == MouseButtons.Right)
            {
                //View.SetDegenerateMode(DegenerateMode);//不需要设置隐藏线模式
                // Do Nothing
                switch (CurrentMode)
                {
                    case CurrentAction3d.CurAction3d_AreaZooming:
                        // 结束区域缩放
                        myXmax = e.X;
                        myYmax = e.Y;
                        DrawRectangle(false);
                        if (Math.Abs(myXmax - myXmin) > ZEROWIDTHMIN &&
                             Math.Abs(myXmax - myYmax) > ZEROWIDTHMIN)
                        {
                            View.FitArea(myXmin, myYmin, myXmax, myYmax);
                        }
                        Cursor = Cursors.Arrow;
                        RaiseZoomingFinished();
                        CurrentMode = CurrentAction3d.CurAction3d_Nothing;
                        break;
                    default:
                        break;
                }
            }

        }
        #endregion
        #region 视图控制
        public void FitAll()
        {
            View.ZoomAllView();
        }
        public void SetViewOrientation(ViewOrientation theMode)
        {
            View.SetViewOrientation((int)theMode);
        }
        public void Reset()
        {
            View.Reset();
        }
        /// <summary>
        /// 设置隐藏线模式
        /// </summary>
        /// <param name="theMode"></param>
        public void Hidden(bool theMode)
        {
            View.SetDegenerateMode(theMode);
            DegenerateMode = theMode;
        }

        /// <summary>
        /// 依次切换显示模式或切换到指定显示模式
        /// </summary>
        public void SwitchDisplayMode()
        {
            View.SetDisplayMode((int)DisplayMode.Wireframe);
            View.UpdateCurrentViewer();

            RaiseZoomingFinished();
        }
        public void SwitchDisplayMode(DisplayMode mode)
        {
            View.SetDisplayMode((int)DisplayMode.Wireframe);
            View.UpdateCurrentViewer();

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
                View.ShiftSelect(Math.Min(myButtonDownX, x), Math.Min(myButtonDownY, y),
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
                View.Select(Math.Min(myButtonDownX, x), Math.Min(myButtonDownY, y),
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
                p = new System.Drawing.Pen(System.Drawing.Color.Black);
                IsRectVisible = false;
                View.UpdateView();
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
            int x = Math.Min( myXmin, myXmax );
            int y = Math.Min( myYmin, myYmax );
            gr.DrawRectangle(p, x, y, Math.Abs(myXmax - myXmin), Math.Abs(myYmax - myYmin));
            myRectDownX = Math.Max(myXmin, myXmax);
            myRectDownY = Math.Max(myYmin, myYmax);
        }
        #endregion
    }
}
