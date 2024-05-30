using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Forms.Integration;
using OCCTK.Laser;
using OCCTK.OCC.AIS;
using OCCTK.OCC.gp;
using OCCTK.OCC.TopoDS;
using OCCViewForm;

namespace TestWPF
{
    /// <summary>
    /// 交互逻辑 MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private OCCCanvas Viewer;

        private WTopoDS_Shape _InputWorkpiece;
        private WAIS_Shape _AISInputWorkpiece;
        private WTopoDS_Shape InputWorkpiece
        {
            get { return _InputWorkpiece; }
            set
            {
                _InputWorkpiece = value;
                _AISInputWorkpiece = new WAIS_Shape(value);
            }
        }
        public WAIS_Shape AISInputWorkpiece
        {
            get
            {
                var a = _AISInputWorkpiece;
                a.SetColor(125, 125, 125);
                return a;
            }
            set
            {
                _AISInputWorkpiece = value;
                _InputWorkpiece = value.Shape();
            }
        }

        public WAIS_Shape AISBasePlate
        {
            get
            {
                WAIS_Shape a = new WAIS_Shape(BasePlate.shape);
                a.SetColor(0, 0, 255);
                return a;
            }
        }

        public WAIS_Shape AISCurrentPlate
        {
            get
            {
                WAIS_Shape a = new WAIS_Shape(CurrentPlate.shape);
                if (CurrentPlate.direction == Direction.X)
                {
                    a.SetColor(255, 0, 0);
                }
                if (CurrentPlate.direction == Direction.Y)
                {
                    a.SetColor(0, 255, 0);
                }
                return a;
            }
        }

        public List<VerticalPlate> VerticalPlates = new List<VerticalPlate>();
        public WTopoDS_Shape CombinedFixtureBoard;

        #region 创建横板参数
        /// <summary>
        /// 横板
        /// </summary>
        private BasePlate? BasePlate;

        public double BasePlateOffsetX { get; set; } = 5;

        public double BasePlateOffsetY { get; set; } = 5;

        public double BasePlateOffsetZ { get; set; } = 100.0;

        #endregion

        #region 创建竖板参数
        public int XNum { get; set; } = 3;

        public int YNum { get; set; } = 3;

        //X方向
        /// <summary>
        /// 竖板横向初始偏移
        /// </summary>
        public double InitialOffsetXParameter { get; set; } = 5;

        /// <summary>
        /// 竖板横向偏移
        /// </summary>
        public double OffsetXParameter { get; set; } = 20;

        //Y方向
        /// <summary>
        /// 竖板纵向初始偏移
        /// </summary>
        public double InitialOffsetYParameter { get; set; } = 5;

        /// <summary>
        /// 竖板纵向偏移
        /// </summary>
        public double OffsetYParameter { get; set; } = 20;

        #endregion

        #region 竖板全局参数
        //Z方向
        /// <summary>
        /// 竖板连接高
        /// </summary>
        public double ConnectionHeightParameter { get; set; } = 20;

        /// <summary>
        /// 竖板最小支撑长度
        /// </summary>
        public double MinSupportingLenParameter { get; set; } = 5;

        //XY方向
        /// <summary>
        /// 竖板避让间隙
        /// </summary>
        public double ClearancesParameter { get; set; } = 4;

        /// <summary>
        /// 竖板切断距离
        /// </summary>
        public double CuttingDistanceParameter { get; set; } = 500;

        /// <summary>
        /// 连接卡槽宽度
        /// </summary>
        public double ConnectionThicknessParameter { get; set; } = 5.0;
        public double FilletRadiusParameter { get; set; } = 2.0;

        #endregion

        #region 单块板属性

        public VerticalPlate? CurrentPlate { get; set; }

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
            #region 全局属性绑定

            BasePlateOffsetX_TextBox.Text = BasePlateOffsetX.ToString();
            BasePlateOffsetX_TextBox.TextChanged += BasePlateOffsetX_TextChanged;

            BasePlateOffsetY_TextBox.Text = BasePlateOffsetY.ToString();
            BasePlateOffsetY_TextBox.TextChanged += BasePlateOffsetY_TextChanged;

            BasePlateHight_TextBox.Text = BasePlateOffsetZ.ToString();
            BasePlateOffsetY_TextBox.TextChanged += BasePlateOffsetZ_TextChanged;

