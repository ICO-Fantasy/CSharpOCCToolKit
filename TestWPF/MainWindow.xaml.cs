using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Forms.Integration;

using OCCTK.Laser;
using OCCTK.OCC.AIS;

using OCCViewForm;

using TestWPF.Models;

using static OCCTK.Laser.VerticalPlateDirection;
namespace TestWPF
{
    /// <summary>
    /// 交互逻辑 MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private OCCCanvas Viewer;
        private WAIS_Shape InputWorkpiece;
        public List<VerticalPlate> VerticalPlates = new List<VerticalPlate>();
        /// <summary>
        /// 横板
        /// </summary>
        private BasePlate? BasePlate;
        #region 创建竖板参数
        public int XNum { get; set; } = 3;
        public int YNum { get; set; } = 3;
        #endregion
        #region 竖板全局参数
        //X方向
        /// <summary>
        /// 竖板横向初始偏移
        /// </summary>
        public double InitialOffsetXParameter { get; set; }
        /// <summary>
        /// 竖板横向偏移
        /// </summary>
        public double OffsetXParameter { get; set; }
        //Y方向
        /// <summary>
        /// 竖板纵向初始偏移
        /// </summary>
        public double InitialOffsetYParameter { get; set; }
        /// <summary>
        /// 竖板纵向偏移
        /// </summary>
        public double OffsetYParameter { get; set; }
        //Z方向
        /// <summary>
        /// 竖板连接高
        /// </summary>
        public double ConnectionHeightParameter { get; set; }
        /// <summary>
        /// 竖板最小支撑长度
        /// </summary>
        public double MinSupportingLenParameter { get; set; }
        //XY方向
        /// <summary>
        /// 竖板避让间隙
        /// </summary>
        public double ClearancesParameter { get; set; }
        /// <summary>
        /// 竖板切断距离
        /// </summary>
        public double CuttingDistanceParameter { get; set; }
        #endregion
        #region 单块板属性
        public VerticalPlate? CurrentPlate { get; set; }
        #endregion
        #region 显示用list
        #endregion
        public MainWindow()
        {
            InitializeComponent();
            // 创建 Windows Forms 控件和 WindowsFormsHost
            WindowsFormsHost aHost = new WindowsFormsHost();
            Viewer = new OCCCanvas();
            Viewer.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            Viewer.Show();
            aHost.Child = Viewer;
            canvas_grid.Children.Add(aHost);
            SetDefaultParameter();
            #region 全局属性
            //设置两个可选方向
            DirectionComboBox.Items.Add("X");
            DirectionComboBox.Items.Add("Y");
            //位置信息在初始时为隐藏
            PlateX_ComboBox.Visibility = Visibility.Collapsed;
            PlateY_ComboBox.Visibility = Visibility.Collapsed;

            // 设置InitialOffsetXLabel的内容和TextBox的初始值，并添加TextChanged事件处理程序
            InitialOffsetXLabel.Label.Content = "竖板横向初始偏移";
            InitialOffsetXLabel.TextBox.Text = InitialOffsetXParameter.ToString();
            InitialOffsetXLabel.TextBox.TextChanged += InitialOffsetX_TextChanged;

            // 设置OffsetXLabel的内容和TextBox的初始值，并添加TextChanged事件处理程序
            OffsetXLabel.Label.Content = "竖板横向偏移";
            OffsetXLabel.TextBox.Text = OffsetXParameter.ToString();
            OffsetXLabel.TextBox.TextChanged += OffsetX_TextChanged;

            // 设置InitialOffsetYLabel的内容和TextBox的初始值，并添加TextChanged事件处理程序
            InitialOffsetYLabel.Label.Content = "竖板纵向初始偏移";
            InitialOffsetYLabel.TextBox.Text = InitialOffsetYParameter.ToString();
            InitialOffsetYLabel.TextBox.TextChanged += InitialOffsetY_TextChanged;

            // 设置OffsetYLabel的内容和TextBox的初始值，并添加TextChanged事件处理程序
            OffsetYLabel.Label.Content = "竖板纵向偏移";
            OffsetYLabel.TextBox.Text = OffsetYParameter.ToString();
            OffsetYLabel.TextBox.TextChanged += OffsetY_TextChanged;

            // 设置ConnectionHeightLabel的内容和TextBox的初始值，并添加TextChanged事件处理程序
            ConnectionHeightLabel.Label.Content = "竖板连接高";
            ConnectionHeightLabel.TextBox.Text = ConnectionHeightParameter.ToString();
            ConnectionHeightLabel.TextBox.TextChanged += ConnectionHeight_TextChanged;

            // 设置MinSupportingLenLabel的内容和TextBox的初始值，并添加TextChanged事件处理程序
            MinSupportingLenLabel.Label.Content = "竖板最小支撑长度";
            MinSupportingLenLabel.TextBox.Text = MinSupportingLenParameter.ToString();
            MinSupportingLenLabel.TextBox.TextChanged += MinSupportingLen_TextChanged;

            // 设置ClearancesLabel的内容和TextBox的初始值，并添加TextChanged事件处理程序
            ClearancesLabel.Label.Content = "竖板避让间隙";
            ClearancesLabel.TextBox.Text = ClearancesParameter.ToString();
            ClearancesLabel.TextBox.TextChanged += Clearances_TextChanged;

            // 设置CuttingDistanceLabel的内容和TextBox的初始值，并添加TextChanged事件处理程序
            CuttingDistanceLabel.Label.Content = "竖板切断距离";
            CuttingDistanceLabel.TextBox.Text = CuttingDistanceParameter.ToString();
            CuttingDistanceLabel.TextBox.TextChanged += CuttingDistance_TextChanged;
            #endregion
            #region 创建竖板
            XNum_TextBox.Text = XNum.ToString();
            XNum_TextBox.TextChanged += XNum_TextChanged;
            YNum_TextBox.Text = YNum.ToString();
            YNum_TextBox.TextChanged += YNum_TextChanged;
            #endregion
            #region 单块板的属性
            #endregion
        }
        #region 视角
        private void ForntView_Button_Click(object sender, RoutedEventArgs e)
        {
            Viewer.SetViewOrientation(ViewOrientation.Front);
        }
        private void BackView_Button_Click(object sender, RoutedEventArgs e)
        {
            Viewer.SetViewOrientation(ViewOrientation.Back);
        }
        private void TopView_Button_Click(object sender, RoutedEventArgs e)
        {
            Viewer.SetViewOrientation(ViewOrientation.Top);
        }
        private void BottomView_Button_Click(object sender, RoutedEventArgs e)
        {
            Viewer.SetViewOrientation(ViewOrientation.Bottom);
        }
        private void LeftView_Button_Click(object sender, RoutedEventArgs e)
        {
            Viewer.SetViewOrientation(ViewOrientation.Left);
        }
        private void RightView_Button_Click(object sender, RoutedEventArgs e)
        {
            Viewer.SetViewOrientation(ViewOrientation.Right);
        }
        private void Wireframe_Button_Click(object sender, RoutedEventArgs e)
        {
            Viewer.SetDisplayMode(DisplayMode.Wireframe);
        }
        private void Shading_Button_Click(object sender, RoutedEventArgs e)
        {
            Viewer.SetDisplayMode(DisplayMode.Shading);
        }
        #endregion
        private void FitAll_Button_Click(object sender, RoutedEventArgs e)
        {
            Viewer.FitAll();
        }
        private void Test_Button_Click(object sender, RoutedEventArgs e)
        {
            //Viewer.MakeClampTest();
            //Viewer.Test();
            InputWorkpiece = Viewer.viewer.TestMakeBox();
            Viewer.Display(InputWorkpiece, true);
        }
        #region 导入
        private void Test_Input_Button_Click(object sender, RoutedEventArgs e)
        {
            Viewer.viewer.EraseAll();
            InputWorkpiece = WMakeSimpleClamp.TestInputWorkpiece("mods\\mytest.STEP");
            //InputWorkpiece = OCCTK.Laser.WMakeSimpleClamp.TestInputWorkpiece("mods\\test1Small.STEP");
            Viewer.Display(InputWorkpiece, true);
        }

