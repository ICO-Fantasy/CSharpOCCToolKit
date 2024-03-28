using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Windows.Forms.Integration;
using OCCViewForm;
using OCCTK;
using OCCTK.OCC.AIS;
namespace TestWPF
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private OCCViewForm.OCCCanvas Viewer;
        private OCCTK.OCC.AIS.WAIS_Shape InputWorkpiece;
        private OCCTK.OCC.AIS.WAIS_Shape BasePlate;
        private List<OCCTK.OCC.AIS.WAIS_Shape> VerticalPlates;
        private OCCTK.OCC.AIS.WAIS_Shape VerticalPlate;
        private OCCTK.Laser.BasePlate testBase;
        public MainWindow()
        {
            InitializeComponent();
            // 创建 Windows Forms 控件和 WindowsFormsHost
            WindowsFormsHost aHost = new WindowsFormsHost();
            Viewer = new OCCViewForm.OCCCanvas();
            Viewer.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            Viewer.Show();
            aHost.Child = Viewer;
            canvas_grid.Children.Add(aHost);
        }

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

        private void FitAll_Button_Click(object sender, RoutedEventArgs e)
        {
            Viewer.FitAll();
        }
        private void Test_Button_Click(object sender, RoutedEventArgs e)
        {
            Viewer.MakeClampTest();
            //Viewer.Test();
        }
        private void Test_Input_Button_Click(object sender, RoutedEventArgs e)
        {
            //InputWorkpiece = OCCTK.Laser.WMakeSimpleClamp.TestInputWorkpiece("testWorkPiece.STEP");
            //InputWorkpiece = OCCTK.Laser.WMakeSimpleClamp.TestInputWorkpiece("test_b柱_5421231-ed01.STEP");
            InputWorkpiece = OCCTK.Laser.WMakeSimpleClamp.TestInputWorkpiece("testSmall.STEP");
            Viewer.Display(InputWorkpiece, true);
        }

        private void Test_input_test_2_Click(object sender, RoutedEventArgs e)
        {
            InputWorkpiece = OCCTK.Laser.WMakeSimpleClamp.TestInputWorkpiece("test2.STEP");
            Viewer.Display(InputWorkpiece, true);
        }
        private void Test_BasePlate_Button_Click(object sender, RoutedEventArgs e)
        {
            double OffsetZ = 300.0;
            double BasePlateOffsetX = 10.0;
            double BasePlateOffsetY = 10.0;
            double BasePlateThickness = 1.0;
            //double OffsetZ = 30.0;
            //double BasePlateOffsetX = 10.0;
            //double BasePlateOffsetY = 10.0;
            //double BasePlateThickness = 1.0;
            //OCCTK.OCC.AIS.WAIS_Shape InputAISFace = Viewer.GetSelectedShape();
            //testBase = OCCTK.Laser.WMakeSimpleClamp.TestMakeBase(InputWorkpiece, InputAISFace, OffsetZ, BasePlateThickness, BasePlateOffsetX, BasePlateOffsetY);
            try
            {
                testBase = OCCTK.Laser.WMakeSimpleClamp.TestMakeBase_NoSelect(InputWorkpiece, OffsetZ, BasePlateThickness, BasePlateOffsetX, BasePlateOffsetY);
                Viewer.Display(testBase.shape, true);
            }
            catch (Exception)
            {

            }
        }

        private void Test_MakeV_Button_Click(object sender, RoutedEventArgs e)
        {
            int XNum = 3;
            int YNum = 3;
            double maxVerticalLength = 20;

            double dx = (int)Math.Floor(testBase.dX / XNum);
            double dy = (int)Math.Floor(testBase.dY / YNum);

            double VerticalPlateClearances = 5.0;
            double VerticalPlateMinSupportingLen = 500.0;
            double VerticalPlateCuttingDistance = 20.0;
            double testXValue = testBase.X;
            double testYValue = testBase.Y;
            List<WAIS_Shape> result = new List<WAIS_Shape>();
            List<WAIS_Shape> resultX, resultY;

            while (testXValue <= testBase.X + testBase.dX)
            {
                testXValue += dx;
                resultX = OCCTK.Laser.WMakeSimpleClamp.TestMakeVertical(InputWorkpiece,
                                                                        testBase,
                                                                        OCCTK.Laser.VerticalPlateDirection.X,
                                                                        testXValue, maxVerticalLength,
                                                                        VerticalPlateClearances,
                                                                        VerticalPlateMinSupportingLen,
                                                                        VerticalPlateCuttingDistance);
                result.AddRange(resultX);
            }

            while (testYValue <= testBase.Y + testBase.dY)
            {
                testYValue += dy;
                resultY = OCCTK.Laser.WMakeSimpleClamp.TestMakeVertical(InputWorkpiece,
                                                                                  testBase,
                                                                                  OCCTK.Laser.VerticalPlateDirection.Y,
                                                                                  testYValue, maxVerticalLength,
                                                                                  VerticalPlateClearances,
                                                                                  VerticalPlateMinSupportingLen,
                                                                                  VerticalPlateCuttingDistance);
                result.AddRange(resultY);
            }

            foreach (var item in result)
            {
                Viewer.Display(item, true);
            }
        }

        private void Test_ConnectV_Button_Click(object sender, RoutedEventArgs e)
        {

        }

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


    }
}