            InitialOffsetX_TextBox.Text = InitialOffsetXParameter.ToString();
            InitialOffsetX_TextBox.TextChanged += InitialOffsetX_TextChanged;

            // 设置OffsetXLabel的内容和TextBox的初始值，并添加TextChanged事件处理程序
            OffsetX_TextBox.Text = OffsetXParameter.ToString();
            OffsetX_TextBox.TextChanged += OffsetX_TextChanged;

            // 设置InitialOffsetYLabel的内容和TextBox的初始值，并添加TextChanged事件处理程序
            InitialOffsetY_TextBox.Text = InitialOffsetYParameter.ToString();
            InitialOffsetY_TextBox.TextChanged += InitialOffsetY_TextChanged;

            // 设置OffsetYLabel的内容和TextBox的初始值，并添加TextChanged事件处理程序
            OffsetY_TextBox.Text = OffsetYParameter.ToString();
            OffsetY_TextBox.TextChanged += OffsetY_TextChanged;

            // 设置ConnectionHeightLabel的内容和TextBox的初始值，并添加TextChanged事件处理程序
            ConnectionHeight_TextBox.Text = ConnectionHeightParameter.ToString();
            ConnectionHeight_TextBox.TextChanged += ConnectionHeight_TextChanged;

            // 设置MinSupportingLenLabel的内容和TextBox的初始值，并添加TextChanged事件处理程序
            MinSupportingLen_TextBox.Text = MinSupportingLenParameter.ToString();
            MinSupportingLen_TextBox.TextChanged += MinSupportingLen_TextChanged;

            // 设置ClearancesLabel的内容和TextBox的初始值，并添加TextChanged事件处理程序
            Clearances_TextBox.Text = ClearancesParameter.ToString();
            Clearances_TextBox.TextChanged += Clearances_TextChanged;

            // 设置CuttingDistanceLabel的内容和TextBox的初始值，并添加TextChanged事件处理程序
            CuttingDistance_TextBox.Text = CuttingDistanceParameter.ToString();
            CuttingDistance_TextBox.TextChanged += CuttingDistance_TextChanged;

            ConnectionThickness_TextBox.Text = ConnectionThicknessParameter.ToString();
            ConnectionThickness_TextBox.TextChanged += ConnectionThickness_TextChanged;

            FilletRadius_TextBox.Text = FilletRadiusParameter.ToString();
            FilletRadius_TextBox.TextChanged += FilletRadius_TextChanged;
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

        #region 测试
        private void FitAll_Button_Click(object sender, RoutedEventArgs e)
        {
            Viewer.FitAll();
        }

        private void Test_Button_Click(object sender, RoutedEventArgs e)
        {
            InputWorkpiece = Viewer.viewer.TestMakeBox().Shape();
            Viewer.Display(AISInputWorkpiece, true);
        }
        #endregion

        #region 导入
        private void Test_Input_Button_Click(object sender, RoutedEventArgs e)
        {
            Viewer.viewer.EraseAll();
            //InputWorkpiece = SimpleClampMaker.TestInputWorkpiece("mods\\mytest.STEP");
            //InputWorkpiece = SimpleClampMaker.TestInputWorkpiece("mods\\test1Small.STEP");
            InputWorkpiece = SimpleClampMaker.TestInputWorkpiece("mods\\test4Small.STEP");
            BasePlate = null;
            Viewer.Display(AISInputWorkpiece, true);
            Viewer.FitAll();
        }

        private void Test_input_test_1_Click(object sender, RoutedEventArgs e)
        {
            Viewer.viewer.EraseAll();
            InputWorkpiece = SimpleClampMaker.TestInputWorkpiece("mods\\test1.stp");
            BasePlate = null;
            Viewer.Display(AISInputWorkpiece, true);
            Viewer.FitAll();
        }

        private void Test_input_test_2_Click(object sender, RoutedEventArgs e)
        {
            Viewer.viewer.EraseAll();
            InputWorkpiece = SimpleClampMaker.TestInputWorkpiece("mods\\test2.stp");
            BasePlate = null;
            Viewer.Display(AISInputWorkpiece, true);
            Viewer.FitAll();
        }

