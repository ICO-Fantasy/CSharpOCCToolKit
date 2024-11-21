using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
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
using System.Windows.Media.TextFormatting;
using System.Windows.Shapes;
using CommunityToolkit.Mvvm.ComponentModel;
using MathNet.Numerics.LinearAlgebra;
using OCCTK.OCC.AIS;
using OCCTK.OCC.BRepPrimAPI;
using OCCTK.Utils;
using OCCViewForm;
using TestWPF.ViewModel;

namespace TestWPF;

/// <summary>
/// RobotWindows.xaml 的交互逻辑
/// </summary>
public partial class RobotWindows : Window
{
    public RobotWindows()
    {
        InitializeComponent();
        //DataContext = new WorkSpace();
        // 创建 Windows Forms 控件和 WindowsFormsHost
        //CanvasHost.Child = ((App)Application.Current).canva;
        if (App.Current.ContextManager.MainContext == null)
        {
            var context = App.Current.ContextManager.CreateContext();
            CanvasHost.Child = new OCCCanvas(context);
        }
        else
        {
            CanvasHost.Child = new OCCCanvas(App.Current.ContextManager.MainContext);
        }
        DataContext = new RobotWindowsViewModel();
    }

    private void WorkSpace_Robot_Create_ItemClick(
        object sender,
        DevExpress.Xpf.Bars.ItemClickEventArgs e
    )
    {
        OCCTK.IO.OBJExchange ex = new("D:\\Download\\output.obj");
        App.Current.ContextManager.MainContext.Display(new AShape(ex), true);
    }
}