        private void Test_input_test_1_Click(object sender, RoutedEventArgs e)
        {
            Viewer.viewer.EraseAll();
            InputWorkpiece = WMakeSimpleClamp.TestInputWorkpiece("mods\\test1.stp");
            BasePlate = null;
            Viewer.Display(InputWorkpiece, true);
            Viewer.FitAll();
        }
        private void Test_input_test_2_Click(object sender, RoutedEventArgs e)
        {
            Viewer.viewer.EraseAll();
            InputWorkpiece = WMakeSimpleClamp.TestInputWorkpiece("mods\\test2.stp");
            BasePlate = null;
            Viewer.Display(InputWorkpiece, true);
            Viewer.FitAll();
        }
        private void Test_input_test_3_Click(object sender, RoutedEventArgs e)
        {
            Viewer.viewer.EraseAll();
            InputWorkpiece = WMakeSimpleClamp.TestInputWorkpiece("mods\\test3.STEP");
            BasePlate = null;
            Viewer.Display(InputWorkpiece, true);
            Viewer.FitAll();
        }
        private void Test_input_test_4_Click(object sender, RoutedEventArgs e)
        {
            Viewer.viewer.EraseAll();
            InputWorkpiece = WMakeSimpleClamp.TestInputWorkpiece("mods\\test4.stp");
            BasePlate = null;
            Viewer.Display(InputWorkpiece, true);
            Viewer.FitAll();
        }
        private void Test_input_test_5_Click(object sender, RoutedEventArgs e)
        {
            Viewer.viewer.EraseAll();
            InputWorkpiece = WMakeSimpleClamp.TestInputWorkpiece("mods\\test5.stp");
            BasePlate = null;
            Viewer.Display(InputWorkpiece, true);
            Viewer.FitAll();
        }

