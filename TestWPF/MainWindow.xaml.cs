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

namespace TestWPF
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private OCCViewForm.OCCCanvas Viewer;
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
        }
    }
}