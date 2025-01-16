using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Controls.Primitives;

using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;

using log4net;

using MathNet.Numerics;
using MathNet.Spatial.Units;

using Microsoft.Win32;

using OCCTK.Extension;
using OCCTK.OCC.AIS;
using OCCTK.OCC.BRepPrimAPI;
using OCCTK.OCC.gp;
using OCCTK.OCC.Topo;
using OCCTK.OCC.V3d;
using OCCTK.Utils;

using OCCViewForm;

using TestWPF.Robotics;

using Windows.System.Profile;

using View = OCCTK.OCC.V3d.View;

namespace TestWPF.ViewModel;

public partial class RobotWindowsViewModel:ObservableObject {
	private static readonly ILog log = LogManager.GetLogger(typeof(RobotWindowsViewModel));
	public RobotWindowsViewModel( ) {
		cube = new( );
		AISContext.Display(cube, true);

		BackgroundColorUp = FromHex("#f2f2f2");
		BackgroundColorDown = FromHex("#fbfbfb");

		AxesSize = 80.0;
		AxesRadius = 1.0;
		AxesConeRadius = 5.0;
		AxesSphereRadius = 5.0;
		EnableFaceBoundary = true;
		FaceBoundaryColor = FromHex("#396ba7");
		Font = "MSYHL";
		FontHeight = 50.0;
		BoxTextColor = FromHex("#000000");
		BoxColor = FromHex("#9accff");
		Transparency = 0.2;
		BoxFacetExtension = 10;
		BoxRoundRadius = 0.15;
		BoxEdgeGap = 4.0;
		Axis = DatumParts.XAxis;
		boxSideLabel = "X";
		AxisTextHeight = 20.0;
		XLabel = "X";
		YLabel = "Y";
		ZLabel = "Z";

		//ResetTriedronDisplay( size:80.0,  axesRadiusRatios:
		//1.0,  axesConeRadius:
		//5.0,  axesSphereRadius:
		//5.0,  axesFontHeight:
		//20.0, xColor:
		//new(255, 0, 0), yColor:
		//new(0, 0, 255), zColor:
		//new(0, 0, 255), enableBoundaryDraw:
		//true, faceBoundaryColor:
		//new("#396ba7"), textColor:, fontFamily:
		//"MSYHL",  boxFontHeight:
		//50.0, boxColor:
		//new(0, 0, 0),  boxTransparency:
		//0.2,  boxFaceSideLength:
		//10,boxRadiusRadio:
		//0.15,boxEdgeGap:
		//4.0) ;
	}

	private static ThreeDimensionContext ThreeDContext =>
		App.Current.ContextManager.MainContext ?? throw new Exception("请先创建OCC画布上下文");

	private OCCCanvas occCanvas => ThreeDContext.ViewList[0];
	private InteractiveContext AISContext => ThreeDContext.AISContext;

	CameraOrientation? currentViewStaute;
	CameraOrientation? CurrentViewStaute {
		get => currentViewStaute;
		set {
			currentViewStaute = value;
			Debug.WriteLine(
				$"Scale: {value.Scale} | Eye: {value.Eye} | Prj: {value.Projection} | Aspect: {value.Aspect} | HightPoint: {value.HightPoint} | ScreenCenter: {value.ScreenCenter} | Size: {value.Size} | ViewPoint: {value.ViewPoint} "
			);
		}
	}

	/// <summary>
	/// 背景颜色上
	/// </summary>
	[ObservableProperty]
	System.Windows.Media.Color backgroundColorUp;
	partial void OnBackgroundColorUpChanged( System.Windows.Media.Color value ) {
		occCanvas.View.SetBgGradientColors(new(value.R, value.G, value.B), new(BackgroundColorDown.R, BackgroundColorDown.G, BackgroundColorDown.B), true);
	}

	/// <summary>
	/// 背景颜色上
	/// </summary>
	[ObservableProperty]
	System.Windows.Media.Color backgroundColorDown;
	partial void OnBackgroundColorDownChanged( System.Windows.Media.Color value ) {
		occCanvas.View.SetBgGradientColors(new(BackgroundColorUp.R, BackgroundColorUp.G, BackgroundColorUp.B), new(value.R, value.G, value.B), true);
	}