        private void Test_input_test_3_Click(object sender, RoutedEventArgs e)
        {
            Viewer.viewer.EraseAll();
            InputWorkpiece = SimpleClampMaker.TestInputWorkpiece("mods\\test3.STEP");
            BasePlate = null;
            Viewer.Display(AISInputWorkpiece, true);
            Viewer.FitAll();
        }

        private void Test_input_test_4_Click(object sender, RoutedEventArgs e)
        {
            Viewer.viewer.EraseAll();
            InputWorkpiece = SimpleClampMaker.TestInputWorkpiece("mods\\test4.stp");
            BasePlate = null;
            Viewer.Display(AISInputWorkpiece, true);
            Viewer.FitAll();
        }

        private void Test_input_test_5_Click(object sender, RoutedEventArgs e)
        {
            Viewer.viewer.EraseAll();
            InputWorkpiece = SimpleClampMaker.TestInputWorkpiece("mods\\test5.stp");
            BasePlate = null;
            Viewer.Display(AISInputWorkpiece, true);
            Viewer.FitAll();
        }

        #endregion

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

        #region 监听事件
        private void BasePlateOffsetX_TextChanged(object sender, EventArgs e)
        {
            // 获取TextBox的新值
            TextBox textBox = (TextBox)sender;
            double newValue;
            if (double.TryParse(textBox.Text, out newValue))
            {
                // 更新 BasePlateOffsetX 的值
                BasePlateOffsetX = newValue;
            }
        }

        private void BasePlateOffsetY_TextChanged(object sender, EventArgs e)
        {
            // 获取TextBox的新值
            TextBox textBox = (TextBox)sender;
            double newValue;
            if (double.TryParse(textBox.Text, out newValue))
            {
                // 更新 BasePlateOffsetY 的值
                BasePlateOffsetY = newValue;
            }
        }

        private void BasePlateOffsetZ_TextChanged(object sender, EventArgs e)
        {
            // 获取TextBox的新值
            TextBox textBox = (TextBox)sender;
            double newValue;
            if (double.TryParse(textBox.Text, out newValue))
            {
                // 更新 BasePlateOffsetZ 的值
                BasePlateOffsetZ = newValue;
            }
        }

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

        private void ConnectionThickness_TextChanged(object sender, EventArgs e)
        {
            // 获取TextBox的新值
            TextBox textBox = (TextBox)sender;
            int newValue;
            if (int.TryParse(textBox.Text, out newValue))
            {
                // 更新XNum的值
                ConnectionThicknessParameter = newValue;
                if (FilletRadiusParameter > newValue / 2)
                {
                    FilletRadiusParameter = Math.Round(ConnectionThicknessParameter / 2);
                }
            }
        }

        private void FilletRadius_TextChanged(object sender, EventArgs e)
        {
            // 获取TextBox的新值
            TextBox textBox = (TextBox)sender;
            int newValue;
            if (int.TryParse(textBox.Text, out newValue))
            {
                // 更新XNum的值
                FilletRadiusParameter = newValue;
            }
        }

        private void CurrentPlateDirection_ComboBox_SelectionChanged(
            object sender,
            SelectionChangedEventArgs e
        )
        {
            ComboBoxItem seletedItem = (ComboBoxItem)CurrentPlateDirection_ComboBox.SelectedItem;
            string selectedDirection = "";
            if (seletedItem != null)
            {
                selectedDirection = seletedItem.Content.ToString();
            }

            if (selectedDirection == "X")
            {
                CurrentPlateLocationX_ComboBox.Visibility = Visibility.Visible;
                CurrentPlateLocationY_ComboBox.Visibility = Visibility.Collapsed;
            }
            else if (selectedDirection == "Y")
            {
                CurrentPlateLocationX_ComboBox.Visibility = Visibility.Collapsed;
                CurrentPlateLocationY_ComboBox.Visibility = Visibility.Visible;
            }
        }

        private void CurrentPlateLocationX_ComboBox_SelectionChanged(
            object sender,
            SelectionChangedEventArgs e
        )
        {
            //清空列表
            CurrentPlate_StackPanel.Children.Clear();
            CurrentPlate = (VerticalPlate)CurrentPlateLocationX_ComboBox.SelectedItem;
            if (CurrentPlate != null)
            {
                //显示选中板的坐标
                CurrentPlateSelectedLocation_TextBox.Text = CurrentPlate.location.ToString("F1");
                //清空画布并重新显示
                Viewer.viewer.EraseAll();
                Viewer.viewer.Display(AISInputWorkpiece, false);
                Viewer.viewer.Display(AISBasePlate, false);
                if (CurrentPlate.shape != null)
                {
                    Viewer.viewer.Display(AISCurrentPlate, true);
                    return;
                }
                foreach (var item in CurrentPlate.pieces)
                {
                    CurrentPlate_StackPanel.Children.Add(MakeStackItem(item));
                }
                Viewer.viewer.UpdateCurrentViewer();
            }
        }