        #endregion
        private void Test_MakeV_Button_Click(object sender, RoutedEventArgs e)
        {
            int XNum = 4; // 实际会生成num-1个
            int YNum = 4;// 实际会生成num-1个

            double dx = (int)Math.Floor(BasePlate.dX / XNum);
            double dy = (int)Math.Floor(BasePlate.dY / YNum);

            double VerticalPlateClearances = 5.0;
            double VerticalPlateMinSupportingLen = 10;
            double VerticalPlateCuttingDistance = 20.0;
            double testXValue = BasePlate.X;
            double testYValue = BasePlate.Y;
            List<WAIS_Shape> result = new List<WAIS_Shape>();
            List<Piece> resultX, resultY;

            while (testXValue <= BasePlate.X + BasePlate.dX)
            {
                testXValue += dx;
                resultX = OCCTK.Laser.WMakeSimpleClamp.TestMakeVertical(InputWorkpiece,
                                                                        BasePlate,
                                                                        OCCTK.Laser.VerticalPlateDirection.X,
                                                                        testXValue,
                                                                        VerticalPlateClearances,
                                                                        VerticalPlateMinSupportingLen,
                                                                        VerticalPlateCuttingDistance);
                result.AddRange(resultX.Select(item => item.shape));
            }

            while (testYValue <= BasePlate.Y + BasePlate.dY)
            {
                testYValue += dy;
                resultY = OCCTK.Laser.WMakeSimpleClamp.TestMakeVertical(InputWorkpiece,
                                                                        BasePlate,
                                                                        OCCTK.Laser.VerticalPlateDirection.Y,
                                                                        testYValue,
                                                                        VerticalPlateClearances,
                                                                        VerticalPlateMinSupportingLen,
                                                                        VerticalPlateCuttingDistance);
                result.AddRange(resultY.Select(item => item.shape));
            }

            foreach (var item in result)
            {
                Viewer.Display(item, true);
            }
        }
        //private void Test_MakeV_Button_Click_old_20240330(object sender, RoutedEventArgs e)
        //{
        //    int XNum = 4; // 实际会生成num-1个
        //    int YNum = 4;// 实际会生成num-1个
        //    double maxVerticalLength = 10;

        //    double dx = (int)Math.Floor(BasePlate.dX / XNum);
        //    double dy = (int)Math.Floor(BasePlate.dY / YNum);

        //    double VerticalPlateClearances = 5.0;
        //    double VerticalPlateMinSupportingLen = 500.0;
        //    double VerticalPlateCuttingDistance = 20.0;
        //    double testXValue = BasePlate.X;
        //    double testYValue = BasePlate.Y;
        //    List<WAIS_Shape> result = new List<WAIS_Shape>();
        //    List<WAIS_Shape> resultX, resultY;

        //    while (testXValue <= BasePlate.X + BasePlate.dX)
        //    {
        //        testXValue += dx;
        //        resultX = OCCTK.Laser.WMakeSimpleClamp.TestMakeVertical(InputWorkpiece,
        //                                                                BasePlate,
        //                                                                OCCTK.Laser.VerticalPlateDirection.X,
        //                                                                testXValue,
        //                                                                VerticalPlateClearances,
        //                                                                VerticalPlateMinSupportingLen,
        //                                                                VerticalPlateCuttingDistance);
        //        result.AddRange(resultX);
        //    }

