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
using System.Windows.Forms;
using System.Windows.Forms.Integration;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Media.TextFormatting;
using System.Windows.Shapes;
using CommunityToolkit.Mvvm.ComponentModel;
using MathNet.Numerics.LinearAlgebra;
using Microsoft.Win32;
using OCCTK.Extension;
using OCCTK.IO;
using OCCTK.OCC.AIS;
using OCCTK.OCC.BRepPrimAPI;
using OCCTK.OCC.gp;
using OCCTK.OCC.Topo;
using OCCTK.Utils;
using OCCViewForm;
using TestWPF.ViewModel;

namespace TestWPF;

/// <summary>
/// RobotWindows.xaml 的交互逻辑
/// </summary>
public partial class RobotWindows : Window, ICameraChangedHandler
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
            OCCCanvas = new OCCCanvas(context);
        }
        else
        {
            OCCCanvas = new OCCCanvas(App.Current.ContextManager.MainContext);
        }
        CanvasHost.Child = OCCCanvas;
        DataContext = new RobotWindowsViewModel();
        OCCCanvas.OnCameraChangedEvent += OnCameraChanged;
    }

    private OCCCanvas OCCCanvas { get; }

    public void OnCameraChanged()
    {
        UpdateBottom_StatusBar_Info();
    }

    //private async Task AnimateBoxesAsync(
    //    List<Pnt> pnts,
    //    AShape aisBox,
    //    InteractiveContext aisContext
    //)
    //{
    //    foreach (var pnt in pnts)
    //    {
    //        await Task.Run(() =>
    //        {
    //            Trsf t = new();
    //            t.SetTranslation(new(new Pnt(), pnt));
    //            aisBox.SetLocalTransformation(t);
    //            if (aisContext.IsDisplayed(aisBox))
    //            {
    //                aisContext.Redisplay(aisBox, true);
    //            }
    //        });
    //    }
    //}

    private void Test_Test1_Button_Click(object sender, RoutedEventArgs e)
    {
        //MakeBox box = new(10, 10, 10);
        //AShape aisBox = new(box);
        //ThreeDimensionContext context =
        //    App.Current.ContextManager.MainContext ?? throw new Exception("2");
        //InteractiveContext aisContext = context.AISContext;
        //context.Display(aisBox);

        //List<Pnt> pnts = new List<Pnt>()
        //{
        //    new(-1, 0, 0),
        //    new(1, 0, 0),
        //    new(2, 0, 0),
        //    new(3, 0, 0),
        //    new(4, 0, 0),
        //    new(5, 0, 0),
        //    new(5, 1, 0),
        //    new(5, 2, 0),
        //    new(5, 3, 0),
        //    new(5, 4, 0),
        //    new(5, 5, 0),
        //};

        //OCCTK.IO.OBJExchange ex = new("D:\\Download\\output.obj");
        //App.Current.ContextManager.MainContext.Display(new AShape(ex), true);

        //// 创建文件选择对话框
        //OpenFileDialog openFileDialog = new OpenFileDialog
        //{
        //    //Filter = "STEP Files (*.STEP;*.step;*.stp)|*.STEP;*.step;*.stp|All Files (*.*)|*.*", // 设置文件过滤器
        //    InitialDirectory = System.IO.Path.Combine(
        //        AppDomain.CurrentDomain.BaseDirectory,
        //        "D:\\Download"
        //    ) // 设置初始目录为指定的路径
        //    //InitialDirectory = System.IO.Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "mods") // 设置初始目录为指定的路径
        //};

        //// 如果用户选择了文件并点击了“打开”按钮
        //if (openFileDialog.ShowDialog() == true)
        //{
        //    string selectedFilePath = openFileDialog.FileName; // 获取选择的文件路径
        //    //STEPExchange exchange = new STEPExchange(selectedFilePath); // 使用选择的文件路径
        //    OCCTK.IO.OBJExchange ex = new(selectedFilePath);
        //    App.Current.ContextManager.MainContext.Display(ex, true);
        //    //// 生成新的文件路径，修改后缀为 .xbf
        //    //// 获取文件目录
        //    //string directory =
        //    //    System.IO.Path.GetDirectoryName(selectedFilePath)
        //    //    ?? throw new Exception("文件目录获取失败");
        //    //// 获取文件名（无扩展名）
        //    //string fileNameWithoutExtension = System.IO.Path.GetFileNameWithoutExtension(
        //    //    selectedFilePath
        //    //);
        //    //// 组合成新路径01车
        //    //string docFilePath = System.IO.Path.Combine(
        //    //    directory,
        //    //    fileNameWithoutExtension + ".xbf"
        //    //);
        //    //exchange.DocTest(selectedFilePath, docFilePath);
        //    //OBJExchange ex = new(selectedFilePath);
        //    //try
        //    //{
        //    //    var shape = ex.Shape();
        //    //    AShape ais = new(shape.TopoShape);
        //    //    var context = App.Current.ContextManager.MainContext;
        //    //    context.Display(ex, true);
        //    //    context.Update();
        //    //}
        //    //catch { }
        //}
    }

    private void UpdateBottom_StatusBar_Info()
    {
        OCCTK.OCC.V3d.View view = App.Current.ContextManager.MainContext.ViewList[0].View;
        if (view == null)
        {
            return;
        }
        CameraOrientation c = view.CurrentViewOrientation();
        string data =
            $"Scale: {c.Scale} | Eye: {c.Eye} | Prj: {c.Projection} | Aspect: {c.Aspect} | HightPoint: {c.HightPoint} | ScreenCenter: {c.ScreenCenter} | Size: {c.Size} | ViewPoint: {c.ViewPoint} ";
        Last_StatusBar.Text = Bottom_StatusBar.Text;
        Bottom_StatusBar.Text = data;
    }
}
