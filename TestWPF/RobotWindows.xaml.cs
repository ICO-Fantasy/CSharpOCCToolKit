using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Forms.Integration;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;
using OCCViewForm;

namespace TestWPF;

/// <summary>
/// RobotWindows.xaml 的交互逻辑
/// </summary>
public partial class RobotWindows : Window
{
    public RobotWindows()
    {
        InitializeComponent();
        DataContext = new WorkSpace();
        // 创建 Windows Forms 控件和 WindowsFormsHost
        //CanvasHost.Child = ((App)Application.Current).canva;
        CanvasHost.Child = App.Current.canva;
    }
}