        //    while (testYValue <= BasePlate.Y + BasePlate.dY)
        //    {
        //        testYValue += dy;
        //        resultY = OCCTK.Laser.WMakeSimpleClamp.TestMakeVertical(InputWorkpiece,
        //                                                                          BasePlate,
        //                                                                          OCCTK.Laser.VerticalPlateDirection.Y,
        //                                                                          testYValue,
        //                                                                          VerticalPlateClearances,
        //                                                                          VerticalPlateMinSupportingLen,
        //                                                                          VerticalPlateCuttingDistance);
        //        result.AddRange(resultY);
        //    }

        //    foreach (var item in result)
        //    {
        //        Viewer.Display(item, true);
        //    }
        //}

        private void Test_ConnectV_Button_Click(object sender, RoutedEventArgs e)
        {

        }
        #region 选择模式
        private void Select_Shape_Button_Click(object sender, RoutedEventArgs e)
        {
            Viewer.SetSelectionMode(OCCTK.Visualization.SelectionMode.Shape);
        }

        private void Select_Face_Button_Click(object sender, RoutedEventArgs e)
        {
            Viewer.SetSelectionMode(OCCTK.Visualization.SelectionMode.Face);
        }

        private void Select_Wire_Button_Click(object sender, RoutedEventArgs e)
        {
            Viewer.SetSelectionMode(OCCTK.Visualization.SelectionMode.Edge);
        }

        private void Select_Vertex_Button_Click(object sender, RoutedEventArgs e)
        {
            Viewer.SetSelectionMode(OCCTK.Visualization.SelectionMode.Vertex);
        }

        #endregion

        private void Erase_Button_Click(object sender, RoutedEventArgs e)
        {
            Viewer.viewer.EraseObjects();
        }