	#region ViewCube
	private System.Windows.Media.Color FromHex( string hex ) {
		string colorcode = hex;
		int argb = Int32.Parse(colorcode.Replace("#", ""), System.Globalization.NumberStyles.HexNumber);
		return System.Windows.Media.Color.FromArgb(
									(byte)( ( argb & -16777216 ) >> 0x18 ),
									(byte)( ( argb & 0xff0000 ) >> 0x10 ),
									(byte)( ( argb & 0xff00 ) >> 8 ),
									(byte)( argb & 0xff ));
	}

	ViewCube cube;

	private void UpdateViewCube( ) {
		AISContext.Redisplay(cube, true);
	}

	/// <summary>
	/// 整体尺寸
	/// </summary>
	[ObservableProperty]
	double axesSize;
	partial void OnAxesSizeChanged( double value ) {
		cube.AxesSize = value;
		UpdateViewCube( );
	}
	/// <summary>
	/// 坐标轴半径
	/// </summary>
	[ObservableProperty]
	double axesRadius;
	partial void OnAxesRadiusChanged( double value ) {
		cube.AxesRadius = value;
		UpdateViewCube( );
	}
	/// <summary>
	/// 坐标轴锥体半径
	/// </summary>
	[ObservableProperty]
	double axesConeRadius;
	partial void OnAxesConeRadiusChanged( double value ) {
		cube.AxesConeRadius = value;
		UpdateViewCube( );
	}
	/// <summary>
	/// 坐标轴圆球半径
	/// </summary>
	[ObservableProperty]
	double axesSphereRadius;
	partial void OnAxesSphereRadiusChanged( double value ) {
		cube.AxesSphereRadius = value;
		UpdateViewCube( );
	}
	/// <summary>
	/// 绘制面的边框
	/// </summary>
	[ObservableProperty]
	bool enableFaceBoundary;
	partial void OnEnableFaceBoundaryChanged( bool value ) {
		cube.EnableFaceBoundary = value;
		UpdateViewCube( );
	}

	System.Windows.Media.Color faceBoundaryColor;
	public System.Windows.Media.Color FaceBoundaryColor {
		get { return faceBoundaryColor; }
		set {
			cube.SetFaceBoundaryColor(new(value.R, value.G, value.B));
			UpdateViewCube( );
			SetProperty(ref faceBoundaryColor, value);
		}
	}

	static Dictionary<string, string> GetFontMappings( ) {
		var fontMap = new Dictionary<string, string>();
		const string fontsRegistryPath = @"SOFTWARE\Microsoft\Windows NT\CurrentVersion\Fonts";

		// 打开注册表键
		using var key = Registry.LocalMachine.OpenSubKey(fontsRegistryPath);
		if( key != null ) {
			foreach( var fontName in key.GetValueNames( ) ) {
				// 提取字体文件名，去掉文件扩展名作为缩写
				var fontFile = key.GetValue(fontName)?.ToString();
				if( !string.IsNullOrEmpty(fontFile) ) {
					var shortName = System.IO.Path.GetFileNameWithoutExtension(fontFile);
					fontMap[fontName] = shortName;
				}
			}
		}

		return fontMap;
	}

	/// <summary>
	/// 字体
	/// </summary>
	[ObservableProperty]
	string font;
	partial void OnFontChanged( string value ) {
		// 获取注册表中字体信息
		var fontMap = GetFontMappings();
		var matchedKey = fontMap.Keys
		.FirstOrDefault(k => k.Contains(value, StringComparison.OrdinalIgnoreCase));
		string mappedValue="";
		if( matchedKey != null && fontMap.TryGetValue(matchedKey, out mappedValue) ) {
			log.Debug($"Font '{value}' maps to '{mappedValue}'.");
		} else {
			log.Debug($"Font '{value}' not found in the registry.");
		}
		cube.Font = mappedValue;
		UpdateViewCube( );
	}