        private void CurrentPlateLocationY_ComboBox_SelectionChanged(
            object sender,
            SelectionChangedEventArgs e
        )
        {
            //清空列表
            CurrentPlate_StackPanel.Children.Clear();
            //更新当前选中的板
            CurrentPlate = (VerticalPlate)CurrentPlateLocationY_ComboBox.SelectedItem;
            if (CurrentPlate != null)
            {
                //显示选中板的坐标
                CurrentPlateSelectedLocation_TextBox.Text = CurrentPlate.location.ToString("F1");
                //清空画布并重新显示
                Viewer.viewer.EraseAll();
                Viewer.viewer.Display(AISInputWorkpiece, false);
                WAIS_Shape theshape = new(BasePlate.shape);
                Viewer.viewer.Display(theshape, false);
                if (CurrentPlate.shape != null)
                {
                    Viewer.viewer.Display(AISCurrentPlate, true);
                    return;
                }
                foreach (var item in CurrentPlate.pieces)
                {
                    CurrentPlate_StackPanel.Children.Add(MakeStackItem(item));
                }
                Viewer.viewer.UpdateCurrentViewer();
            }
        }

        //单板

        private Grid MakeStackItem(VerticalPiece thePiece)
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
            PieceLabel.Content = "位置: " + thePiece.ToString("F2");
            PieceLabel.HorizontalContentAlignment = HorizontalAlignment.Left; // 设置 Label 的水平内容对齐方式为 Left

