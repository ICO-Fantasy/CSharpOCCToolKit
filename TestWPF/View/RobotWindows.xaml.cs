using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Runtime.Serialization;
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

using DevExpress.Xpf.Bars;

using log4net;

using MathNet.Numerics.LinearAlgebra;

using Microsoft.Win32;

using Newtonsoft.Json;

using OCCTK.Extension;
using OCCTK.IO;
using OCCTK.OCC.AIS;
using OCCTK.OCC.BRepPrimAPI;
using OCCTK.OCC.gp;
using OCCTK.OCC.Image;
using OCCTK.OCC.Topo;
using OCCTK.OCC.V3d;
using OCCTK.Utils;

using OCCViewForm;

using TestWPF.ViewModel;

namespace TestWPF;

/// <summary>
/// RobotWindows.xaml 的交互逻辑
/// </summary>
public partial class RobotWindows:Window, ICameraChangedHandler {
	private static readonly ILog log = LogManager.GetLogger(typeof(RobotWindows));
	public RobotWindows( ) {
		InitializeComponent( );
		//DataContext = new WorkSpace();
		// 创建 Windows Forms 控件和 WindowsFormsHost
		//CanvasHost.Child = ((App)Application.Current).canva;
		if( App.Current.ContextManager.MainContext == null ) {
			var context = App.Current.ContextManager.CreateContext();
			OCCCanvas = new OCCCanvas(context);
		} else {
			OCCCanvas = new OCCCanvas(App.Current.ContextManager.MainContext);
		}
		CanvasHost.Child = OCCCanvas;
		DataContext = new RobotWindowsViewModel( );
		OCCCanvas.OnCameraChangedEvent += OnCameraChanged;
	}

	private OCCCanvas OCCCanvas { get; }

