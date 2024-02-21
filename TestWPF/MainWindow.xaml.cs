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
        public MainWindow()
        {
            InitializeComponent();
            // 创建 Windows Forms 控件和 WindowsFormsHost
            WindowsFormsHost aHost = new WindowsFormsHost();
            OCCViewer aForm = new OCCViewer();
            aForm.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            aForm.Show();
            aHost.Child = aForm;
            main_grid.Children.Add(aHost);
        }
    }
}