	/// <summary>
	/// 文字高度
	/// </summary>
	[ObservableProperty]
	double fontHeight;
	partial void OnFontHeightChanged( double value ) {
		cube.FontHeight = value;
		UpdateViewCube( );
	}

	/// <summary>
	/// 文字颜色
	/// </summary>
	[ObservableProperty]
	System.Windows.Media.Color boxTextColor;
	partial void OnBoxTextColorChanged( System.Windows.Media.Color value ) {
		cube.BoxTextColor = new(value.R, value.G, value.B);
		UpdateViewCube( );
	}

	/// <summary>
	/// 立方体颜色
	/// </summary>
	[ObservableProperty]
	System.Windows.Media.Color boxColor ;
	partial void OnBoxColorChanged( System.Windows.Media.Color value ) {
		Debug.WriteLine(value.R + ", " + value.G + ", " + value.B);
		cube.BoxColor = new(value.R, value.G, value.B);
		UpdateViewCube( );
	}
	/// <summary>	
	/// 立方体透明度
	/// </summary>
	[ObservableProperty]
	double transparency;
	partial void OnTransparencyChanged( double value ) {
		cube.Transparency = value;
		UpdateViewCube( );
	}
	/// <summary>
	/// 立方体面到边的距离
	/// </summary>
	[ObservableProperty]
	double boxFacetExtension;
	partial void OnBoxFacetExtensionChanged( double value ) {
		cube.BoxFacetExtension = value;
		UpdateViewCube( );
	}
	/// <summary>
	/// 盒子圆角半径
	/// </summary>
	[ObservableProperty]
	double boxRoundRadius;
	partial void OnBoxRoundRadiusChanged( double value ) {
		cube.BoxRoundRadius = value;
		UpdateViewCube( );
	}

	/// <summary>
	/// 立方体面到边间空隙
	/// </summary>
	/// <remarks>
	/// 可选择范围
	/// </remarks>
	[ObservableProperty]
	double boxEdgeGap;
	partial void OnBoxEdgeGapChanged( double value ) {
		cube.BoxEdgeGap = value;
		UpdateViewCube( );
	}


	/// <summary>
	/// 选中的轴
	/// </summary>
	[ObservableProperty]
	DatumParts axis;

	public List<DatumParts> AxisParts => [DatumParts.XAxis, DatumParts.YAxis, DatumParts.ZAxis];


	System.Windows.Media.Color axisColor;
	/// <summary>
	/// 轴的颜色
	/// </summary>
	public System.Windows.Media.Color AxisColor {
		get { return FromHex("#FF" + cube.GettAxesColor(Axis).Hex); }
		set {
			cube.SetAxesColor(Axis, new(value.R, value.G, value.B));
			UpdateViewCube( );
			SetProperty(ref axisColor, value);
		}
	}
	System.Windows.Media.Color axisTextColor=new();
	/// <summary>
	/// 轴的文字颜色
	/// </summary>
	public System.Windows.Media.Color AxisTextColor {
		get { return axisTextColor; }
		set {
			cube.SetAxesTextColor(Axis, new(value.R, value.G, value.B));
			UpdateViewCube( );
			SetProperty(ref axisTextColor, value);
		}
	}
	double axisTextHeight;
	/// <summary>
	/// 轴文字高度
	/// </summary>
	public double AxisTextHeight {
		get { return axisTextHeight; }
		set {
			cube.SetAxesTextHeight(Axis, value);
			UpdateViewCube( );
			SetProperty(ref axisTextHeight, value);
		}
	}

	public List<ViewOrientation> OrientationParts => [ViewOrientation.Front, ViewOrientation.Back, ViewOrientation.Left, ViewOrientation.Right, ViewOrientation.Top, ViewOrientation.Bottom];

	[ObservableProperty]
	ViewOrientation orientation;

	string boxSideLabel;
	public string BoxSideLabel {
		get { return cube.GetBoxSideLabel(orientation); }
		set {
			cube.SetBoxSideLabel(orientation, value);
			UpdateViewCube( );
			SetProperty(ref boxSideLabel, value);
		}
	}

