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
using System.Windows.Interop;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace OCCFramework;

/// <summary>
/// OCCCanvasView.xaml 的交互逻辑
/// </summary>
public partial class OCCCanvasView : System.Windows.Controls.UserControl
{
    private OCCTK.OCC.V3d.View _mainView;

    public OCCCanvasView()
    {
        InitializeComponent();
        DataContext = new OCCCanvasViewModel();

        // 创建一个 Windows Forms 的 Panel 并将其嵌入到 WindowsFormsHost 中
        System.Windows.Forms.Panel winFormsPanel = new();
        winFormsPanel.Paint += new System.Windows.Forms.PaintEventHandler(OnPaint);
        WindowsFormsHost formHost = new();
        canvasGrid.Children.Add(formHost);
        formHost.Child = winFormsPanel;

        // 获取 Windows Forms Panel 的句柄
        IntPtr windowHandle = winFormsPanel.Handle;
        _mainView = ((OCCCanvasViewModel)DataContext).CreateView(windowHandle);
        _mainView.SetDefault();
    }

    private void OnPaint(object? sender, PaintEventArgs e)
    {
        if (((OCCCanvasViewModel)DataContext)._context == null)
        {
            return;
        }
        if (_mainView == null)
        {
            return;
        }
        _mainView.Redraw();
        ((OCCCanvasViewModel)DataContext)._context.UpdateCurrentViewer();
    }
}