        private void SetDefaultParameter()
        {
            InitialOffsetXParameter = 5;
            OffsetXParameter = 20;
            InitialOffsetYParameter = 5;
            OffsetYParameter = 5;

            ConnectionHeightParameter = 20;
            MinSupportingLenParameter = 0;

            ClearancesParameter = 40;
            CuttingDistanceParameter = 50;
        }
        #region 监听事件
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
                ConnectionHeightParameter = newValue;
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
                // 更新CuttingDistanceParameter的值
                CuttingDistanceParameter = newValue;
            }
        }
        //更新当前板坐标
        private void UpdateCurrentPlateLocation_Button_Click(object sender, RoutedEventArgs e)
        {
            // 获取TextBox的新值
            double newValue;
            if (CurrentPlate != null)
            {
                //更新视图显示
                foreach (var item in CurrentPlate.Slices)
                {
                    Viewer.viewer.Erase(item.shape, false);
                }
                Viewer.viewer.UpdateCurrentViewer();
                // 更新CurrentPlateValueLable的值
                if (double.TryParse(CurrentPlateLocationTextBox.Text, out newValue))
                {
                    CurrentPlate.Location = newValue;
                }
                //更新stack中的元素
                Plate_StackPanel.Children.Clear();
                foreach (var item in CurrentPlate.Slices)
                {
                    Plate_StackPanel.Children.Add(MakeStackItem(item));
                }
                //显示新创建的板
                foreach (var item in CurrentPlate.Slices)
                {
                    Viewer.viewer.Display(item.shape, theToUpdateViewer: false);
                }
                Viewer.viewer.UpdateCurrentViewer();
            }
        }
        private void XNum_TextChanged(object sender, EventArgs e)
        {
            // 获取TextBox的新值
            TextBox textBox = (TextBox)sender;
            int newValue;
            if (int.TryParse(textBox.Text, out newValue))
            {
                // 更新XNum的值
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
                // 更新XNum的值
                YNum = newValue;
            }
        }
        private void DirectionComboBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            string selectedDirection = (string)DirectionComboBox.SelectedItem;

            if (selectedDirection == "X")
            {
                PlateX_ComboBox.Visibility = Visibility.Visible;
                PlateY_ComboBox.Visibility = Visibility.Collapsed;
            }
            else if (selectedDirection == "Y")
            {
                PlateX_ComboBox.Visibility = Visibility.Collapsed;
                PlateY_ComboBox.Visibility = Visibility.Visible;
            }
        }
        private void PlateX_ComboBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            //清空列表
            Plate_StackPanel.Children.Clear();
            CurrentPlate = (VerticalPlate)PlateX_ComboBox.SelectedItem;
            if (CurrentPlate != null)
            {
                //显示选中板的坐标
                CurrentPlateLocationTextBox.Text = CurrentPlate.Location.ToString();
                //清空画布并重新显示
                Viewer.viewer.EraseAll();
                Viewer.viewer.Display(InputWorkpiece, false);
                Viewer.viewer.Display(BasePlate.shape, false);
                foreach (var item in CurrentPlate.Slices)
                {
                    Plate_StackPanel.Children.Add(MakeStackItem(item));
                }
                Viewer.viewer.UpdateCurrentViewer();
            }
        }
        private void PlateY_ComboBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            //清空列表
            Plate_StackPanel.Children.Clear();
            //更新当前选中的板
            CurrentPlate = (VerticalPlate)PlateY_ComboBox.SelectedItem;
            if (CurrentPlate != null)
            {
                //显示选中板的坐标
                CurrentPlateLocationTextBox.Text = CurrentPlate.Location.ToString();
                //清空画布并重新显示
                Viewer.viewer.EraseAll();
                Viewer.viewer.Display(InputWorkpiece, false);
                Viewer.viewer.Display(BasePlate.shape, false);
                foreach (var item in CurrentPlate.Slices)
                {
                    Plate_StackPanel.Children.Add(MakeStackItem(item));
                }
                Viewer.viewer.UpdateCurrentViewer();
            }
        }

        //单板

        private Grid MakeStackItem(Piece thePiece)
        {
            if (CurrentPlate == null)
            {
                return null; // 直接返回，不执行后续代码
            }
            // 创建一个新的 StackPanel
            Grid outerStackPanel = new Grid();

            // 创建列定义
            ColumnDefinition column1 = new ColumnDefinition();
            ColumnDefinition column2 = new ColumnDefinition();
            ColumnDefinition column3 = new ColumnDefinition();

            // 设置列宽度为相对值
            column1.Width = new GridLength(1, GridUnitType.Star);
            column2.Width = new GridLength(1, GridUnitType.Auto);
            column3.Width = new GridLength(1, GridUnitType.Auto);
            // 将列添加到 Grid 中
            outerStackPanel.ColumnDefinitions.Add(column1);
            outerStackPanel.ColumnDefinitions.Add(column2);
            outerStackPanel.ColumnDefinitions.Add(column3);

            // 创建要添加到 StackPanel 中的子元素
            Label PieceLabel = new Label();
            PieceLabel.Content = "位置: " + thePiece.start.ToString("F1");
            PieceLabel.HorizontalContentAlignment = HorizontalAlignment.Left; // 设置 Label 的水平内容对齐方式为 Left


            Button selectButton = new Button();
            selectButton.Content = "选中";
            selectButton.Click += (sender, e) => { Viewer.viewer.SelectAIS(thePiece.shape); };

            Button deletButton = new Button();
            deletButton.Content = "删除";
            // 创建删除按钮的 Click 事件处理程序
            deletButton.Click += (sender, e) =>
            {
                #region 删除按钮所在的Grid
                // 获取删除按钮所在的 Grid
                Grid parentStackPanel = (Grid)deletButton.Parent;

                // 如果父 StackPanel 不为空，则从其父元素中移除
                if (parentStackPanel != null)
                {
                    // 获取父元素的父元素，一般来说应该是一个容器，比如一个 Grid 或者另一个 StackPanel
                    Panel parentContainer = (Panel)parentStackPanel.Parent;

                    // 从父容器中移除父 StackPanel
                    if (parentContainer != null)
                    {
                        parentContainer.Children.Remove(parentStackPanel);
                    }
                }
                #endregion
                #region 删除AIS对象和piece对象
                Viewer.viewer.Erase(thePiece.shape, true);
                CurrentPlate.Slices.Remove(thePiece);
                #endregion
            };

            // 将子元素添加到 Grid 中，并指定列索引
            Grid.SetColumn(PieceLabel, 0);
            Grid.SetColumn(selectButton, 1);
            Grid.SetColumn(deletButton, 2);

            // 将子元素添加到 StackPanel 的 Children 集合中
            outerStackPanel.Children.Add(PieceLabel);
            outerStackPanel.Children.Add(selectButton);
            outerStackPanel.Children.Add(deletButton);
            //显示
            Viewer.viewer.Display(thePiece.shape, false);
            return outerStackPanel;
        }
        #endregion

        private void MakeVerticalPlate_Button_Click(object sender, RoutedEventArgs e)
        {
            #region 测试用代码，之后删除
            if (InputWorkpiece == null)
            {
                InputWorkpiece = OCCTK.Laser.WMakeSimpleClamp.TestInputWorkpiece("mods\\mytest.STEP");
                Viewer.Display(InputWorkpiece, true);
            }
            if (BasePlate == null)
            {
                MakeBasePlate();
            }
            #endregion
            #region 更新界面的逻辑
            clearPlates();
            #endregion
            if (InputWorkpiece != null && BasePlate != null)
            {
                Viewer.viewer.EraseAll();
                Viewer.Display(InputWorkpiece, true);
                Viewer.Display(BasePlate.shape, true);

                MakeVerticalPlate();
            }
            Viewer.FitAll();
        }
        private void MakeBasePlate()
        {
            double OffsetZ = 300.0;
            double BasePlateOffsetX = 10.0;
            double BasePlateOffsetY = 10.0;
            BasePlate = WMakeSimpleClamp.MakeBase_NoSelect(InputWorkpiece, OffsetZ, BasePlateOffsetX, BasePlateOffsetY);
            Viewer.Display(BasePlate.shape, true);
            //根据计算结果，设置推荐值
            InitialOffsetXParameter = Math.Round(BasePlate.dX * 0.1);
            InitialOffsetXLabel.TextBox.Text = InitialOffsetXParameter.ToString();
            InitialOffsetYParameter = Math.Round(BasePlate.dY * 0.1);
            InitialOffsetYLabel.TextBox.Text = InitialOffsetYParameter.ToString();
        }
        private void MakeVerticalPlate()
        {
            #region 创建竖板的逻辑

            OffsetXParameter = (int)Math.Floor((BasePlate.dX - InitialOffsetXParameter * 2) / (XNum - 1));
            OffsetYParameter = (int)Math.Floor((BasePlate.dY - InitialOffsetYParameter * 2) / (YNum - 1));

            double theXValue = Math.Round(BasePlate.X + InitialOffsetXParameter);
            double theYValue = Math.Round(BasePlate.Y + InitialOffsetYParameter);
            List<Piece> result = new List<Piece>();
            //沿着X方向创建竖板
            while (theXValue < BasePlate.X + BasePlate.dX)
            {
                VerticalPlate theVerticalPlateX = new VerticalPlate(InputWorkpiece, BasePlate, X, theXValue, ClearancesParameter, MinSupportingLenParameter, CuttingDistanceParameter);
                VerticalPlates.Add(theVerticalPlateX);
                PlateX_ComboBox.Items.Add(theVerticalPlateX);
                result.AddRange(theVerticalPlateX.Slices);
                theXValue = Math.Round(theXValue + OffsetXParameter);
            }
            //沿着Y方向创建竖板
            while (theYValue <= BasePlate.Y + BasePlate.dY)
            {
                VerticalPlate theVerticalPlateY = new VerticalPlate(InputWorkpiece, BasePlate, Y, theYValue, ClearancesParameter, MinSupportingLenParameter, CuttingDistanceParameter);
                VerticalPlates.Add(theVerticalPlateY);
                PlateY_ComboBox.Items.Add(theVerticalPlateY);
                result.AddRange(theVerticalPlateY.Slices);
                theYValue = Math.Round(theYValue + OffsetYParameter);
            }

            foreach (var item in result)
            {
                Viewer.Display(item.shape, true);
            }
            #endregion
        }

        private void clearPlates()
        {
            Plate_StackPanel.Children.Clear();

            DirectionComboBox.SelectedIndex = -1;
            PlateX_ComboBox.SelectedIndex = -1;
            PlateY_ComboBox.SelectedIndex = -1;
            PlateX_ComboBox.Visibility = Visibility.Collapsed;
            PlateY_ComboBox.Visibility = Visibility.Collapsed;
            PlateX_ComboBox.Items.Clear();
            PlateY_ComboBox.Items.Clear();
            CurrentPlateLocationTextBox.Text = null;
        }
        private void changeCurrentLocation(object sender, RoutedEventArgs e)
        {
            Button button = (Button)sender;
            VerticalPlate thePlate = (VerticalPlate)button.Tag;
            CurrentPlateLocationTextBox.Text = thePlate.Location.ToString();
        }



    }
}