	private void UpdateAxesLabel( ) {
		cube.SetAxesLabels((XLabel, YLabel, ZLabel));
		UpdateViewCube( );
	}
	[ObservableProperty]
	string xLabel;
	partial void OnXLabelChanged( string value ) {
		UpdateAxesLabel( );
		UpdateViewCube( );
	}
	[ObservableProperty]
	string yLabel;
	partial void OnYLabelChanged( string value ) {
		UpdateAxesLabel( );
		UpdateViewCube( );
	}
	[ObservableProperty]
	string zLabel;
	partial void OnZLabelChanged( string value ) {
		UpdateAxesLabel( );
		UpdateViewCube( );
	}

	//   Extension::Color GettAxesColor(DatumParts axis);
	//   void SetAxesColor(DatumParts axis, Extension::Color color);
	//   void SetAxesTextColor(DatumParts axis, Extension::Color color);
	//   void SetAxesTextHeight(DatumParts axis, double height);
	//   double GetAxesTextHeight(DatumParts axis);
	//   void SetBoxSideLabel(V3d::ViewOrientation side, System::String^ label);
	//   void SetAxesLabels(System::ValueTuple<System::String^, System::String^, System::String^> texts);
	//System::String^ GetAxisLabels(DatumParts axis);

	#endregion

	#region Command

	[RelayCommand]
	private void FrontView( ) {
		CurrentViewStaute = occCanvas.CurrentViewStaute;
		Debug.WriteLine(
			$"旧: Scale: {occCanvas.CurrentViewStaute.Scale.Round(1)} | Eye: {occCanvas.CurrentViewStaute.Eye} | Prj: {occCanvas.CurrentViewStaute.Projection} | Aspect: {occCanvas.CurrentViewStaute.Aspect} | HightPoint: {occCanvas.CurrentViewStaute.HightPoint} | ScreenCenter: {occCanvas.CurrentViewStaute.ScreenCenter} | Size: {occCanvas.CurrentViewStaute.Size} | ViewPoint: {occCanvas.CurrentViewStaute.ViewPoint} "
		);
		occCanvas.SetViewOrientation(OCCTK.OCC.V3d.ViewOrientation.Front);
		Debug.WriteLine(
			$"新: Scale: {occCanvas.CurrentViewStaute.Scale.Round(1)} | Eye: {occCanvas.CurrentViewStaute.Eye} | Prj: {occCanvas.CurrentViewStaute.Projection} | Aspect: {occCanvas.CurrentViewStaute.Aspect} | HightPoint: {occCanvas.CurrentViewStaute.HightPoint} | ScreenCenter: {occCanvas.CurrentViewStaute.ScreenCenter} | Size: {occCanvas.CurrentViewStaute.Size} | ViewPoint: {occCanvas.CurrentViewStaute.ViewPoint} "
		);
	}

	[RelayCommand]
	private void BackView( ) {
		CurrentViewStaute = occCanvas.CurrentViewStaute;
		occCanvas.SetViewOrientation(OCCTK.OCC.V3d.ViewOrientation.Back);
	}

	[RelayCommand]
	private void TopView( ) {
		CurrentViewStaute = occCanvas.CurrentViewStaute;
		occCanvas.SetViewOrientation(OCCTK.OCC.V3d.ViewOrientation.Top);
	}

	[RelayCommand]
	private void BottomView( ) {
		CurrentViewStaute = occCanvas.CurrentViewStaute;
		occCanvas.SetViewOrientation(OCCTK.OCC.V3d.ViewOrientation.Bottom);
	}

	[RelayCommand]
	private void LeftView( ) {
		CurrentViewStaute = occCanvas.CurrentViewStaute;
		occCanvas.SetViewOrientation(OCCTK.OCC.V3d.ViewOrientation.Left);
	}