            Button selectButton = new Button();
            selectButton.Content = "选中";
            selectButton.Click += (sender, e) =>
            {
                Viewer.viewer.SelectAIS(thePiece.aisShape);
            };

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
                Viewer.viewer.Erase(thePiece.aisShape, true);
                CurrentPlate.pieces.Remove(thePiece);
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
            Viewer.viewer.Display(thePiece.aisShape, false);
            return outerStackPanel;
        }
        #endregion
        //更新当前板坐标
        private void UpdateCurrentPlateLocation_Button_Click(object sender, RoutedEventArgs e)
        {
            // 获取TextBox的新值
            double newValue;
            if (CurrentPlate != null)
            {
                //更新视图显示
                foreach (var item in CurrentPlate.pieces)
                {
                    Viewer.viewer.Erase(item.aisShape, false);
                }
                Viewer.viewer.UpdateCurrentViewer();
                // 更新CurrentPlateValueLable的值
                if (double.TryParse(CurrentPlateAddLocation_TextBox.Text, out newValue))
                {
                    CurrentPlate.location = newValue;
                }
                //更新stack中的元素
                CurrentPlate_StackPanel.Children.Clear();
                foreach (var item in CurrentPlate.pieces)
                {
                    CurrentPlate_StackPanel.Children.Add(MakeStackItem(item));
                }
                //显示新创建的板
                foreach (var item in CurrentPlate.pieces)
                {
                    Viewer.viewer.Display(item.aisShape, theToUpdateViewer: false);
                }
                Viewer.viewer.UpdateCurrentViewer();
            }
        }

        //添加板
        private void addPlate_Button_Click(object sender, RoutedEventArgs e)
        {
            VerticalPlate newPLate;
            // 尝试将输入的文本转换为 double 类型
            if (!double.TryParse(CurrentPlateAddLocation_TextBox.Text, out double addedLocation))
            {
                // 转换失败，则清除文本
                CurrentPlateAddLocation_TextBox.Text = "输入不合法";
            }
            if (CurrentPlateLocationX_ComboBox.IsVisible)
            {
                newPLate = SimpleClampMaker.MakeVerticalPlate(
                    InputWorkpiece,
                    BasePlate,
                    Direction.X,
                    addedLocation,
                    ClearancesParameter,
                    MinSupportingLenParameter,
                    CuttingDistanceParameter
                );
                CurrentPlateLocationX_ComboBox.Items.Add(newPLate);
                // 设置 ComboBox 的 SelectedItem 为新添加的项
                CurrentPlateLocationX_ComboBox.SelectedItem = newPLate;
                VerticalPlates.Add(newPLate);

                // 获取 ComboBox 的 ItemsSource
                var items =
                    CurrentPlateLocationX_ComboBox.ItemsSource
                    as ObservableCollection<VerticalPlate>;

                // 对集合进行排序（示例中按照字符串排序，你可以根据需要修改比较器）
                if (items != null)
                {
                    var sortedItems = new ObservableCollection<VerticalPlate>(
                        items.OrderBy(item => double.Parse(item.ToString()))
                    );

                    // 更新 ComboBox 的 ItemsSource
                    CurrentPlateLocationX_ComboBox.ItemsSource = sortedItems;

                    // 如果需要，选中排序后的第一个项
                    CurrentPlateLocationX_ComboBox.SelectedIndex = 0;
                }
            }
            if (CurrentPlateLocationY_ComboBox.IsVisible)
            {
                newPLate = SimpleClampMaker.MakeVerticalPlate(
                    InputWorkpiece,
                    BasePlate,
                    Direction.Y,
                    addedLocation,
                    ClearancesParameter,
                    MinSupportingLenParameter,
                    CuttingDistanceParameter
                );
                CurrentPlateLocationY_ComboBox.Items.Add(newPLate);
                // 设置 ComboBox 的 SelectedItem 为新添加的项
                CurrentPlateLocationY_ComboBox.SelectedItem = newPLate;
                VerticalPlates.Add(newPLate);

                // 获取 ComboBox 的 ItemsSource
                var items =
                    CurrentPlateLocationY_ComboBox.ItemsSource
                    as ObservableCollection<VerticalPlate>;

                // 对集合进行排序（示例中按照字符串排序，你可以根据需要修改比较器）
                if (items != null)
                {
                    var sortedItems = new ObservableCollection<VerticalPlate>(
                        items.OrderBy(item => double.Parse(item.ToString()))
                    );

                    // 更新 ComboBox 的 ItemsSource
                    CurrentPlateLocationY_ComboBox.ItemsSource = sortedItems;
                    // 如果需要，选中排序后的第一个项
                    CurrentPlateLocationY_ComboBox.SelectedIndex = 0;
                }
            }
        }

        private void SetXYNum_Button_Click(object sender, RoutedEventArgs e)
        {
            XNum_StackPanel.Visibility = Visibility.Visible;
            YNum_StackPanel.Visibility = Visibility.Visible;
            OffsetX_StackPanel.Visibility = Visibility.Collapsed;
            OffsetY_StackPanel.Visibility = Visibility.Collapsed;
        }

        private void SetXYOffset_Button_Click(object sender, RoutedEventArgs e)
        {
            XNum_StackPanel.Visibility = Visibility.Collapsed;
            YNum_StackPanel.Visibility = Visibility.Collapsed;
            OffsetX_StackPanel.Visibility = Visibility.Visible;
            OffsetY_StackPanel.Visibility = Visibility.Visible;
            XNum = 0;
            XNum_TextBox.Text = "0";
            YNum = 0;
            YNum_TextBox.Text = "0";
            OffsetX_TextBox.Text = OffsetXParameter.ToString();
            OffsetY_TextBox.Text = OffsetYParameter.ToString();
        }

        //创建竖板
        private void MakeVerticalPlate_Button_Click(object sender, RoutedEventArgs e)
        {
            #region 测试用代码，之后删除
            if (InputWorkpiece == null)
            {
                InputWorkpiece = SimpleClampMaker.TestInputWorkpiece("mods\\mytest.STEP");
                Viewer.Display(AISInputWorkpiece, true);
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
                Viewer.Display(AISInputWorkpiece, true);
                Viewer.Display(AISBasePlate, true);

                VerticalPlates = SimpleClampMaker.MakeVerticalPlates(
                    InputWorkpiece,
                    BasePlate,
                    XNum,
                    YNum,
                    InitialOffsetXParameter,
                    InitialOffsetYParameter,
                    ClearancesParameter,
                    MinSupportingLenParameter,
                    CuttingDistanceParameter
                );
                UpdateComboBox();
                foreach (var onePlate in VerticalPlates)
                {
                    foreach (var onePiece in onePlate.pieces)
                    {
                        Viewer.Display(onePiece.aisShape, false);
                    }
                }
            }
            Viewer.Update();
            Viewer.FitAll();
        }

        //创建底板
        private void MakeBasePlate_Button_Clcik(object sender, RoutedEventArgs e)
        {
            Viewer.viewer.EraseAll();
            if (AISInputWorkpiece == null)
            {
                InputWorkpiece = SimpleClampMaker.TestInputWorkpiece("mods\\mytest.STEP");
            }
            Viewer.Display(AISInputWorkpiece, true);
            MakeBasePlate();
        }

        //连接竖板
        private void ConnectPlate_Button_Click(object sender, RoutedEventArgs e)
        {
            testconnect();
            UpdateComboBox();
            //重新显示更新后的shape
            if (AISInputWorkpiece != null && AISBasePlate != null)
            {
                Viewer.viewer.EraseAll();
                Viewer.Display(AISInputWorkpiece, true);
                Viewer.Display(AISBasePlate, true);

                foreach (var onePlate in VerticalPlates)
                {
                    Viewer.Display(onePlate.aisShape, true);
                }
            }
            Viewer.FitAll();
        }

        private void testconnect()
        {
            VerticalPlates = SimpleClampMaker.SuturePLates(
                VerticalPlates,
                BasePlate,
                ConnectionHeightParameter,
                ConnectionThicknessParameter,
                FilletRadiusParameter
            );
        }

        private void MakeBasePlate()
        {
            BasePlate = SimpleClampMaker.MakeBasePlate_NoInteract(
                InputWorkpiece,
                BasePlateOffsetZ,
                BasePlateOffsetX,
                BasePlateOffsetY
            );
            Viewer.Display(AISBasePlate, true);
            //根据计算结果，设置推荐值
            InitialOffsetXParameter = Math.Round(BasePlate.dX * 0.1);
            InitialOffsetX_TextBox.Text = InitialOffsetXParameter.ToString("F1");
            InitialOffsetYParameter = Math.Round(BasePlate.dY * 0.1);
            InitialOffsetY_TextBox.Text = InitialOffsetYParameter.ToString("F1");
        }

        private void UpdateComboBox()
        {
            clearPlates();
            foreach (var onePlate in VerticalPlates)
            {
                if (onePlate.direction == Direction.X)
                {
                    CurrentPlateLocationX_ComboBox.Items.Add(onePlate);
                }
                if (onePlate.direction == Direction.Y)
                {
                    CurrentPlateLocationY_ComboBox.Items.Add(onePlate);
                }
            }
        }

        private void clearPlates()
        {
            CurrentPlate_StackPanel.Children.Clear();

            CurrentPlateDirection_ComboBox.SelectedIndex = -1;
            CurrentPlateLocationX_ComboBox.SelectedIndex = -1;
            CurrentPlateLocationY_ComboBox.SelectedIndex = -1;
            CurrentPlateLocationX_ComboBox.Visibility = Visibility.Collapsed;
            CurrentPlateLocationY_ComboBox.Visibility = Visibility.Collapsed;
            CurrentPlateLocationX_ComboBox.Items.Clear();
            CurrentPlateLocationY_ComboBox.Items.Clear();
            CurrentPlateSelectedLocation_TextBox.Text = null;
        }

        private void changeCurrentLocation(object sender, RoutedEventArgs e)
        {
            Button button = (Button)sender;
            VerticalPlate thePlate = (VerticalPlate)button.Tag;
            CurrentPlateSelectedLocation_TextBox.Text = thePlate.location.ToString("F1");
        }

        private void PermutationPlate_Button_Click(object sender, RoutedEventArgs e)
        {
            //重新显示更新后的shape
            if (InputWorkpiece != null && BasePlate != null)
            {
                Viewer.viewer.EraseAll();
            }
            CombinedFixtureBoard = SimpleClampMaker.DeployPlates(VerticalPlates, BasePlate);
            Viewer.Display(new WAIS_Shape(CombinedFixtureBoard), true);
            Viewer.FitAll();
        }

        private void OutputSTEP_Button_Click(object sender, RoutedEventArgs e)
        {
            SimpleClampMaker.SaveSTEP(CombinedFixtureBoard, "mods\\test_output.STEP");
        }
    }
}