	public void OnCameraChanged( ) {
		UpdateBottom_StatusBar_Info( );
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

	private void Test_Test1_Button_Click( object sender, RoutedEventArgs e ) {
		MakeBox box = new(10, 10, 10);
		AShape aisBox = new(box);
		ThreeDimensionContext context =
			App.Current.ContextManager.MainContext ?? throw new Exception("没context");
		InteractiveContext aisContext = context.AISContext;
		OCCTK.OCC.V3d.View myView = context.ViewList[0].View;
		context.Display(aisBox);

		List<Trsf> TrsfList = new List<Trsf>()
		{
			new(new Pnt(),new(-1, 0, 0)),
			new(new Pnt(),new(1, 0, 0)),
			new(new Pnt(),new(2, 0, 0)),
			new(new Pnt(),new(3, 0, 0)),
			new(new Pnt(),new(4, 0, 0)),
			new(new Pnt(),new(5, 0, 0)),
			new(new Pnt(),new(5, 1, 0)),
			new(new Pnt(),new(5, 2, 0)),
			new(new Pnt(),new(5, 3, 0)),
			new(new Pnt(),new(5, 4, 0)),
			new(new Pnt(),new(5, 5, 0)),
		};

		#region Serilization

		// 创建一个 Ax1 实例
		var pnt = new Trsf(new Ax2(new(0,0,0),new Dir(1,0,0)),new Ax2(new(1,2,3),new Dir(0,1,0)));
		// 序列化对象到 JSON
		//string json = JsonConvert.SerializeObject(pnt);
		string jsonPath=System.Text.Json.JsonSerializer.Serialize(pnt);
		// 写入到文件 TestSerializable.txt
		string filePath = "TestSerializable.txt";
		//File.WriteAllText(filePath, json);
		log.Debug($"序列化信息写入文件： {filePath}");
		//log.Info($"Json:\n {json}");
		log.Info($"jsonPath: {jsonPath}");
		// 反序列化对象
		//var deserializedAx1 = JsonConvert.DeserializeObject<Trsf>(json);
		var weatherForecast =
				System.Text.Json.JsonSerializer.Deserialize<Trsf>(jsonPath);
		// 打印反序列化后的数据
		log.Debug("反序列化");
		//log.Info($"Trsf: {deserializedAx1}");
		log.Info($"DeTrsf: {weatherForecast}");

		#endregion

		#region 视频录制

		//myView.RecordTest(aisContext, aisBox, "testvideo.mkv", TrsfList, context.ViewList[0].Width, context.ViewList[0].Height);

		//// 设置视频录制参数，例如视频格式、分辨率和帧率。
		//VideoParams aRecParams=new();
		//aRecParams.Width = context.ViewList[0].Width;   // 视频的宽度，取自窗口大小。
		//aRecParams.Height = context.ViewList[0].Height;  // 视频的高度，取自窗口大小。
		//aRecParams.FpsNum = 3;            // 帧率的分子，表示每秒3帧。
		//aRecParams.Format = "matroska";    // 视频文件格式，这里指定为 Matroska (.mkv) 格式。
		//aRecParams.PixelFormat = "yuv420p"; // 像素格式，常用于视频编码以压缩存储。
		//aRecParams.VideoCodec = "mjpeg";   // 视频编码器，这里使用 MJPEG（Motion JPEG）。
		//								   // 创建一个视频录制器对象，用于管理视频的录制过程。
		//VideoRecorder aRecorder = new ( );
		//// 打开视频录制器，指定输出文件名和录制参数。
		//if( !aRecorder.Open("D:\\Download\\video.mkv", aRecParams) ) {
		//	log.Debug("打开失败");
		//}

		//// 开始动画循环，录制动画为视频。
		//for( int aNbFrames = 0; aNbFrames < TrsfList.Count; aNbFrames++ ) {

		//	// 计算当前帧的展示时间戳（PTS，Presentation Timestamp）。
		//	double aPts =(double)aRecParams.FpsDen /(double)aRecParams.FpsNum* (double)aNbFrames; // PTS = 时间间隔 × 帧数

		//	// 更新动画到当前时间点。
		//	aisBox.SetLocalTransformation(TrsfList[aNbFrames]);
		//	aisContext.Redisplay(aisBox, true);

		//	// 转储当前帧内容为像素数据。
		//	ImageDumpOptions aDumpParams=ImageDumpOptions.Default;
		//	aDumpParams.Width = aRecParams.Width;  // 帧的宽度，等于视频宽度。
		//	aDumpParams.Height = aRecParams.Height; // 帧的高度，等于视频高度。
		//	aDumpParams.BufferType = 1; // 图像缓冲区类型为RGBA。
		//	aDumpParams.ToAdjustAspect = true;      // 调整宽高比以匹配目标分辨率。

		//	// 从视图中捕获当前帧的像素数据，并将其保存到视频录制器的缓冲区。
		//	if( !myView.ToPixMap(aRecorder.ChangeFrame( ), aDumpParams) ) {
		//		throw new Exception("转储失败");
		//	}

		//	// 将帧的像素数据沿Y轴翻转。
		//	PixMap.FlipY(aRecorder.ChangeFrame( ));

		//	// 将当前帧推送到视频录制器以存入视频文件。
		//	if( !aRecorder.PushFrame( ) ) { break; } // 如果推送失败，退出循环。
		//}
		#endregion



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

	private void UpdateBottom_StatusBar_Info( ) {
		OCCTK.OCC.V3d.View view = App.Current.ContextManager.MainContext.ViewList[0].View;
		if( view == null ) {
			return;
		}
		CameraOrientation c = view.CurrentViewOrientation();
		string data =
			$"Scale: {c.Scale} | Eye: {c.Eye} | Prj: {c.Projection} | Aspect: {c.Aspect} | HightPoint: {c.HightPoint} | ScreenCenter: {c.ScreenCenter} | Size: {c.Size} | ViewPoint: {c.ViewPoint} ";
		Last_StatusBar.Text = Bottom_StatusBar.Text;
		Bottom_StatusBar.Text = data;
	}
}