	[RelayCommand]
	private void RightView( ) {
		CurrentViewStaute = occCanvas.CurrentViewStaute;
		Debug.WriteLine(
			$"旧: Scale: {occCanvas.CurrentViewStaute.Scale.Round(1)} | Eye: {occCanvas.CurrentViewStaute.Eye} | Prj: {occCanvas.CurrentViewStaute.Projection} | Aspect: {occCanvas.CurrentViewStaute.Aspect} | HightPoint: {occCanvas.CurrentViewStaute.HightPoint} | ScreenCenter: {occCanvas.CurrentViewStaute.ScreenCenter} | Size: {occCanvas.CurrentViewStaute.Size} | ViewPoint: {occCanvas.CurrentViewStaute.ViewPoint} "
		);
		occCanvas.SetViewOrientation(OCCTK.OCC.V3d.ViewOrientation.Right);
		Debug.WriteLine(
			$"新: Scale: {occCanvas.CurrentViewStaute.Scale.Round(1)} | Eye: {occCanvas.CurrentViewStaute.Eye} | Prj: {occCanvas.CurrentViewStaute.Projection} | Aspect: {occCanvas.CurrentViewStaute.Aspect} | HightPoint: {occCanvas.CurrentViewStaute.HightPoint} | ScreenCenter: {occCanvas.CurrentViewStaute.ScreenCenter} | Size: {occCanvas.CurrentViewStaute.Size} | ViewPoint: {occCanvas.CurrentViewStaute.ViewPoint} "
		);
	}

	[RelayCommand]
	private void AxoView( ) {
		CurrentViewStaute = occCanvas.CurrentViewStaute;
		Debug.WriteLine(
			$"旧: Scale: {occCanvas.CurrentViewStaute.Scale.Round(1)} | Eye: {occCanvas.CurrentViewStaute.Eye} | Prj: {occCanvas.CurrentViewStaute.Projection} | Aspect: {occCanvas.CurrentViewStaute.Aspect} | HightPoint: {occCanvas.CurrentViewStaute.HightPoint} | ScreenCenter: {occCanvas.CurrentViewStaute.ScreenCenter} | Size: {occCanvas.CurrentViewStaute.Size} | ViewPoint: {occCanvas.CurrentViewStaute.ViewPoint} "
		);
		occCanvas.SetViewOrientation(OCCTK.OCC.V3d.ViewOrientation.Axo);
		Debug.WriteLine(
			$"新: Scale: {occCanvas.CurrentViewStaute.Scale.Round(1)} | Eye: {occCanvas.CurrentViewStaute.Eye} | Prj: {occCanvas.CurrentViewStaute.Projection} | Aspect: {occCanvas.CurrentViewStaute.Aspect} | HightPoint: {occCanvas.CurrentViewStaute.HightPoint} | ScreenCenter: {occCanvas.CurrentViewStaute.ScreenCenter} | Size: {occCanvas.CurrentViewStaute.Size} | ViewPoint: {occCanvas.CurrentViewStaute.ViewPoint} "
		);
	}

	[RelayCommand]
	private void ResetView( ) {
		Debug.WriteLine(
			$"重设旧: Scale: {occCanvas.CurrentViewStaute.Scale.Round(1)} | Eye: {occCanvas.CurrentViewStaute.Eye} | Prj: {occCanvas.CurrentViewStaute.Projection} | Aspect: {occCanvas.CurrentViewStaute.Aspect} | HightPoint: {occCanvas.CurrentViewStaute.HightPoint} | ScreenCenter: {occCanvas.CurrentViewStaute.ScreenCenter} | Size: {occCanvas.CurrentViewStaute.Size} | ViewPoint: {occCanvas.CurrentViewStaute.ViewPoint} "
		);
		if( CurrentViewStaute == null ) {
			return;
		}
		occCanvas.View.SetViewOrientation(CurrentViewStaute, true);
		Debug.WriteLine(
			$"重设新: Scale: {occCanvas.CurrentViewStaute.Scale.Round(1)} | Eye: {occCanvas.CurrentViewStaute.Eye} | Prj: {occCanvas.CurrentViewStaute.Projection} | Aspect: {occCanvas.CurrentViewStaute.Aspect} | HightPoint: {occCanvas.CurrentViewStaute.HightPoint} | ScreenCenter: {occCanvas.CurrentViewStaute.ScreenCenter} | Size: {occCanvas.CurrentViewStaute.Size} | ViewPoint: {occCanvas.CurrentViewStaute.ViewPoint} "
		);
	}
	#endregion

}
